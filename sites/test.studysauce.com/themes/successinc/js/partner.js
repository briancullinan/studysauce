
jQuery(document).ready(function($) {

    var partner = jQuery('#partner');

    var partnerFunc = function () {
        var valid = true;
        if(partner.find('input[name="partner-first"]').val().trim() == '' ||
           partner.find('input[name="partner-last"]').val().trim() == '' ||
           partner.find('input[name="partner-email"]').val().trim() == '' ||
            !(/\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,6}\b/i).test(partner.find('input[name="partner-email"]').val()))
            valid = false;
        if(valid)
            partner.removeClass('invalid').addClass('valid');
        else
            partner.removeClass('valid').addClass('invalid');
    };

    partner.on('change', 'input[name="partner-first"],input[name="partner-last"],input[name="partner-email"]', partnerFunc);
    partner.on('keyup', 'input[name="partner-first"],input[name="partner-last"],input[name="partner-email"]', partnerFunc);
    partnerFunc();

    var partnerUpload = $('#partner-plupload');
    var uploader = new plupload.Uploader({
        alt_field: 0,
        browse_button: "partner-plupload-select",
        chunk_size: "512K",
        container: "partner-plupload",
        dragdrop: true,
        drop_element: "partner-plupload-filelist",
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
        name: "partner-plupload-upload",
        runtimes: "html5,gears,flash,silverlight,browserplus,html4",
        silverlight_xap_url: "/sites/all/libraries/plupload/js/plupload.silverlight.xap",
        title_field: 0,
        unique_names: true,
        upload: "partner-plupload-upload",
        url: jQuery('#partner-upload-path').val(),
        urlstream_upload: false
    });
    $('#partner-plupload-select').click(function(e) {
        uploader.start();
        e.preventDefault();
    });
    uploader.init();

    uploader.bind('FilesAdded', function(up, files) {
        partnerUpload.find('.plup-drag-info').hide(); // Hide info
        partnerUpload.find('.plup-upload').show(); // Show upload button

        // Put files visually into queue
        $.each(files, function(i, file) {
            partnerUpload.find('.plup-filelist table').append('<tr id="' + file.id + '">' +
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
        partnerUpload.find('.plup-progress').progressbar({value: uploader.total.percent});
    });

    // Event after a file has been uploaded from queue
    uploader.bind('FileUploaded', function(up, file, response) {
        // Respone is object with response parameter so 2x repsone
        var fileSaved = jQuery.parseJSON(response.response);
        var delta = partnerUpload.find('.plup-list li').length;
        var name = 'partner-plupload[' + delta + ']';

        // Plupload has weird error handling behavior so we have to check for errors here
        if (fileSaved.error_message) {
            $('#' + file.id + ' > .plup-filelist-message').append('<b>Error: ' + fileSaved.error_message + '</b>');
            up.refresh(); // Refresh for flash or silverlight
            return;
        }

        partnerUpload.find('.plup-filelist #' + file.id).remove(); // Remove uploaded file from queue
        // Add image thumbnail into list of uploaded items
        partnerUpload.find('.plup-list').append(
            '<li class="ui-state-default">' +
                //'<div class="plup-thumb-wrapper"><img src="'+ Drupal.settings.plup[thisID].image_style_path + file.target_name + '" /></div>' +
            '<div class="plup-thumb-wrapper"><img src="'+ fileSaved.secure_uri + '" title="'+ Drupal.checkPlain(file.target_name) +'" /></div>' +
            '<a class="plup-remove-item"></a>' +
            '<input type="hidden" name="' + name + '[fid]" value="' + fileSaved.fid + '" />' +
            '<input type="hidden" name="' + name + '[weight]" value="' + delta + '" />' +
            '<input type="hidden" name="' + name + '[rename]" value="' + file.name +'" />' +
            '</li>');
        // Bind remove functionality to uploaded file
        var new_element = $('input[name="'+ name +'[fid]"]');
        partnerUpload.find('img[src*="empty-photo.png"]').remove();
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
        partnerUpload.find('.plup-list').sortable('refresh'); // Refresh sortable
        partnerUpload.find('.plup-drag-info').show(); // Show info
    });

    partner.on('click', 'a[href="#partner-save"]', function (evt) {
        evt.preventDefault();
        if(partner.is('.invalid'))
            return;

        var inputs = partner.find('input[name^="partner-plupload"]'),
            uploads = [],
            permissions = [];

        jQuery.each(inputs, function () {
            var matches = (/\[([0-9]+)\]\[([a-z]+)\]/ig).exec(jQuery(this).attr('name'));
            if(typeof uploads[parseInt(matches[1])] == 'undefined')
                uploads[parseInt(matches[1])] = {};
            uploads[parseInt(matches[1])][matches[2]] = jQuery(this).val();
        });

        partner.find('.partner-permissions input:checked').each(function () {
            permissions[permissions.length] = jQuery(this).val();
        });

        jQuery.ajax({
                   url: 'partner/save',
                   type: 'POST',
                   dataType: 'json',
                   data: {
                       first: partner.find('input[name="partner-first"]').val(),
                       last: partner.find('input[name="partner-last"]').val(),
                       email: partner.find('input[name="partner-email"]').val(),
                       permissions: permissions.join(','),
                       uploads: uploads
                   },
                   success: function () {
                       partner.removeClass('valid').addClass('invalid');
                   }
               });

    });

});

