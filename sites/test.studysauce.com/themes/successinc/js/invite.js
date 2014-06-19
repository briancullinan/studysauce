
jQuery(document).ready(function($) {

    var invites = jQuery('#invite');

    var inviteFunc = function () {
        var valid = true;
        if(invites.find('input[name="invite-first"]').val().trim() == '' ||
            invites.find('input[name="invite-last"]').val().trim() == '' ||
            invites.find('input[name="invite-email"]').val().trim() == '' ||
            !(/\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,6}\b/i).test(invites.find('input[name="invite-email"]').val()))
            valid = false;
        if(valid)
            invites.removeClass('invalid').addClass('valid');
        else
            invites.removeClass('valid').addClass('invalid');
    };

    invites.on('change', 'input[name="invite-first"],input[name="invite-last"],input[name="invite-email"]', inviteFunc);
    invites.on('keyup', 'input[name="invite-first"],input[name="invite-last"],input[name="invite-email"]', inviteFunc);
    inviteFunc();

    invites.on('click', 'a[href="#invite-send"]', function (evt) {
        evt.preventDefault();
        if(invites.is('.invalid'))
            return;

        $.ajax({
                   url: 'invite/send',
                   type: 'POST',
                   dataType: 'json',
                   data: {
                       first: invites.find('input[name="invite-first"]').val(),
                       last: invites.find('input[name="invite-last"]').val(),
                       email: invites.find('input[name="invite-email"]').val()
                   },
                   success: function (data) {
                       invites.find('input[name="invite-first"]').val('');
                       invites.find('input[name="invite-last"]').val('');
                       invites.find('input[name="invite-email"]').val('');
                       invites.removeClass('valid').addClass('invalid');
                   }
               });
    });

});