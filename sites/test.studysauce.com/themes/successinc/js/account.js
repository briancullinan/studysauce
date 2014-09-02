
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

    var account = jQuery('#account');

    var accountFunc = function () {
        var valid = true,
            email = account.find('.form-item-mail input').prop('defaultValue');

        if(account.find('.form-item-mail input').val().trim() == '' ||
            account.find('.field-name-field-first-name input').val() == '' ||
            account.find('.field-name-field-last-name input').val() == '' ||
            (account.find('.form-item-mail input').val().trim() != email &&
                account.find('.form-item-current-pass input').val().trim() == '') ||
            (account.find('.form-item-pass input').val().trim() != '' &&
                account.find('.form-item-current-pass input').val().trim() == ''))
            valid = false;

        if(!valid)
            account.removeClass('valid').addClass('invalid');
        else
            account.removeClass('invalid').addClass('valid');
    };

    // set default value for email
    if((/\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,6}\b/i).test(account.find('.form-item-mail input').val()))
        account.addClass('invalid').find('.form-item-mail input').prop('defaultValue', account.find('.form-item-mail input').val().trim());

    account.on('change', '.field-name-field-first-name input, .field-name-field-last-name input, .form-item-mail input, ' +
                         '.form-item-current-pass input, .form-item-pass input', function () {
        accountFunc();
    });
    account.on('keyup', '.field-name-field-first-name input, .field-name-field-last-name input, .form-item-mail input, ' +
        '.form-item-current-pass input, .form-item-pass input', function () {
        accountFunc();
    });
    account.on('keydown', '.field-name-field-first-name input, .field-name-field-last-name input, .form-item-mail input, ' +
        '.form-item-current-pass input, .form-item-pass input', function () {
        accountFunc();
    });

    account.find('.field-name-account-type input').each(function () {
        jQuery(this).data('origState', jQuery(this).prop('checked'));
    });
    account.find('.field-name-account-type input').change(function (evt) {
        evt.preventDefault();
        if(jQuery(this).prop('checked') != jQuery(this).data('origState'))
            jQuery(this).prop('checked', jQuery(this).data('origState'));


        if(account.find('input[value="monthly"]:checked').length > 0)
            account.find('.field-name-account-type a').attr('href', '/cart/add/e-p13_q1_a4o14_s?destination=cart/checkout');
        else if(account.find('input[value="yearly"]:checked').length > 0)
            account.find('.field-name-account-type a').attr('href', '/cart/add/e-p13_q1_a4o14_s?destination=cart/checkout');
        else
            account.find('.field-name-account-type a').attr('href', '/buy');
    });

    account.find('.field-name-account-type label').click(function (evt) {
        evt.preventDefault();
    });
    //account.find('.field-name-account-type input').first().trigger('change');

    var accountUpload = jQuery('#account-plupload');
    if(accountUpload.length > 0)
    {
        var uploader = new plupload.Uploader({
            alt_field: 0,
            browse_button: "account-plupload-select",
            chunk_size: "512K",
            container: "account-plupload",
            dragdrop: true,
            drop_element: "account-plupload-filelist",
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
            name: "account-plupload-upload",
            runtimes: "html5,gears,flash,silverlight,browserplus,html4",
            silverlight_xap_url: "/sites/all/libraries/plupload/js/plupload.silverlight.xap",
            title_field: 0,
            unique_names: true,
            upload: "account-plupload-upload",
            url: jQuery('#account-upload-path').val(),
            urlstream_upload: false
        });
        uploader.init();
        jQuery('#account-plupload-select').click(function(e) {
            uploader.start();
            e.preventDefault();
        });

        uploader.bind('FilesAdded', function(up, files) {
            accountUpload.find('.plup-drag-info').hide(); // Hide info
            accountUpload.find('.plup-upload').show(); // Show upload button

            // Put files visually into queue
            jQuery.each(files, function(i, file) {
                accountUpload.find('.plup-filelist table').append('<tr id="' + file.id + '">' +
                    '<td class="plup-filelist-file">' +  file.name + '</td>' +
                    '<td class="plup-filelist-size">' + plupload.formatSize(file.size) + '</td>' +
                    '<td class="plup-filelist-message"></td>' +
                    '<td class="plup-filelist-remove"><a class="plup-remove-file"></a></td>' +
                    '</tr>');
                // Bind remove functionality to files added int oqueue
                jQuery('#' + file.id + ' .plup-filelist-remove > a').bind('click', function(event) {
                    jQuery('#' + file.id).remove();
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
            accountUpload.find('.plup-progress').progressbar({value: uploader.total.percent});
        });

        // Event after a file has been uploaded from queue
        uploader.bind('FileUploaded', function(up, file, response) {
            // Respone is object with response parameter so 2x repsone
            accountUpload.find('.plup-list li').remove();
            var fileSaved = jQuery.parseJSON(response.response);
            var delta = accountUpload.find('.plup-list li').length;
            var name = 'account-plupload[' + delta + ']';

            // Plupload has weird error handling behavior so we have to check for errors here
            if (fileSaved.error_message) {
                jQuery('#' + file.id + ' > .plup-filelist-message').append('<b>Error: ' + fileSaved.error_message + '</b>');
                up.refresh(); // Refresh for flash or silverlight
                return;
            }

            accountUpload.find('.plup-filelist #' + file.id).remove(); // Remove uploaded file from queue
            // Add image thumbnail into list of uploaded items
            accountUpload.find('.plup-list').append(
                '<li>' +
                    //'<div class="plup-thumb-wrapper"><img src="'+ Drupal.settings.plup[thisID].image_style_path + file.target_name + '" /></div>' +
                    '<div class="plup-thumb-wrapper"><img src="'+ fileSaved.secure_uri + '" title="'+ Drupal.checkPlain(file.target_name) +'" /></div>' +
                    '<a class="plup-remove-item"></a>' +
                    '<input type="hidden" name="' + name + '[fid]" value="' + fileSaved.fid + '" />' +
                    '<input type="hidden" name="' + name + '[weight]" value="' + delta + '" />' +
                    '<input type="hidden" name="' + name + '[rename]" value="' + file.name +'" />' +
                    '</li>');
            // Bind remove functionality to uploaded file
            var new_element = jQuery('input[name="'+ name +'[fid]"]');
            accountUpload.find('img[src*="empty-photo.png"]').remove();
            var remove_element = jQuery(new_element).siblings('.plup-remove-item');
            plup_remove_item(remove_element);
            // Bind resize effect to inputs of uploaded file
            var text_element = jQuery(new_element).siblings('input.form-text');
            plup_resize_input(text_element);
            // Tell Drupal that form has been updated
            new_element.trigger('formUpdated');

            // trigger save
            account.removeClass('invalid').addClass('valid');
            var inputs = account.find('input[name^="account-plupload"]'),
                uploads = [];

            jQuery.each(inputs, function () {
                var matches = (/\[([0-9]+)\]\[([a-z]+)\]/ig).exec(jQuery(this).attr('name'));
                if(typeof uploads[parseInt(matches[1])] == 'undefined')
                    uploads[parseInt(matches[1])] = {};
                uploads[parseInt(matches[1])][matches[2]] = jQuery(this).val();
            });
            jQuery.ajax({
                url:'/user/save',
                type: 'POST',
                dataType: 'json',
                data: {
                    picture: uploads
                },
                success: function (data) {
                    account.removeClass('valid').addClass('invalid');
                }
            })

        });

        // All fiels from queue has been uploaded
        uploader.bind('UploadComplete', function(up, files) {
            accountUpload.find('.plup-drag-info').show(); // Show info
        });
    }

    account.on('click', 'a[href="#cancel-account"]', function (evt) {
        evt.preventDefault();
        jQuery.ajax({
                        url:'/user/save',
                        type: 'POST',
                        dataType: 'json',
                        data: {
                            cancel: true
                        },
                        success: function () {
                            window.location = '/';
                        }
                    })
    });

    account.on('click', 'a[href="#save-account"]', function (evt) {
        evt.preventDefault();

        if(account.is('.invalid'))
            return;

        var inputs = account.find('input[name^="account-plupload"]'),
            uploads = [];

        jQuery.each(inputs, function () {
            var matches = (/\[([0-9]+)\]\[([a-z]+)\]/ig).exec(jQuery(this).attr('name'));
            if(typeof uploads[parseInt(matches[1])] == 'undefined')
                uploads[parseInt(matches[1])] = {};
            uploads[parseInt(matches[1])][matches[2]] = jQuery(this).val();
        });

        account.find('.form-item-current-pass').removeClass('passwordError');
        jQuery.ajax({
            url:'/user/save',
            type: 'POST',
            dataType: 'json',
            data: {
                first: account.find('.field-name-field-first-name input').val(),
                last: account.find('.field-name-field-last-name input').val(),
                email: account.find('.form-item-mail input').val(),
                pass: account.find('.form-item-current-pass input').val(),
                newPass: account.find('.form-item-pass input').val(),
                picture: uploads
            },
            success: function (data) {
                if(data.password_error)
                {
                    account.find('.form-item-current-pass').addClass('passwordError');
                }
                account.find('.form-item-current-pass input').val('');
                account.find('.form-item-pass input').val('');
                account.removeClass('valid').addClass('invalid');
            }
        })
    });


});