
if(typeof plup_remove_item == 'undefined')
{
    // Bind removing of file to selector elements
    function plup_remove_item(selector) {
        jQuery(selector).bind('click', function(event) {
            var parent = jQuery(this).parent();
            var parentsParent = parent.parent();
            parent.remove();
            parentsParent.trigger('formUpdated');
        });
    }
}

if(typeof plup_resize_input == 'undefined')
{
    // Bind resize effect on title and alt fields on focus
    function plup_resize_input(selector) {
        var w = jQuery(selector).outerWidth();
        if (w < 300) {
            jQuery(selector).bind('focus', function(event) {
                jQuery(this).css('z-index', 10).animate({'width': '300px'}, 300);
            });
            jQuery(selector).bind('blur', function(event) {
                jQuery(this).removeAttr('style');
            });
        }
    }
}

jQuery(document).ready(function() {

    var $ = jQuery,
        goals = jQuery('#goals');

    var uploader = new plupload.Uploader({
        alt_field: 0,
        browse_button: "goals-plupload-select",
        chunk_size: "512K",
        container: "goals-plupload",
        dragdrop: true,
        drop_element: "goal-plupload-filelist",
        filters: [
            {
                extensions: "png,gif,jpg,jpeg,images",
                title: "Allowed extensions"
            }
        ],
        flash_swf_url: "/sites/all/libraries/plupload/js/plupload.flash.swf",
        image_style: "achievement",
        image_style_path: "/sites/studysauce.com/files/styles/achievement/temporary/",
        max_file_size: "512MB",
        max_files: 1,
        multipart: false,
        multiple_queues: true,
        name: "goals-plupload-upload",
        runtimes: "html5,gears,flash,silverlight,browserplus,html4",
        silverlight_xap_url: "/sites/all/libraries/plupload/js/plupload.silverlight.xap",
        title_field: 0,
        unique_names: true,
        upload: "goals-plupload-upload",
        url: jQuery('#goal-upload-path').val(),
        urlstream_upload: false
    });
    uploader.init();

    uploader.bind('FilesAdded', function(up, files) {
        $('#goals-plupload').find('.plup-drag-info').hide(); // Hide info
        $('#goals-plupload').find('.plup-upload').show(); // Show upload button

        // Put files visually into queue
        $.each(files, function(i, file) {
            $('#goals-plupload').find('.plup-filelist table').append('<tr id="' + file.id + '">' +
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
        $('#goals-plupload').find('.plup-progress').progressbar({value: uploader.total.percent});
    });

    // Event after a file has been uploaded from queue
    uploader.bind('FileUploaded', function(up, file, response) {
        // Respone is object with response parameter so 2x repsone
        $('#goals-plupload').find('.plup-list li').remove();
        var fileSaved = jQuery.parseJSON(response.response);
        var delta = $('#goals-plupload').find('.plup-list li').length;
        var name = 'goals-plupload[' + delta + ']';

        // Plupload has weird error handling behavior so we have to check for errors here
        if (fileSaved.error_message) {
            $('#' + file.id + ' > .plup-filelist-message').append('<b>Error: ' + fileSaved.error_message + '</b>');
            up.refresh(); // Refresh for flash or silverlight
            return;
        }

        $('#goals-plupload').find('.plup-filelist #' + file.id).remove(); // Remove uploaded file from queue
        // Add image thumbnail into list of uploaded items
        $('#goals-plupload').find('.plup-list').append(
            '<li>' +
                //'<div class="plup-thumb-wrapper"><img src="'+ Drupal.settings.plup[thisID].image_style_path + file.target_name + '" /></div>' +
            '<div class="plup-thumb-wrapper"><img src="'+ fileSaved.secure_uri + '" title="'+ Drupal.checkPlain(file.target_name) +'" /></div>' +
            '<a class="plup-remove-item"></a>' +
            '<input type="hidden" name="' + name + '[fid]" value="' + fileSaved.fid + '" />' +
            '<input type="hidden" name="' + name + '[weight]" value="' + delta + '" />' +
            '<input type="hidden" name="' + name + '[rename]" value="' + file.name +'" />' +
            '</li>');
        // Bind remove functionality to uploaded file
        var new_element = $('input[name="'+ name +'[fid]"]');
        var remove_element = $(new_element).siblings('.plup-remove-item');
        plup_remove_item(remove_element);
        // Bind resize effect to inputs of uploaded file
        var text_element = $(new_element).siblings('input.form-text');
        plup_resize_input(text_element);
        // Tell Drupal that form has been updated
        new_element.trigger('formUpdated');
    });

    // All fiels from queue has been uploaded
    uploader.bind('UploadComplete', function(up, files) {
        $('#goals-plupload').find('.plup-list').sortable('refresh'); // Refresh sortable
        $('#goals-plupload').find('.plup-drag-info').show(); // Show info
    });

    $.fn.goalsFunc = function () {
        jQuery(this).each(function (i, r) {
            var that = jQuery(r),
                valid = true;
            that.find('.field-name-field-read-only input').each(function () {
                if(jQuery(this).val() == '0')
                    that.addClass('edit');
            });
            that.find('.field-name-field-type input').trigger('change');
            if(that.find('select:visible').val() == '_none' ||
               that.find('.field-name-field-reward textarea').val().trim() == '')
                valid = false;
            if(!valid)
                that.removeClass('valid').addClass('invalid');
            else
                that.removeClass('invalid').addClass('valid');
        });
    };

    goals.on('change', '.row select, .row textarea', function () {
        jQuery(this).parents('.row').goalsFunc();
    });
    goals.on('keyup', '.row textarea', function () {
        jQuery(this).parents('.row').goalsFunc();
    });

    goals.on('click', 'a[href="#edit-reward"]', function (evt) {
        evt.preventDefault();
        var row = jQuery(this).parents('.row');
        row.addClass('edit').find('.field-name-field-read-only input').val(0);
        row.goalsFunc();
        jQuery('#goals').addClass('edit-goal');
    });

    goals.on('click', 'a[href="#cancel-incentive"]', function (evt) {
        evt.preventDefault();
        jQuery(this).parents('.row').removeClass('edit');
        jQuery('#goals').removeClass('edit-goal').scrollintoview();
    });

    goals.on('click', 'a[href="#claim"]', function (evt) {
        evt.preventDefault();
        jQuery('#goals').addClass('achievement-only');
        jQuery('#goals').scrollintoview();
        var row = jQuery(this).parents('.row'),
            gid = (/gid([0-9]+)(\s|$)/ig).exec(row.attr('class'))[1];
        jQuery('#goals-brag').addClass('gid' + gid);
    });

    goals.on('click', 'a[href="#brag-done"]', function (evt) {
        evt.preventDefault();
        var brag = jQuery('#goals-brag'),
            gid = (/gid([0-9]+)(\s|$)/ig).exec(brag.attr('class'))[1],
            inputs = brag.find('input[name^="goals-plupload"]'),
            uploads = [];

        jQuery.each(inputs, function () {
            var matches = (/\[([0-9]+)\]\[([a-z]+)\]/ig).exec(jQuery(this).attr('name'));
            if(typeof uploads[parseInt(matches[1])] == 'undefined')
                uploads[parseInt(matches[1])] = {};
            uploads[parseInt(matches[1])][matches[2]] = jQuery(this).val();
        });
        $.ajax({
                   url: '/node/save/goals',
                   type: 'POST',
                   dataType: 'json',
                   data: {
                       claim: gid,
                       uploads: uploads,
                       message: brag.find('textarea').val()
                   },
                   success: function (data) {
                       brag.removeClass('gid' + gid);
                       $('#goals').removeClass('achievement-only').scrollintoview();

                       // clear uploads
                       $.each(uploader.files, function (i, file) {
                           uploader.removeFile(file);
                       });
                       $('#goals-plupload').find('.plup-list').children().remove();
                       brag.find('textarea').val('');

                       // update achievements
                       jQuery('#achievements > *').remove();
                       jQuery(data.achievements).find('> *')
                           .appendTo(jQuery('#achievements'));
                   }
               });
    });

    goals.on('click', 'a[href="#save-incentive"]', function (evt) {
        evt.preventDefault();
        var row = jQuery(this).parents('.row');
        if(row.is('.valid'))
        {
            row.find('.field-name-field-read-only input').val(1);
            $.ajax({
                       url: '/node/save/goals',
                       type: 'POST',
                       dataType: 'json',
                       data: {
                           value: row.find('select').first().val(),
                           reward: row.find('.field-name-field-reward textarea').val(),
                           type: row.find('select').first().attr('name').substring(5)
                       },
                       success: function (data) {

                           // update tabs
                           jQuery('#goals').removeClass('edit-goal');
                           jQuery('#goals .pane-content > div').removeClass('step_1 step_2');
                           jQuery('#goals .pane-content > div').addClass('step_3');

                           // update incentives rows
                           jQuery('#non-sponsored > *, #parent-sponsored > *').remove();
                           jQuery(data.goals).find('#non-sponsored > *')
                               .appendTo(jQuery('#non-sponsored'));
                           jQuery('#goals .row').goalsFunc();

                           // update achievements
                           jQuery('#achievements > *').remove();
                           jQuery(data.achievements).find('> *')
                               .appendTo(jQuery('#achievements'));
                           jQuery('#home-goals').attr('checked', 'checked');
                           if(jQuery('#home').find('input[type="checkbox"]:checked').length == jQuery('#home').find('input[type="checkbox"]').length - 1)
                               jQuery('#home-tasks-checklist').attr('checked', 'checked');

                           // update awards such as pulse detected for setting up a class
                           if (typeof data.awards != 'undefined') {
                               var lastAward = null;
                               for (var i in data.awards) {
                                   if (data.awards[i] != false && jQuery('#badges #' + i).is('.not-awarded')) {
                                       jQuery('#badges #' + i).removeClass('not-awarded').addClass('awarded');
                                       lastAward = i;
                                   }
                               }

                               if (lastAward == 'setup-hours' || lastAward == 'setup-milestone' || lastAward == 'setup-outcome' ||
                                   lastAward == 'beginner-apples' || lastAward == 'beginner-jackpot' || lastAward == 'intermediate-disco' ||
                                   lastAward == 'intermediate-high' || lastAward == 'advanced=magneto' || lastAward == 'advanced-wall' ||
                                   lastAward == 'advanced-veni')
                                   jQuery('#badges').relocateAward(lastAward, '#goals > .pane-content');
                               else if (lastAward != null)
                                   jQuery('#badges').relocateAward(lastAward, '#badges > .pane-content');
                           }
                       }
                   });
        }
        if(jQuery(window).outerWidth(true) <= 963) {
            row.find('.field-name-field-type').scrollintoview({padding: {top:120,bottom:200,left:0,right:0}});
        }
    });

    goals.find('.row').goalsFunc();

});

