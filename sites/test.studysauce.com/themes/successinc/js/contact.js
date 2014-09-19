
jQuery(document).ready(function() {

    if(typeof window.contactSetUp != 'undefined')
        return;

    window.contactSetUp = true;

    jQuery('body').on('click', 'a[href="#submit-contact"]', function (evt) {
        var contact = jQuery(this).closest('.dialog, #contact, .page-path-contact, .landing-home, body').first();

        var that = jQuery(this);
        evt.preventDefault();
        if(contact.is('.invalid'))
            return;
        contact.removeClass('valid').addClass('invalid');

        jQuery.ajax({
            url: 'contact/save',
            type: 'POST',
            dataType: 'json',
            data: {
                name: contact.find('input[name="submitted[your_name]"]').val(),
                email: contact.find('input[name="submitted[your_email]"]').val(),
                message: contact.find('textarea[name="submitted[message]"]').val()
            },
            success: function () {
                window.location = '/#home';
                that.parents('.dialog').dialog('hide');
                contact.find('input[name="submitted[your_name]"], input[name="submitted[your_email]"], textarea[name="submitted[message]"]').val('');
            }
        });

    });

});


