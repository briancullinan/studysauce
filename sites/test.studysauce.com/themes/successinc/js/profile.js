
jQuery(document).ready(function($) {

    var profile = $('#profile');

    $.fn.profileFunc = function () {
        var valid = true;
        if(profile.find('.field-name-field-university input').val().trim() == '' ||
           profile.find('.field-name-field-grades input:checked').length == 0 ||
           profile.find('.field-name-field-weekends input:checked').length == 0 ||
           profile.find('.field-name-field-6-am-11-am input:checked').length == 0 ||
           profile.find('.field-name-field-11-am-4-pm input:checked').length == 0 ||
           profile.find('.field-name-field-4-pm-9-pm input:checked').length == 0 ||
           profile.find('.field-name-field-9-pm-2-am input:checked').length == 0)
            valid = false;
        if(valid)
            profile.removeClass('invalid').addClass('valid');
        else
            profile.removeClass('valid').addClass('invalid');
    };
    profile.on('keyup', '.field-name-field-university input', $.fn.profileFunc);
    profile.on('change', '.field-name-field-university input', $.fn.profileFunc);
    profile.on('change', '.field-name-field-grades input', $.fn.profileFunc);
    profile.on('change', '.field-name-field-weekends input', $.fn.profileFunc);
    profile.on('change', '.field-name-field-6-am-11-am input', $.fn.profileFunc);
    profile.on('change', '.field-name-field-11-am-4-pm input', $.fn.profileFunc);
    profile.on('change', '.field-name-field-4-pm-9-pm input', $.fn.profileFunc);
    profile.on('change', '.field-name-field-9-pm-2-am input', $.fn.profileFunc);
    $.fn.profileFunc();

    profile.on('click', 'a[href="#save-profile"]', function (evt) {
        evt.preventDefault();
        if(!profile.is('.invalid'))
        {
            $.ajax({
                       url: '/node/save/schedule',
                       type: 'POST',
                       data: {
                           university: profile.find('.field-name-field-university input').val(),
                           grades: profile.find('.field-name-field-grades input:checked').val(),
                           weekends: profile.find('.field-name-field-weekends input:checked').val(),
                           '6-am-11-am': profile.find('.field-name-field-6-am-11-am input:checked').val(),
                           '11-am-4-pm': profile.find('.field-name-field-11-am-4-pm input:checked').val(),
                           '4-pm-9-pm': profile.find('.field-name-field-4-pm-9-pm input:checked').val(),
                           '9-pm-2-am': profile.find('.field-name-field-9-pm-2-am input:checked').val()
                       },
                       success: function (data) {

                       }
                   });
        }
    });

});