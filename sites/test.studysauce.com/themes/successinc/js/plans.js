var calendar = null,
    youtubeReady = false,
    uploads = [];

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
                className: event['title'],
                completed: that.is(':checked'),
                type: event['className'].indexOf('p-event') != -1
                    ? 'p'
                    : (event['className'].indexOf('sr-event') != -1
                    ? 'sr'
                    : (event['className'].indexOf('free-event') != -1
                    ? 'f'
                    : (event['className'].indexOf('other-event') != -1
                    ? 'o'
                    : (event['className'].indexOf('deadline-event') != -1
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
            classname = row.find('.field-name-field-class-name .read-only').text(),
            title = row.find('input[name="plan-title"]').val();

        // add strategy if they haven't used it before
        if(row.find('.strategy-' + strategy).length == 0 && jQuery('#plan .strategy-' + strategy).length > 0)
        {
            var newStrategy = jQuery('#plan .strategy-' + strategy).first().clone();
            row.append(newStrategy);
            newStrategy.html(newStrategy.html().replace(/\{classname\}/g, classname).replace(/\{eid\}/g, eid));
            if(strategy == 'active')
            {
                // copy values back in to newly rendered fields
                if(typeof window.strategies != 'undefined' && typeof window.strategies[title] != 'undefined' &&
                   typeof window.strategies[title]['active'] != 'undefined')
                {
                    newStrategy.find('textarea[name="strategy-skim"]').val(window.strategies[title]['active'].skim);
                    newStrategy.find('textarea[name="strategy-why"]').val(window.strategies[title]['active'].why);
                    newStrategy.find('textarea[name="strategy-questions"]').val(window.strategies[title]['active'].questions);
                    newStrategy.find('textarea[name="strategy-summarize"]').val(window.strategies[title]['active'].summarize);
                    newStrategy.find('textarea[name="strategy-exam"]').val(window.strategies[title]['active'].exam);
                }
            }
            if(strategy == 'other')
            {
                // copy values back in to newly rendered fields
                if(typeof window.strategies != 'undefined' && typeof window.strategies[title] != 'undefined' &&
                   typeof window.strategies[title]['other'] != 'undefined')
                {
                    newStrategy.find('textarea[name="strategy-notes"]').val(window.strategies[title]['other'].notes);
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
                if(typeof window.strategies != 'undefined' && typeof window.strategies[title] != 'undefined' &&
                   typeof window.strategies[title]['spaced'] != 'undefined')
                {
                    newStrategy.find('textarea[name="strategy-notes"]').val(window.strategies[title]['spaced'].notes);
                    if(window.strategies[title]['spaced'].review != '')
                        window.strategies[title]['spaced'].review.split(',').forEach(function (x, i) {
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
                if(typeof window.strategies != 'undefined' && typeof window.strategies[title] != 'undefined' &&
                    typeof window.strategies[title]['prework'] != 'undefined')
                {
                    newStrategy.find('textarea[name="strategy-notes"]').val(window.strategies[title]['prework'].notes);
                    if(window.strategies[title]['prework'].prepared != '')
                        window.strategies[title]['prework'].prepared.split(',').forEach(function (x, i) {
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

                if(typeof window.strategies != 'undefined' && typeof window.strategies[title] != 'undefined' &&
                   typeof window.strategies[title]['teach'] != 'undefined')
                {
                    newStrategy.find('input[name="strategy-title"]').val(window.strategies[title]['teach'].title);
                    newStrategy.find('textarea[name="strategy-notes"]').val(window.strategies[title]['teach'].notes);
                    var delta = $('#plan-' + eid + '-plupload').find('.plup-list li').length;
                    var name = 'plan-plupload[' + delta + ']';
                    if(typeof window.strategies[title]['teach'].uploads != 'undefined' &&
                        typeof window.strategies[title]['teach'].uploads[0] != 'undefined')
                    {
                        var thumb = '<img src="' + window.strategies[title]['teach'].uploads[0].uri + '" title="teaching" />';
                        if(typeof window.strategies[title]['teach'].uploads[0].play != 'undefined')
                        {
                            thumb = '<video width="184" height="184" preload="auto" controls="controls" poster="' + window.strategies[title]['teach'].uploads[0].uri + '">' +
                                    '<source src="' + window.strategies[title]['teach'].uploads[0].play + '" type="video/webm" />';
                            $('#plan-' + eid + '-plupload').addClass('uploaded');
                        }
                        $('#plan-' + eid + '-plupload').find('img[src*="empty-play.png"]').remove();

                        $('#plan-' + eid + '-plupload').find('.plup-list').append(
                            '<li>' +
                                //'<div class="plup-thumb-wrapper"><img src="'+ Drupal.settings.plup[thisID].image_style_path + file.target_name + '" /></div>' +
                                '<div class="plup-thumb-wrapper">' + thumb + '</div>' +
                                '<a class="plup-remove-item"></a>' +
                                '<input type="hidden" name="' + name + '[fid]" value="' + window.strategies[title]['teach'].uploads[0].fid + '" />' +
                                '<input type="hidden" name="' + name + '[thumbnail]" value="' + window.strategies[title]['teach'].uploads[0].thumbnail + '" />' +
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
            strategies = [];
        row.find('.strategy-active, .strategy-spaced, .strategy-teach, .strategy-other, .strategy-prework').each(function () {
            var that = jQuery(this);
            if(that.is('.strategy-active'))
            {
                var strategy = {
                    type: 'active',
                    name:      row.find('input[name="plan-title"]').val(),
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
                    name:row.find('input[name="plan-title"]').val(),
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
                    name:  row.find('input[name="plan-title"]').val(),
                    title: that.find('input[name="strategy-title"]').val() || '',
                    notes: that.find('textarea[name="strategy-notes"]').val() || '',
                    uploads: uploads
                };
                if(strategy.title.trim() == '' &&
                   strategy.notes.trim() == '' &&
                   strategy.uploads.length == 0)
                    strategy = {
                        type:   'teach',
                        name:   row.find('input[name="plan-title"]').val() || '',
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
                    name:   row.find('input[name="plan-title"]').val(),
                    notes:  that.find('textarea[name="strategy-notes"]').val() || '',
                    review: review.join(',') || ''
                };
                if(strategy.notes.trim() == '' &&
                   strategy.review.trim() == '')
                    strategy = {
                        type: 'spaced',
                        name:row.find('input[name="plan-title"]').val(),
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
                    name:  row.find('input[name="plan-title"]').val(),
                    notes: that.find('textarea[name="strategy-notes"]').val() || ''
                };
                if(strategy.notes.trim() == '')
                    strategy = {
                        type:   'other',
                        name:   row.find('input[name="plan-title"]').val(),
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
                    name:    row.find('input[name="plan-title"]').val(),
                    notes:   that.find('textarea[name="strategy-notes"]').val() || '',
                    prepared: prepare.join(',') || ''
                };
                if(strategy.notes.trim() == '' &&
                    strategy.prepare.trim() == '')
                    strategy = {
                        type:   'prework',
                        name:   row.find('input[name="plan-title"]').val(),
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
            row.find('.strategy-spaced:visible, .strategy-active:visible, .strategy-teach:visible, .strategy-other:visible, .strategy-prework:visible').first().scrollintoview({padding: {top:120,bottom:100,left:0,right:0}});
    });

    var date = new Date();
    var isInitialized = false,
        initialize = function () {
            if (!isInitialized)
            {
                calendar = $('#calendar').fullCalendar(
                    {
                        titleFormat: 'MMMM',
                        editable: true,
                        draggable: true,
                        aspectRatio: 1.9,
                        height:500,
                        timeslotsPerHour: 4,
                        slotEventOverlap: false,
                        slotMinutes: 15,
                        firstHour: new Date().getHours(),
                        eventRender: function (event, element) {
                            element.find('.fc-event-title').html(event.title);
                            return true;
                        },
                        header: {
                            left: '',
                            center: '',
                            right: 'prev,next today'
                        },
                        defaultView: 'agendaWeek',
                        selectable: false,
                        events: function (start, end, callback) {
                            for(var eid in window.planEvents)
                            {
                                window.planEvents[eid].start = new Date(window.planEvents[eid].start);
                                window.planEvents[eid].end = new Date(window.planEvents[eid].end);
                            }
                            callback(window.planEvents);
                        },
                        eventClick: function(calEvent, jsEvent, view) {
                            var eid = window.planEvents.indexOf(calEvent);
                            // var eid =  calEvent._id.substring(3);
                            // change the border color just for fun
                            if(plans.find('#eid-' + eid).length > 0)
                                plans.find('#eid-' + eid).scrollintoview({padding: {top:120,bottom:100,left:0,right:0}});

                        },
                        eventDragStart: function (event, jsEvent, ui, view) {
                            var prev, next;
                            for (var i in window.planEvents)
                                if (window.planEvents[i].start < event.start &&
                                    window.planEvents[i].className[1] == event.className[0])
                                    prev = window.planEvents[i];
                                else if (window.planEvents[i].className[1] == event.className[0]) {
                                    next = window.planEvents[i];
                                    break;
                                }
                            /*var dragDropEvent = {
                             allDay:false,
                             className: ['drag-event'],
                             start: prev.end,
                             end: next.start,
                             editable: false,
                             title: 'Drag &amp; Drop',
                             _id: 'drag-and-drop'
                             };*/
                            //dragSource = [dragDropEvent];
                            //calendar.fullCalendar( 'renderEvent',  dragDropEvent, true);
                            //view.setEventData(events); // for View.js, TODO: unify with renderEvents
                            //window.planEvents.push(dragDropEvent);
                            //view.renderEvents(window.planEvents, 'drag-and-drop'); // actually render the DOM elements

                        },
                        eventDragStop: function (event, jsEvent, ui, view) {
                        },
                        eventDrop: function (event, dayDelta, minuteDelta, allDay, revertFunc) {
                            //calendar.fullCalendar( 'removeEventSource',  dragSource);
                            var prev, next,
                                lastTime = event.start.getTime() - dayDelta * 86400 * 1000 - minuteDelta * 60 * 1000;
                            for (var i in window.planEvents) {
                                if ((window.planEvents[i].className[1] == event.className[0] ||
                                     window.planEvents[i].className[0] == event.className[0]) &&
                                    window.planEvents[i] != event) {
                                    // TODO: update this if classes are draggable
                                    if (window.planEvents[i].start.getTime() < lastTime &&
                                        (prev == null || window.planEvents[i].end.getTime() > prev.end.getTime()))
                                        prev = window.planEvents[i];

                                    if (window.planEvents[i].start.getTime() > lastTime &&
                                        (next == null || window.planEvents[i].start.getTime() < next.start.getTime()))
                                        next = window.planEvents[i];
                                }
                            }
                            // + dayDelta * 86400 * 1000 + minuteDelta * 60 * 1000
                            if (event.start.getTime() < prev.end.getTime() ||
                                event.end.getTime() > next.start.getTime())
                                revertFunc();
                            //alert(
                            //    event.title + " was moved " +
                            //    dayDelta + " days and " +
                            //    minuteDelta + " minutes."
                            //);

                            //if (allDay) {
                            //alert("Event is now all-day");
                            //}else{
                            //alert("Event has a time-of-day");
                            //}

                            //if (!confirm("Are you sure about this change?")) {
                            //revertFunc();
                            //}
                            $.ajax({
                                       url: '/node/save/schedule',
                                       type: 'POST',
                                       dataType: 'json',
                                       data: {
                                           className: event['title'],
                                           start: event['start'].toJSON(),
                                           end: event['end'].toJSON(),
                                           dotw: '',
                                           type: event['className'].indexOf('p-event') != -1
                                               ? 'p'
                                               : (event['className'].indexOf('sr-event') != -1
                                               ? 'sr'
                                               : (event['className'].indexOf('free-event') != -1
                                               ? 'f'
                                               : ''))
                                       },
                                       error: revertFunc,
                                       success: function (data) {
                                           // update calendar events
                                           window.planEvents = data.events;
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
});

