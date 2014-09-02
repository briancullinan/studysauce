
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
    partner.on('change', '.partner-permissions input', function () {
        partnerFunc();
        partner.find('a[href="#partner-save"]').first().trigger('click');
    });
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
        partnerUpload.find('.plup-list li').remove();
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
        partnerUpload.find('img[src*="empty-photo.png"]').remove();
        var remove_element = $(new_element).siblings('.plup-remove-item');
        plup_remove_item(remove_element);
        // Bind resize effect to inputs of uploaded file
        var text_element = $(new_element).siblings('input.form-text');
        plup_resize_input(text_element);
        // Tell Drupal that form has been updated
        new_element.trigger('formUpdated');

        // trigger save
        partnerFunc();
        partner.find('a[href="#partner-save"]').first().trigger('click');
    });

    // All fiels from queue has been uploaded
    uploader.bind('UploadComplete', function(up, files) {
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
                   success: function (fileSaved) {
                       partner.removeClass('valid').addClass('invalid');
                       jQuery('#home-partner').attr('checked', 'checked');
                       if(jQuery('#home').find('input[type="checkbox"]:checked').length == jQuery('#home').find('input[type="checkbox"]').length - 1)
                           jQuery('#home-tasks-checklist').attr('checked', 'checked');

                       // update masthead
                       jQuery('#partner-message a[href="#partner"], #partner-message span').replaceWith('<span>' + partner.find('input[name="partner-first"]').val() + ' ' + partner.find('input[name="partner-last"]').val() + '</span>');
                       // update masthead picture
                       jQuery('#partner-message img').replaceWith('<img src="'+ (typeof fileSaved.uri != 'undefined' ? fileSaved.uri : (pathToTheme + '/images/empty-photo.png')) + '" height="48" width="48" alt="Partner" />')
                   }
               });

    });

    partner.on('click', 'a[href^="#uid-"]', function (evt) {
        evt.preventDefault();
        var uid = jQuery(this).attr('href').substring(5);
        jQuery('.user-pane').remove();
        var pane = jQuery('#uid-' + uid + '.panel-pane');
        if(pane.length == 0)
        {
            pane = jQuery('<div id="uid-' + uid + '" class="panel-pane user-pane"><div class="pane-content" /></div>')
                .appendTo(jQuery('.page .grid_12 > div'));
        }

        jQuery('body').removeClass(footerOnly).removeClass(menuOnly).removeClass('menu-open').addClass('uid-only');
        window.location.hash = '#uid-' + uid;

        pane.find('.pane-content').addClass('loading');
        jQuery.ajax({
            url: '/partner?uid=' + uid,
            dataType: 'json',
            type: 'GET',
            success: function (response) {
                pane.find('.pane-content').html(response.content);

                jQuery(response.styles).each(function () {
                    var url = jQuery(this).attr('href');
                    if(typeof url != 'undefined' && jQuery('link[href="' + url + '"]').length == 0)
                        $('head').append('<link href="' + url + '" type="text/css" rel="stylesheet" />');
                    else
                    {
                        var re = (/url\("(.*?)"\)/ig),
                            match,
                            media = jQuery(this).attr('media');
                        while (match = re.exec(jQuery(this).html())) {
                            if(jQuery('link[href="' + match[1] + '"]').length == 0 &&
                                jQuery('style:contains("' + match[1] + '")').length == 0)
                            {
                                if(typeof media == 'undefined' || media == 'all')
                                    $('head').append('<link href="' + match[1] + '" type="text/css" rel="stylesheet" />');
                                else
                                    $('head').append('<style media="' + media + '">@import url("' + match[1] + '");');
                            }
                        }
                    }
                });

                jQuery(response.scripts).each(function () {
                    var url = jQuery(this).attr('src');
                    if(typeof url != 'undefined' && jQuery('script[src="' + url + '"]').length == 0)
                        jQuery.getScript(url);
                });

                pane.find('.pane-content').removeClass('loading');
            }
        });
    });
});

