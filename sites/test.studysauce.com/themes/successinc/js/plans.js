
var calendar = null,
    youtubeReady = false,
    uploads = [];

function resizeCalendar(calendarView) {
    if(!jQuery('#plan').is('.fullcalendar'))
        return;
    if(calendarView.name === 'agendaWeek' || calendarView.name === 'agendaDay')
    {
        // if height is too big for these views, then scrollbars will be hidden
        calendarView.setHeight(jQuery(this).find('.fc-slats').outerHeight() + jQuery(this).find('.fc-day-grid').outerHeight() + jQuery(this).find('.fc-widget-header').outerHeight() + 8);
    }
}

function initPlayer(id)
{
    uploads[uploads.length] = id;
    if(youtubeReady)
        onYouTubeIframeAPIReady();
}

function onYouTubeIframeAPIReady() {
    var player, widget, uploads = window.uploads;
    window.uploads = [];
    youtubeReady = true;
    for(var i in uploads)
    {
        widget = new YT.UploadWidget(uploads[i], {
            width: 210,
            events: {
                //'onUploadSuccess': onUploadSuccess,
                'onProcessingComplete': function (event) {
                    player = new YT.Player('player', {
                        height: 210,
                        width: 210,
                        videoId: event.data.videoId,
                        events: {}
                    });
                }
            }
        });
    }
}

var tag = document.createElement('script');
tag.src = "https://www.youtube.com/iframe_api";
var firstScriptTag = document.getElementsByTagName('script')[0];
firstScriptTag.parentNode.insertBefore(tag, firstScriptTag);

jQuery(document).ready(function () {

    var plans = jQuery('#plan');
    var $ = jQuery;

    $.propHooks.checked = {
        set: function (elem, value, name) {
            var ret = (elem[ name ] = value);
            $(elem).trigger("change");
            return ret;
        }
    };

    plans.on('change', '.sort-by input[type="radio"]', function (evt) {
        var headings = {};
        jQuery('#plan .head').each(function () {
            var head = jQuery(this);
            head.nextUntil('.head,p:last-of-type').each(function () {
                var row = jQuery(this),
                    that = row.find('.field-name-field-class-name .read-only');
                if(typeof headings[that.text()] == 'undefined')
                    headings[that.text()] = row;
                else
                    headings[that.text()] = jQuery.merge(headings[that.text()], row);
                that.text(head.text());
            });
        });
        var rows = [];
        for(var h in headings)
        {
            var hidden = headings[h].filter('.row:not(.hide)').length == 0;
            rows = jQuery.merge(rows, jQuery.merge(jQuery('<div class="head ' + (hidden ? 'hide' : '') + '">' + h + '</div>'), headings[h].detach()));
        }
        plans.find('.head, .row').replaceWith(rows);
    });

    plans.on('click', 'a.return-to-top', function (evt) {
        evt.preventDefault();
        jQuery(this).parent().find('.row:visible').first().scrollintoview({padding: {top:120,bottom:100,left:0,right:0}});
    });

    plans.on('change', '#schedule-historic', function () {
        if(jQuery(this).is(':checked'))
            plans.addClass('show-historic');
        else
            plans.removeClass('show-historic');
    });

    plans.on('change', '.page-dashboard #plan .field-name-field-completed input', function () {
        var that = jQuery(this),
            row = that.parents('.row'),
            event = window.planEvents[row.attr('id').substring(4)];


        $.ajax({
            url: '/node/save/completed',
            type: 'POST',
            dataType: 'json',
            data: {
                cid: event['cid'],
                completed: that.is(':checked'),
                type: event['className'].indexOf('event-type-p') != -1
                    ? 'p'
                    : (event['className'].indexOf('event-type-sr') != -1
                    ? 'sr'
                    : (event['className'].indexOf('event-type-f') != -1
                    ? 'f'
                    : (event['className'].indexOf('event-type-o') != -1
                    ? 'o'
                    : (event['className'].indexOf('event-type-d') != -1
                    ? 'd'
                    : ''))))
            },
            success: function () {
                if(that.is(':checked'))
                    row.addClass('done');
                else
                    row.removeClass('done');
            }
        });
    });

    function renderStrategy()
    {
        var row = jQuery(this).parents('.row'),
            that = row.find('select[name="strategy-select"]'),
            strategy = jQuery('#plan .strategy-' + that.val()).length == 0 // make sure this type of strategy still exists
                ? (/default-([a-z]*)(\s|$)/ig).exec(row.attr('class'))[1]
                : that.val(),
            eid = row.attr('id').substring(4),
            classname = row.find('.field-name-field-class-name .read-only').text();

        // add strategy if they haven't used it before
        if(row.find('.strategy-' + strategy).length == 0 && jQuery('#plan .strategy-' + strategy).length > 0)
        {
            var newStrategy = jQuery('#plan .strategy-' + strategy).first().clone();
            row.append(newStrategy);
            newStrategy.html(newStrategy.html().replace(/\{classname\}/g, classname).replace(/\{eid\}/g, eid));
            if(strategy == 'active')
            {
                // copy values back in to newly rendered fields
                if(typeof window.strategies != 'undefined' && typeof window.strategies[eid] != 'undefined' &&
                   typeof window.strategies[eid]['active'] != 'undefined')
                {
                    newStrategy.find('textarea[name="strategy-skim"]').val(window.strategies[eid]['active'].skim);
                    newStrategy.find('textarea[name="strategy-why"]').val(window.strategies[eid]['active'].why);
                    newStrategy.find('textarea[name="strategy-questions"]').val(window.strategies[eid]['active'].questions);
                    newStrategy.find('textarea[name="strategy-summarize"]').val(window.strategies[eid]['active'].summarize);
                    newStrategy.find('textarea[name="strategy-exam"]').val(window.strategies[eid]['active'].exam);
                }
            }
            if(strategy == 'other')
            {
                // copy values back in to newly rendered fields
                if(typeof window.strategies != 'undefined' && typeof window.strategies[eid] != 'undefined' &&
                   typeof window.strategies[eid]['other'] != 'undefined')
                {
                    newStrategy.find('textarea[name="strategy-notes"]').val(window.strategies[eid]['other'].notes);
                }
            }
            if(strategy == 'spaced')
            {
                var dates = window.planEvents[row.attr('id').substring(4)]['dates'];
                if(typeof dates != 'undefined')
                {
                    var dateStr = dates.map(function ($d, $i) {
                        return '<input type="checkbox" name="strategy-from-' + (604800 * $i) + '-' + eid + '" id="strategy-from-' + (604800 * $i) + '-' + eid + '" value="' + (604800 * $i) + '">' +
                               '<label for="strategy-from-' + (604800 * $i) + '-' + eid + '">' + $d + '</label>';
                    }).join('<br />');
                    newStrategy.find('.strategy-review').append(dateStr);
                }
                if(typeof window.strategies != 'undefined' && typeof window.strategies[eid] != 'undefined' &&
                   typeof window.strategies[eid]['spaced'] != 'undefined')
                {
                    newStrategy.find('textarea[name="strategy-notes"]').val(window.strategies[eid]['spaced'].notes);
                    if(window.strategies[eid]['spaced'].review != '')
                        window.strategies[eid]['spaced'].review.split(',').forEach(function (x, i) {
                            newStrategy.find('input[value="' + x + '"]').prop('checked', true);
                        });
                }
            }
            if(strategy == 'prework')
            {
                newStrategy.find('input[type="checkbox"], input[type="radio"]').each(function () {
                    var that = jQuery(this),
                        oldId = that.attr('id');
                    that.attr('id', oldId + eid);
                    if(that.is('[type="radio"]'))
                        that.attr('name', that.attr('name') + eid);
                    newStrategy.find('label[for="' + oldId + '"]').attr('for', oldId + eid);
                });
                if(typeof window.strategies != 'undefined' && typeof window.strategies[eid] != 'undefined' &&
                    typeof window.strategies[eid]['prework'] != 'undefined')
                {
                    newStrategy.find('textarea[name="strategy-notes"]').val(window.strategies[eid]['prework'].notes);
                    if(window.strategies[eid]['prework'].prepared != '')
                        window.strategies[eid]['prework'].prepared.split(',').forEach(function (x, i) {
                            newStrategy.find('input[value="' + x + '"]').prop('checked', true);
                        });
                }
            }
            if(strategy == 'teach')
            {
                /*$('#plan-' + eid + '-plupload').replaceWith('<iframe id="widget' + eid + '" type="text/html" width="210" height="210" ' +
                    'src="https://www.youtube.com/upload_embed" frameborder="0"></iframe>' +
                    '<script>' +
                    'jQuery(document).ready(function () { initPlayer("widget' + eid + '"); });' +
                    '</script>');
                */

                var uploader = new plupload.Uploader({
                                                         alt_field: 0,
                                                         browse_button: 'plan-' + eid + '-select',
                                                         chunk_size: '512K',
                                                         container: 'plan-' + eid + '-plupload',
                                                         dragdrop: true,
                                                         drop_element: 'plan-' + eid + '-filelist',
                                                         filters: [
                                                             {
                                                                 extensions: 'video/mpeg,mpeg,mpg,mpe,video/quicktime,qt,mov,video/mp4,mp4,video/x-m4v,m4v,video/x-flv,flv,video/x-ms-wmv,wmv,video/avi,avi,video/webm,webm,video/vnd.rn-realvideo,rv,videos',
                                                                 title: 'Allowed extensions'
                                                             }
                                                         ],
                                                         flash_swf_url: '/sites/all/libraries/plupload/js/plupload.flash.swf',
                                                         image_style: 'achievement',
                                                         image_style_path: '/sites/studysauce.com/files/styles/achievement/temporary/',
                                                         max_file_size: '512MB',
                                                         max_files: 1,
                                                         multipart: false,
                                                         multiple_queues: true,
                                                         name: 'plan-' + eid + '-upload',
                                                         runtimes: 'html5,gears,flash,silverlight,browserplus,html4',
                                                         silverlight_xap_url: '/sites/all/libraries/plupload/js/plupload.silverlight.xap',
                                                         title_field: 0,
                                                         unique_names: true,
                                                         upload: 'plan-' + eid + '-upload',
                                                         url: row.find('input[name="plan-path"]').val(),
                                                         urlstream_upload: false
                                                     });
                $('#plan-' + eid + '-select').click(function(e) {
                    uploader.start();
                    e.preventDefault();
                });
                uploader.init();

                uploader.bind('FilesAdded', function(up, files) {
                    $('#plan-' + eid + '-plupload').find('.plup-drag-info').hide(); // Hide info
                    $('#plan-' + eid + '-plupload').find('.plup-upload').show(); // Show upload button

                    // Put files visually into queue
                    $.each(files, function(i, file) {
                        $('#plan-' + eid + '-plupload').find('.plup-filelist table').append('<tr id="' + file.id + '">' +
                                                                                            '<td class="plup-filelist-file">' +  file.name + '</td>' +
                                                                                            '<td class="plup-filelist-size">' + plupload.formatSize(file.size) + '</td>' +
                                                                                            '<td class="plup-filelist-message"></td>' +
                                                                                            '<td class="plup-filelist-remove"><a class="plup-remove-file"></a></td>' +
                                                                                            '</tr>');
                        // Bind remove functionality to files added int oqueue
                        $('#' + file.id + ' .plup-filelist-remove > a').bind('click', function(event) {
                            $('#' + file.id).remove();
                            up.removeFile(file);
                        });
                    });

                    up.refresh(); // Refresh for flash or silverlight
                    up.start();
                    jQuery('#' + up.settings.browse_button).parents('.plupload-container').addClass('uploaded');
                });

                // File is being uploaded
                uploader.bind('UploadProgress', function(up, file) {
                    // Refresh progressbar
                    $('#plan-' + eid + '-plupload').find('.plup-progress').progressbar({value: uploader.total.percent});
                });

                // Event after a file has been uploaded from queue
                uploader.bind('FileUploaded', function(up, file, response) {
                    $('#plan-' + eid + '-plupload').find('.plup-list li').remove();
                    // Respone is object with response parameter so 2x repsone
                    var fileSaved = jQuery.parseJSON(response.response);
                    var delta = $('#plan-' + eid + '-plupload').find('.plup-list li').length;
                    var name = 'plan-plupload[' + delta + ']';

                    // Plupload has weird error handling behavior so we have to check for errors here
                    if (fileSaved.error_message) {
                        $('#' + file.id + ' > .plup-filelist-message').append('<b>Error: ' + fileSaved.error_message + '</b>');
                        up.refresh(); // Refresh for flash or silverlight
                        return;
                    }

                    $('#plan-' + eid + '-plupload').find('.plup-filelist #' + file.id).remove(); // Remove uploaded file from queue
                    var thumb = '<video width="184" height="184" preload="auto" controls="controls" poster="' + fileSaved.secure_uri + '">' +
                        '<source src="' + fileSaved.uri + '" type="video/webm" />';
                    $('#plan-' + eid + '-plupload').addClass('uploaded');
                    $('#plan-' + eid + '-plupload').find('.plup-progress').hide();
                    // Add image thumbnail into list of uploaded items
                    $('#plan-' + eid + '-plupload').find('.plup-list').append(
                        '<li>' +
                            //'<div class="plup-thumb-wrapper"><img src="'+ Drupal.settings.plup[thisID].image_style_path + file.target_name + '" /></div>' +
                        '<div class="plup-thumb-wrapper">' + thumb + '</div>' +
                        '<a class="plup-remove-item"></a>' +
                        '<input type="hidden" name="' + name + '[fid]" value="' + fileSaved.fid + '" />' +
                        '<input type="hidden" name="' + name + '[thumbnail]" value="' + fileSaved.thumbnail + '" />' +
                        '<input type="hidden" name="' + name + '[rename]" value="' + file.name +'" />' +
                        '</li>');
                    // Bind remove functionality to uploaded file
                    var new_element = $('input[name="'+ name +'[fid]"]');
                    $('#plan-' + eid + '-plupload').find('img[src*="empty-play.png"]').remove();
                    var remove_element = $(new_element).siblings('.plup-remove-item');
                    plup_remove_item(remove_element);
                    // Bind resize effect to inputs of uploaded file
                    var text_element = $(new_element).siblings('input.form-text');
                    plup_resize_input(text_element);
                    // Tell Drupal that form has been updated
                    new_element.trigger('formUpdated');
                    newStrategy.find('a[href="#save-strategy"]').first().trigger('click');
                });

                // All fiels from queue has been uploaded
                uploader.bind('UploadComplete', function(up, files) {
                    $('#plan-' + eid + '-plupload').find('.plup-list').sortable('refresh'); // Refresh sortable
                    $('#plan-' + eid + '-plupload').find('.plup-drag-info').show(); // Show info
                });

                if(typeof window.strategies != 'undefined' && typeof window.strategies[eid] != 'undefined' &&
                   typeof window.strategies[eid]['teach'] != 'undefined')
                {
                    newStrategy.find('input[name="strategy-title"]').val(window.strategies[eid]['teach'].title);
                    newStrategy.find('textarea[name="strategy-notes"]').val(window.strategies[eid]['teach'].notes);
                    var delta = $('#plan-' + eid + '-plupload').find('.plup-list li').length;
                    var name = 'plan-plupload[' + delta + ']';
                    if(typeof window.strategies[eid]['teach'].uploads != 'undefined' &&
                        typeof window.strategies[eid]['teach'].uploads[0] != 'undefined')
                    {
                        var thumb = '<img src="' + window.strategies[eid]['teach'].uploads[0].uri + '" title="teaching" />';
                        if(typeof window.strategies[eid]['teach'].uploads[0].play != 'undefined')
                        {
                            thumb = '<video width="184" height="184" preload="auto" controls="controls" poster="' + window.strategies[eid]['teach'].uploads[0].uri + '">' +
                                    '<source src="' + window.strategies[eid]['teach'].uploads[0].play + '" type="video/webm" /></video>';
                            $('#plan-' + eid + '-plupload').addClass('uploaded');
                        }
                        $('#plan-' + eid + '-plupload').find('img[src*="empty-play.png"]').remove();

                        $('#plan-' + eid + '-plupload').find('.plup-list').append(
                            '<li>' +
                                //'<div class="plup-thumb-wrapper"><img src="'+ Drupal.settings.plup[thisID].image_style_path + file.target_name + '" /></div>' +
                                '<div class="plup-thumb-wrapper">' + thumb + '</div>' +
                                '<a class="plup-remove-item"></a>' +
                                '<input type="hidden" name="' + name + '[fid]" value="' + window.strategies[eid]['teach'].uploads[0].fid + '" />' +
                                '<input type="hidden" name="' + name + '[thumbnail]" value="' + window.strategies[eid]['teach'].uploads[0].thumbnail + '" />' +
                                '</li>');
                        // Bind remove functionality to uploaded file
                    }
                }

            }
        }

        row.find('.strategy-spaced, .strategy-active, .strategy-teach, .strategy-other, .strategy-prework').hide();
        row.find('.strategy-' + strategy).show();

    }

    plans.on('change', 'select[name="strategy-select"]', renderStrategy);

    plans.on('click', 'a[href="#expand"]', function (evt) {
        evt.preventDefault();
        var row = jQuery(this).parents('.row');
        row.toggleClass('expanded').scrollintoview();
    });

    plans.on('click', 'a[href="#save-strategy"]', function (evt) {
        evt.preventDefault();
        var that = jQuery(this),
            row = that.parents('.row'),
            eid = row.attr('id').substring(4),
            strategies = [];
        row.find('.strategy-active, .strategy-spaced, .strategy-teach, .strategy-other, .strategy-prework').each(function () {
            var that = jQuery(this);
            if(that.is('.strategy-active'))
            {
                var strategy = {
                    type: 'active',
                    eid:      eid,
                    skim:      that.find('textarea[name="strategy-skim"]').val() || '',
                    why:       that.find('textarea[name="strategy-why"]').val() || '',
                    questions: that.find('textarea[name="strategy-questions"]').val() || '',
                    summarize: that.find('textarea[name="strategy-summarize"]').val() || '',
                    exam:      that.find('textarea[name="strategy-exam"]').val() || ''
                };
                if(strategy.skim.trim() == '' &&
                   strategy.why.trim() == '' &&
                   strategy.questions.trim() == '' &&
                   strategy.summarize.trim() == '' &&
                   strategy.exam.trim() == '')
                strategy = {
                    type: 'active',
                    eid:eid,
                    remove:true
                };
                strategies[strategies.length] = strategy;
                jQuery('#home-active').attr('checked', 'checked');
                if(jQuery('#home').find('input[type="checkbox"]:checked').length == jQuery('#home').find('input[type="checkbox"]').length - 1)
                    jQuery('#home-tasks-checklist').attr('checked', 'checked');
            }
            else if(that.is('.strategy-teach'))
            {
                var inputs = that.find('input[name^="plan-plupload"]'),
                    uploads = [];

                jQuery.each(inputs, function () {
                    var matches = (/\[([0-9]+)\]\[([a-z]+)\]/ig).exec(jQuery(this).attr('name'));
                    if(typeof uploads[parseInt(matches[1])] == 'undefined')
                        uploads[parseInt(matches[1])] = {};
                    uploads[parseInt(matches[1])][matches[2]] = jQuery(this).val();
                });

                var strategy = {
                    type: 'teach',
                    eid:  eid,
                    title: that.find('input[name="strategy-title"]').val() || '',
                    notes: that.find('textarea[name="strategy-notes"]').val() || '',
                    uploads: uploads
                };
                if(strategy.title.trim() == '' &&
                   strategy.notes.trim() == '' &&
                   strategy.uploads.length == 0)
                    strategy = {
                        type:   'teach',
                        eid:   eid,
                        remove: true
                    };
                strategies[strategies.length] = strategy;
                jQuery('#home-teach').attr('checked', 'checked');
                if(jQuery('#home').find('input[type="checkbox"]:checked').length == jQuery('#home').find('input[type="checkbox"]').length - 1)
                    jQuery('#home-tasks-checklist').attr('checked', 'checked');
            }
            else if(that.is('.strategy-spaced'))
            {
                var review = [];
                that.find('input[name^="strategy-from"]:checked').each(function () {
                    review[review.length] = jQuery(this).val();
                });
                var strategy = {
                    type:   'spaced',
                    eid:   eid,
                    notes:  that.find('textarea[name="strategy-notes"]').val() || '',
                    review: review.join(',') || ''
                };
                if(strategy.notes.trim() == '' &&
                   strategy.review.trim() == '')
                    strategy = {
                        type: 'spaced',
                        eid:eid,
                        remove:true
                    };
                strategies[strategies.length] = strategy;
                jQuery('#home-spaced').attr('checked', 'checked');
                if(jQuery('#home').find('input[type="checkbox"]:checked').length == jQuery('#home').find('input[type="checkbox"]').length - 1)
                    jQuery('#home-tasks-checklist').attr('checked', 'checked');
            }
            else if(that.is('.strategy-other'))
            {
                var strategy = {
                    type:  'other',
                    eid:  eid,
                    notes: that.find('textarea[name="strategy-notes"]').val() || ''
                };
                if(strategy.notes.trim() == '')
                    strategy = {
                        type:   'other',
                        eid:   eid,
                        remove: true
                    };
                strategies[strategies.length] = strategy;
            }
            else if(that.is('.strategy-prework'))
            {
                var prepare = [];
                that.find('input[name^="strategy-"]:checked').each(function () {
                    prepare[prepare.length] = jQuery(this).val();
                });
                var strategy = {
                    type:   'prework',
                    eid:    eid,
                    notes:   that.find('textarea[name="strategy-notes"]').val() || '',
                    prepared: prepare.join(',') || ''
                };
                if(strategy.notes.trim() == '' &&
                    strategy.prepare.trim() == '')
                    strategy = {
                        type:   'prework',
                        eid:   eid,
                        remove: true
                    };
                strategies[strategies.length] = strategy;
            }

        });

        $.ajax({
                   url: '/node/save/strategies',
                   type: 'POST',
                   dataType: 'json',
                   data: {
                       // save selected strategy per event
                       'default':row.find('select[name="strategy-select"]').val() != '_none'
                           ? row.find('select[name="strategy-select"]').val()
                           : null,
                       strategies: strategies
                   },
                   success: function (data) {
                       row.find('div[class^="strategy"]').removeClass('saved').addClass('unsaved');
                   }
               });

    });

    plans.on('keyup', 'div[class^="strategy"] input[type="text"], div[class^="strategy"] textarea', function () {
        jQuery(this).parents('div[class^="strategy"]').removeClass('saved').addClass('unsaved');
    });
    plans.on('change', 'div[class^="strategy"] input[type="checkbox"], div[class^="strategy"] input[type="radio"], div[class^="strategy"] input[type="text"], div[class^="strategy"] textarea', function () {
        jQuery(this).parents('div[class^="strategy"]').removeClass('saved').addClass('unsaved');
    });

    plans.on('click', '.page-dashboard #plan .field-name-field-assignment,' +
                                '.page-dashboard #plan .field-name-field-class-name,' +
                                '.page-dashboard #plan .field-name-field-percent', function () {
        var row = $(this).parents('.row'),
            strategy = (/default-([a-z]*)(\s|$)/ig).exec(row.attr('class'))[1],
            eid = row.attr('id').substring(4),
            cid = (/cid([0-9]+)(\s|$)/ig).exec(row.attr('class')),
            classname = row.find('.field-name-field-class-name .read-only').text();
        row.toggleClass('selected');

        // add mini-checkin if class number is set
        if(cid != null && row.find('.mini-checkin').length == 0 && strategy != 'other' &&
            jQuery('#plan .mini-checkin').length > 0)
        {
            var newMiniCheckin = jQuery('#plan .mini-checkin').first().clone();
            row.append(newMiniCheckin);
            newMiniCheckin.html(newMiniCheckin.html().replace(/\{classname\}/g, classname).replace(/\{eid\}/g, eid));
        }

        // add the default strategy
        if(cid != null && row.find('.strategy-' + strategy).length == 0 &&
           jQuery('#plan .strategy-' + strategy).length > 0 && strategy != 'other' && strategy != 'prework')
        {
            var newStrategySelect = jQuery('#plan .field-select-strategy').first().clone();
            row.append(newStrategySelect);
        }

        // display the default strategy
        renderStrategy.apply(this);

        //
        if(!row.is('.selected'))
            row.find('.strategy-spaced, .strategy-active, .strategy-teach, .strategy-other, .strategy-prework').hide();
        else
            row.find('.mini-checkin:visible, .strategy-spaced:visible, .strategy-active:visible, .strategy-teach:visible, .strategy-other:visible, .strategy-prework:visible').first().scrollintoview({padding: {top:120,bottom:100,left:0,right:0}});
    });

    var date = new Date(),
        original;
    var isInitialized = false,
        initialize = function () {
            if (!isInitialized)
            {
                // find min an max time
                var early = -1,
                    morning = 10,
                    late = 18;
                for(var eid in window.planEvents)
                {
                    var s = new Date(window.planEvents[eid].start),
                        e = new Date(window.planEvents[eid].end);
                    window.planEvents[eid].start = s;
                    window.planEvents[eid].end = e;

                    if(window.planEvents[eid].allDay ||
                        window.planEvents[eid].className.indexOf('event-type-z') > -1 ||
                        window.planEvents[eid].className.indexOf('event-type-m') > -1)
                        continue;
                    if(e.getHours() < 5 && e.getHours() > early)
                        early = e.getHours();
                    if(e.getHours() > late)
                        late = e.getHours();
                    if(s.getHours() > 3 && s.getHours() < morning)
                        morning = s.getHours();
                }

                // use early morning as end time
                var min,max;
                if(early > -1)
                    max = (24 + early + 1) + ':00:00';
                else
                    max = (late + 1) + ':00:00';
                min = morning + ':00:00';

                calendar = $('#calendar').fullCalendar(
                    {
                        minTime: min,
                        maxTime: max,
                        titleFormat: 'MMMM',
                        editable: true,
                        draggable: true,
                        aspectRatio: 1.9,
                        height:500,
                        timezone: 'local',
                        timeslotsPerHour: 4,
                        slotEventOverlap: false,
                        slotMinutes: 15,
                        firstHour: new Date().getHours(),
                        eventRender: function (event, element, view) {
                            element.find('.fc-title').html(event.title);
                            return true;
                        },
                        header: {
                            left: '',
                            center: '',
                            right: 'prev,next today'
                        },
                        defaultView: 'agendaWeek',
                        selectable: false,
                        events: function (start, end, timezone, callback) {
                            var events = [],
                                s = (start.unix() - 86400) * 1000,
                                e = (end.unix() + 86400) * 1000;
                            //var early = 0,
                            //    morning = 9,
                            //    late = 18;
                            for(var eid in window.planEvents)
                            {
                                if(window.planEvents[eid].start.getTime() > s && window.planEvents[eid].end.getTime() < e)
                                {
                                    events[events.length] = window.planEvents[eid];

                            //        if(window.planEvents[eid].allDay)
                            //            continue;
                            //        if(e.getHours() < 5 && e.getHours() > early)
                            //            early = e.getHours();
                            //        if(e.getHours() > late)
                            //            late = e.getHours();
                            //        if(s.getHours() > 5 && s.getHours() < morning)
                            //            morning = s.getHours();
                                }
                            }
                            //var newMin,newMax;
                            //if(early > 0)
                            //    newMax = (24 + early) + ':00:00';
                            //else
                            //    newMax = (late) + ':00:00';
                            //newMin = morning + ':00:00';
                            //if(newMin != min)
                            //{
                            //    $('#calendar').fullCalendar('option', 'minTime', min = newMin);
                            //}
                            //if(newMax != max)
                            //{
                            //    $('#calendar').fullCalendar('option', 'maxTime', max = newMax);
                            //}

                            callback(events);
                        },
                        eventClick: function(calEvent, jsEvent, view) {
                            // var eid =  calEvent._id.substring(3);
                            // change the border color just for fun
                            if(plans.find('#eid-' + calEvent.cid).length > 0)
                                plans.find('#eid-' + calEvent.cid).scrollintoview({padding: {top:120,bottom:100,left:0,right:0}});

                        },
                        eventDragStart: function (event, jsEvent, ui, view) {
                            original = new Date(event.start.unix() * 1000);
                        },
                        eventDragStop: function (event, jsEvent, ui, view) {
                        },
                        eventDrop: function (event, delta, revertFunc, jsEvent, ui, view) {

                            if(event.allDay)
                            {
                                revertFunc();
                                return;
                            }

                            var prev, next;

                            for (var i in window.planEvents) {
                                if ((window.planEvents[i].className[1] == event.className[1] ||
                                     window.planEvents[i].className[0] == event.className[1]) &&
                                    window.planEvents[i].className[0] != 'event-type-r' &&
                                    window.planEvents[i].className[0] != 'event-type-h' &&
                                    window.planEvents[i].className[0] != 'event-type-d' &&
                                    i != event.cid)
                                {
                                    // TODO: update this if classes are draggable
                                    if ((next == null || window.planEvents[i].start.getTime() < next.start.getTime()) &&
                                        window.planEvents[i].start.getTime() > original.getTime())
                                    {
                                        next = window.planEvents[i];
                                    }
                                    else if((prev == null || window.planEvents[i].start.getTime() > prev.start.getTime()) &&
                                        window.planEvents[i].start.getTime() < original.getTime())
                                        prev = window.planEvents[i];
                                }
                            }

                            // check for last event of type or first event of type
                            if ((prev != null && event.start.getTime() < prev.end.getTime()) ||
                                (next != null && event.end.getTime() > next.start.getTime()))
                            {
                                revertFunc();
                                return;
                            }


                            $.ajax({
                                       url: '/node/move/schedule',
                                       type: 'POST',
                                       dataType: 'json',
                                       data: {
                                           cid: event['cid'],
                                           start: event['start'].toJSON(),
                                           end: event['end'].toJSON(),
                                           type: event['className'].indexOf('event-type-p') != -1
                                               ? 'p'
                                               : (event['className'].indexOf('event-type-sr') != -1
                                               ? 'sr'
                                               : (event['className'].indexOf('event-type-f') != -1
                                               ? 'f'
                                               : (event['className'].indexOf('event-type-o') != -1
                                               ? 'o'
                                               : (event['className'].indexOf('event-type-d') != -1
                                               ? 'd'
                                               : ''))))
                                       },
                                       error: revertFunc,
                                       success: function (data) {
                                           // update calendar events
                                           window.planEvents = data.events;
                                           for(var eid in window.planEvents)
                                           {
                                               var s = new Date(window.planEvents[eid].start),
                                                   e = new Date(window.planEvents[eid].end);
                                               window.planEvents[eid].start = s;
                                               window.planEvents[eid].end = e;
                                           }
                                           if(calendar != null && typeof calendar.fullCalendar != 'undefined')
                                               calendar.fullCalendar('refetchEvents');
                                       }
                                   });
                        }
                    });

                //calendar.find('.fc-agenda-slots tr:nth-child(4n-3) .fc-agenda-axis').eq(new Date().getHours()).scrollintoview({padding: {top:120,bottom:500,left:0,right:0}});

                isInitialized = true;
            }
        },
        d = date.getDate(),
        m = date.getMonth(),
        y = date.getFullYear(),
        dragSource = [];

    // The calendar needs to be in view for sizing information.  This will not initialize when display:none;, so instead
    //   we will activate the calendar only once, when the menu is clicked, this assumes #hash detection works, and
    //   it triggers the menu clicking
    if(jQuery('#plan:visible').length > 0)
    {
        setTimeout(initialize, 1000);
    }
    jQuery('body').on('click', 'a[href="#plan"]', function () {
        initialize();
        if($('#calendar:visible').length > 0)
            $('#calendar').fullCalendar('refetchEvents');
    });

    jQuery.fn.updatePlan = function(events)
    {
        window.planEvents = events;
        $('#calendar').fullCalendar('destroy');
        isInitialized = false;
        if(jQuery('#plan:visible').length > 0)
        {
            setTimeout(initialize, 1000);
        }
    };

    plans.on('click', '.sort-by a[href="#expand"]', function () {
        if(plans.is('.fullcalendar'))
        {
            plans.removeClass('fullcalendar');
            $('#calendar').fullCalendar('option', 'height', 500);
        }
        else
        {
            plans.addClass('fullcalendar');
            $('#calendar').fullCalendar('option', 'height', 'auto');
        }
    });
});

//@ sourceURL=plans.js
//# sourceURL=plans.js
