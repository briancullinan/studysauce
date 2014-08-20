
jQuery(document).ready(function($) {

    var profile = $('#profile, .page-path-profile, .page-path-customization, .page-path-customization2').first();

    $.fn.profileFunc = function () {
        var valid = true,
            classProfile = profile.find('.class-profile'),
            studyPreferences = profile.find('.study-preferences');
        if(classProfile.is(':visible'))
        {
            classProfile.find('.field-name-type-of-studying .row').each(function () {
                var row = jQuery(this),
                    cid = (/cid-([0-9]+)(\s|$)/ig).exec(row.attr('class'))[1];
                if(classProfile.find('.field-name-type-of-studying .row.cid-' + cid + ' input:checked').length == 0 ||
                    classProfile.find('.field-name-difficulty-level .row.cid-' + cid + ' input:checked').length == 0)
                    valid = false;
            });
        }

        // check profile questions for completeness
        if(profile.find('div[class*="profile-question"]').filter(':visible').length > 0)
        {
            profile.find('div[class*="profile-question"]').filter(':visible').each(function () {
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
    profile.on('change', 'div[class*="profile-question"] input', $.fn.profileFunc);
    profile.on('change', '.field-name-type-of-studying input', $.fn.profileFunc);
    profile.on('change', '.field-name-difficulty-level input', $.fn.profileFunc);
    $.fn.profileFunc();

    if(window.location.pathname == '/profile')
    {
        // make the next unanswered question visible
        var questions = profile.find('div[class*="profile-question"]');
        questions.each(function () {
            if(jQuery(this).find('input:checked').length == 0)
            {
                questions.hide();
                jQuery(this).show();
                return false;
            }
        });
    }

    profile.on('click', 'a[href="#save-profile"]', function (evt) {
        evt.preventDefault();
        if(profile.is('.invalid'))
            return;

        var classProfile = profile.find('.class-profile'),
            studyPreferences = profile.find('.study-preferences');
        if(profile.find('div[class*="profile-question"]').filter(':visible').length > 0)
        {
            var profileData = { },
                questions = profile.find('div[class*="profile-question"]').filter(':visible');
            questions.each(function () {
                var that = jQuery(this),
                    k = that.find('input:checked').attr('name').replace('profile-question-', '');
                profileData[k] = that.find('input:checked').val();
            });


            if(!studyPreferences.is(':visible') && !classProfile.is(':visible'))
            {
                profile.addClass('building').scrollintoview();
                profile.find('.timer').pietimer('reset');
                profile.find('.timer').pietimer({
                    timerSeconds: 30,
                    color: '#09B',
                    fill: false,
                    showPercentage: true,
                    callback: function() {
                    }
                });
                profile.find('.timer').pietimer('start');
            }

            // skip building the schedule if we are in the middle of the buy funnel
            // skip building if we still have to save schedule because it will do it then too
            if(window.location.pathname == '/profile' || studyPreferences.is(':visible') || classProfile.is(':visible'))
                profileData['skipBuild'] = true;

            $.ajax({
                       url: '/node/save/profile',
                       type: 'POST',
                       dataType: 'json',
                       data: profileData,
                       success: function (data) {
                           if(window.location.pathname == '/profile')
                           {
                               // we have reached the last question?
                               if(questions.is(profile.find('div[class*="profile-question"]').last()))
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
                               jQuery('#calendar').updatePlan(data.events);

                               // update plan tab
                               var plan = jQuery('#plan');
                               plan.find('.row, .head').remove();
                               jQuery(data.plan).find('.row, .head')
                                   .insertBefore(plan.find('.pane-content p').last());

                               // update profile tab
                               profile.removeClass('valid').addClass('invalid');
                               if(!studyPreferences.is(':visible') && !classProfile.is(':visible'))
                                   profile.removeClass('building');
                           }
                       },
                       error: function () {
                           profile.removeClass('building');
                       }
                   });
        }


        // TODO: move some of this to profile node instead?
        if(!studyPreferences.is(':visible') && !classProfile.is(':visible'))
            return;

        var scheduleData = { };

        if(classProfile.is(':visible'))
        {
            classProfile.find('.field-name-type-of-studying .row').each(function () {
                var row = jQuery(this),
                    cid = (/cid-([0-9]+)(\s|$)/ig).exec(row.attr('class'))[1];
                scheduleData[cid] = {
                    type: classProfile.find('.field-name-type-of-studying .row.cid-' + cid + ' input:checked').val(),
                    difficulty: classProfile.find('.field-name-difficulty-level .row.cid-' + cid + ' input:checked').val()};
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

        profile.addClass('building').scrollintoview();
        profile.find('.timer').pietimer('reset');
        profile.find('.timer').pietimer({
            timerSeconds: 60,
            color: '#09B',
            fill: false,
            showPercentage: true,
            callback: function() {
            }
        });
        profile.find('.timer').pietimer('start');

        // skip building the schedule if we are in the middle of the buy funnel
        if(window.location.pathname == '/customization')
            scheduleData['skipBuild'] = true;

        $.ajax({
                   url: '/node/save/schedule',
                   type: 'POST',
                   dataType: 'json',
                   data: scheduleData,
                   success: function (data) {

                       if(window.location.pathname == '/customization')
                           window.location = '/customization2';
                       else if(window.location.pathname == '/customization2')
                           window.location = '/#plan';
                       else
                       {
                           // update calendar events
                           jQuery('#calendar').updatePlan(data.events);

                           // update plan tab
                           var plan = jQuery('#plan');
                           plan.find('.row, .head').remove();
                           jQuery(data.plan).find('.row, .head')
                               .insertBefore(plan.find('.pane-content p').last());

                           // update profile tab
                           profile.removeClass('valid').addClass('invalid');
                           profile.removeClass('building');
                       }
                   },
                   error: function () {
                       profile.removeClass('building');
                   }
               });

    });

});