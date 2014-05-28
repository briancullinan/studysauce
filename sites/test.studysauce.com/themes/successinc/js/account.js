
jQuery(document).ready(function() {

    var account = jQuery('#account');

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
    })
    //account.find('.field-name-account-type input').first().trigger('change');


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