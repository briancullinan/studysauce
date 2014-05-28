
jQuery(document).ready(function($) {

    var profile = $('#profile, .page-path-profile, .page-path-customization, .page-path-customization2').first();

    $.fn.profileFunc = function () {
        var valid = true,
            classProfile = jQuery('.class-profile'),
            studyPreferences = jQuery('.study-preferences');
        if(classProfile.is(':visible'))
        {
            classProfile.find('.row').each(function () {
                if(jQuery(this).find('.field-name-type-of-studying input:checked').length == 0 ||
                    jQuery(this).find('.field-name-difficulty-level input:checked').length == 0)
                    valid = false;
            });
        }

        // check profile questions for completeness
        if(studyPreferences.nextUntil(classProfile).filter(':visible').length > 0)
        {
            studyPreferences.nextUntil(classProfile).filter(':visible').each(function () {
                if(jQuery(this).find('input:checked').length == 0)
                    valid = false;
            });
        }

        if(studyPreferences.is(':visible') && (
            (profile.find('.field-name-field-university input').length > 0 &&
            profile.find('.field-name-field-university input').val().trim() == '') ||
           profile.find('.field-name-field-grades input:checked').length == 0 ||
           profile.find('.field-name-field-weekends input:checked').length == 0 ||
           profile.find('.field-name-field-6-am-11-am input:checked').length == 0 ||
           profile.find('.field-name-field-11-am-4-pm input:checked').length == 0 ||
           profile.find('.field-name-field-4-pm-9-pm input:checked').length == 0 ||
           profile.find('.field-name-field-9-pm-2-am input:checked').length == 0))
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
        if(profile.is('.invalid'))
            return;

        var classProfile = jQuery('.class-profile'),
            studyPreferences = jQuery('.study-preferences');
        if(studyPreferences.nextUntil(classProfile).filter(':visible').length > 0)
        {
            var profileData = { },
                questions = studyPreferences.nextUntil(classProfile).filter(':visible');
            questions.each(function () {
                var that = jQuery(this),
                    k = that.find('input:checked').attr('name').replace('profile-question-', '');
                profileData[k] = that.find('input:checked').val();
            });

            $.ajax({
                       url: '/node/save/profile',
                       type: 'POST',
                       data: profileData,
                       success: function (data) {
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
                           else
                           {
                               // update calendar events
                               window.planEvents = data.events;
                               if(calendar != null && typeof calendar.fullCalendar != 'undefined')
                                   calendar.fullCalendar('refetchEvents');


                               // update plan tab
                               var plan = jQuery('#plan');
                               plan.find('.row, .head').remove();
                               jQuery(data.plan).find('.row, .head')
                                   .insertBefore(plan.find('.pane-content p').last());

                               // update profile tab
                               profile.removeClass('valid').addClass('invalid');
                           }
                       }
                   });
        }


        // TODO: move some of this to profile node instead?
        if(!studyPreferences.is(':visible') && !classProfile.is(':visible'))
            return;

        var scheduleData = { };

        if(classProfile.is(':visible'))
        {
            jQuery('.class-profile .row').each(function () {
                var row = jQuery(this),
                    eid = row.attr('id').substring(4);
                scheduleData[eid] = {
                    type: row.find('.field-name-type-of-studying input:checked').val(),
                    difficulty: row.find('.field-name-difficulty-level input:checked').val()};
            });
        }

        if(studyPreferences.is(':visible'))
        {
            //university: profile.find('.field-name-field-university input').val(),
            scheduleData['grades'] = profile.find('.field-name-field-grades input:checked').val();
            scheduleData['weekends'] = profile.find('.field-name-field-weekends input:checked').val();
            scheduleData['6-am-11-am'] = profile.find('.field-name-field-6-am-11-am input:checked').val();
            scheduleData['11-am-4-pm'] = profile.find('.field-name-field-11-am-4-pm input:checked').val();
            scheduleData['4-pm-9-pm'] = profile.find('.field-name-field-4-pm-9-pm input:checked').val();
            scheduleData['9-pm-2-am'] = profile.find('.field-name-field-9-pm-2-am input:checked').val();
        }

        $.ajax({
                   url: '/node/save/schedule',
                   type: 'POST',
                   data: scheduleData,
                   success: function (data) {

                       if(window.location.pathname == '/customization')
                           window.location = '/customization2';
                       else if(window.location.pathname == '/customization2')
                           window.location = '/#plan';
                       else
                       {
                           // update calendar events
                           window.planEvents = data.events;
                           if(calendar != null && typeof calendar.fullCalendar != 'undefined')
                               calendar.fullCalendar('refetchEvents');


                           // update plan tab
                           var plan = jQuery('#plan');
                           plan.find('.row, .head').remove();
                           jQuery(data.plan).find('.row, .head')
                               .insertBefore(plan.find('.pane-content p').last());

                           // update profile tab
                           profile.removeClass('valid').addClass('invalid');
                       }
                   }
               });

    });

});