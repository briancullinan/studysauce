
jQuery(document).ready(function($) {

    var profile = $('#profile, .page-path-profile, .page-path-customization, .page-path-customization2').first();

    $.fn.profileFunc = function () {
        var valid = true;
        if(jQuery('.class-profile').is(':visible'))
        {
            jQuery('.class-profile').find('.row').each(function () {
                if(jQuery(this).find('.field-name-type-of-studying input:checked').length == 0 ||
                    jQuery(this).find('.field-name-difficulty-level input:checked').length == 0)
                    valid = false;
            });
        }
        // check profile questions for completeness
        else if(jQuery('.study-preferences').nextUntil('.class-profile').filter(':visible').length > 0)
        {
            jQuery('.study-preferences').nextUntil('.class-profile').filter(':visible').each(function () {
                if(jQuery(this).find('input:checked').length == 0)
                    valid = false;
            });
        }
        else if((profile.find('.field-name-field-university input').length > 0 && profile.find('.field-name-field-university input').val().trim() == '') ||
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
    jQuery('.study-preferences').nextUntil('.class-profile').find('input').on('change', $.fn.profileFunc);
    profile.on('change', '.field-name-type-of-studying input', $.fn.profileFunc);
    profile.on('change', '.field-name-difficulty-level input', $.fn.profileFunc);
    $.fn.profileFunc();

    profile.on('click', 'a[href="#save-profile"]', function (evt) {
        evt.preventDefault();
        if(!profile.is('.invalid'))
        {
            var data;
            if(window.location.pathname == '/profile')
            {
                var questions = jQuery('.study-preferences').nextUntil('.class-profile').filter(':visible');
                data = { };
                questions.each(function () {
                    var that = jQuery(this),
                        k = that.find('input:checked').attr('name').replace('profile-question-', '');
                    data[k] = that.find('input:checked').val();
                });
            }
            else if (window.location.pathname == '/customization2')
            {
                data = {};
                jQuery('.class-profile .row').each(function () {
                    var row = jQuery(this),
                        eid = row.attr('id').substring(4);
                    data[eid] = {
                        type: row.find('.field-name-type-of-studying input:checked').val(),
                        difficulty: row.find('.field-name-difficulty-level input:checked').val()};
                });
            }
            else
                data = {
                    //university: profile.find('.field-name-field-university input').val(),
                    grades: profile.find('.field-name-field-grades input:checked').val(),
                    weekends: profile.find('.field-name-field-weekends input:checked').val(),
                    '6-am-11-am': profile.find('.field-name-field-6-am-11-am input:checked').val(),
                    '11-am-4-pm': profile.find('.field-name-field-11-am-4-pm input:checked').val(),
                    '4-pm-9-pm': profile.find('.field-name-field-4-pm-9-pm input:checked').val(),
                    '9-pm-2-am': profile.find('.field-name-field-9-pm-2-am input:checked').val()
                };
            $.ajax({
                       url: window.location.pathname == '/profile'
                            ? '/node/save/profile'
                            : '/node/save/schedule',
                       type: 'POST',
                       data: data,
                       success: function () {
                           if(window.location.pathname == '/profile')
                           {
                               // TODO: we have reached the last question?
                               if(questions.is(jQuery('.class-profile').prev()))
                                   window.location.pathname = '/schedule';
                               else
                               {
                                   questions.hide();
                                   questions.next().show();
                               }
                           }
                           else if(window.location.pathname == '/customization')
                               window.location = '/customization2';
                           else if(window.location.pathname == '/customization2')
                               window.location = '/';
                       }
                   });
        }
    });

});