
jQuery(document).ready(function($) {

    var inviteFunc = function () {
        var valid = true;
        if(jQuery('#invite').find('input[name="invite-first"]').val().trim() == '' ||
           jQuery('#invite').find('input[name="invite-last"]').val().trim() == '' ||
           jQuery('#invite').find('input[name="invite-email"]').val().trim() == '')
            valid = false;
        if(valid)
            jQuery('#invite').removeClass('invalid').addClass('valid');
        else
            jQuery('#invite').removeClass('valid').addClass('invalid');
    };

    jQuery('#invite').on('change', 'input[name="invite-first"],input[name="invite-last"],input[name="invite-email"]', inviteFunc);
    jQuery('#invite').on('keyup', 'input[name="invite-first"],input[name="invite-last"],input[name="invite-email"]', inviteFunc);
    inviteFunc();

    jQuery('#invite').on('click', 'a[href="#invite-send"]', function (evt) {
        evt.preventDefault();
        if(jQuery('#invite').is('.invalid'))
            return;

        $.ajax({
                   url: 'invite/send',
                   type: 'POST',
                   dataType: 'json',
                   data: {
                       first: jQuery('#invite').find('input[name="invite-first"]').val(),
                       last: jQuery('#invite').find('input[name="invite-last"]').val(),
                       email: jQuery('#invite').find('input[name="invite-email"]').val()
                   },
                   success: function (data) {

                   }
               });
    });

});