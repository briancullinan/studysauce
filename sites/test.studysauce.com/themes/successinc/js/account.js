
jQuery(document).ready(function() {

    var account = jQuery('#account');

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
        jQuery.ajax({
            url:'/user/save',
            type: 'POST',
            dataType: 'json',
            data: {
                first: account.find('.field-name-field-first-name input').val(),
                last: account.find('.field-name-field-last-name input').val(),
                email: account.find('.form-item-mail input').val(),
                pass: account.find('.form-item-current-pass input').val(),
                newPass: account.find('.form-item-pass input').val()
            },
            success: function (data) {
            }
        })
    });

});