
jQuery(document).ready(function() {

    var contact = jQuery('#contact, .page-path-contact').first();

    if(typeof window.contactSetUp != 'undefined')
        return;

    window.contactSetUp = true;

    contact.on('click', 'a[href="#submit-contact"]', function (evt) {
        evt.preventDefault();

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
                contact.find('input[name="submitted[your_name]"], input[name="submitted[your_email]"], textarea[name="submitted[message]"]').val('');
            }
        });

    });

});


