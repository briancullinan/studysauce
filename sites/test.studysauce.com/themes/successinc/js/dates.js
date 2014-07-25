
jQuery(document).ready(function($) {

    var deadlines = $('#deadlines');
    deadlines.on('click', '.row .field-name-field-assignment .read-only', function () {
        jQuery(this).parents('.row').toggleClass('selected');
    });

    deadlines.on('change', '.field-name-field-class-name select, .field-name-field-reminder input, .field-name-field-due-date input', function () {
        jQuery(this).parents('.row').datesFunc();
    });

    deadlines.on('click', 'a[href="#cancel-dates"]', function (evt) {
        evt.preventDefault();
        deadlines.removeClass('edit-date-only').scrollintoview();
        deadlines.find('.field-add-more-submit').show();
        jQuery(this).parents('.row').removeClass('edit');
    });

    deadlines.on('keyup', '.field-name-field-assignment input', function () {
        jQuery(this).parents('.row').datesFunc();
    });

    deadlines.on('click', 'a[href="#add-deadline"]', function (evt) {
        evt.preventDefault();
        var count = deadlines.find('.row').length,
            addDeadline = deadlines.find('#new-dates-row').last(),
            newDeadline = addDeadline.clone().attr('id', '').addClass('edit').insertBefore(addDeadline);
        newDeadline.find('input[type="checkbox"], input[type="radio"]').each(function () {
            var that = jQuery(this),
                oldId = that.attr('id');
            that.attr('id', oldId + count);
            if(that.is('[type="radio"]'))
                that.attr('name', that.attr('name') + count);
            newDeadline.find('label[for="' + oldId + '"]').attr('for', oldId + count);
        });
        newDeadline.scrollintoview().datesFunc();
        deadlines.addClass('edit-date-only');
    });

    deadlines.on('click', 'a[href="#remove-reminder"]', function (evt) {
        evt.preventDefault();
        var row = jQuery(this).parents('.row');
        $.ajax({
                   url: '/node/save/key_dates',
                   type: 'POST',
                   dataType: 'json',
                   data: {
                       remove: row.attr('id').substr(0, 4) == 'eid-' ? row.attr('id').substring(4) : null
                   },
                   success: function (data) {
                       deadlines.find('.row, .head, a[href="#add-deadline"]')
                           .replaceWith(jQuery(data.reminders).find('.row, .head, a[href="#add-deadline"]'));

                       // update calendar events
                       window.planEvents = data.events;

                       // update plan tab
                       var plan = jQuery('#plan');
                       plan.find('.row, .head').remove();
                       jQuery(data.plan).find('.row, .head')
                           .insertBefore(plan.find('.pane-content p').last());

                       if(deadlines.find('.row').not('#new-dates-row').length == 0)
                           deadlines.find('a[href="#add-deadline"]').first().trigger('click');
                   }
               });
    });

    deadlines.on('change', '#deadlines-historic', function () {
        if(jQuery(this).is(':checked'))
            deadlines.addClass('show-historic');
        else
            deadlines.removeClass('show-historic');
    });

    deadlines.on('change', '.sort-by input[type="radio"]', function (evt) {
        var headings = {};
        jQuery('#deadlines .head').each(function () {
            var head = jQuery(this);
            head.nextUntil('*:not(.row)').each(function () {
                var row = jQuery(this),
                    that = row.find('.field-name-field-class-name .read-only');
                if(typeof headings[that.text().substring(1)] == 'undefined')
                    headings[that.text().substring(1)] = row;
                else
                    headings[that.text().substring(1)] = jQuery.merge(headings[that.text().substring(1)], row);
                that.html(that.html().replace(that.text().substring(1), head.text()));
            });
        });
        var rows = [];
        for(var h in headings)
        {
            var hidden = headings[h].filter('.row:not(.hide)').length == 0;
            rows = jQuery.merge(rows, jQuery.merge(jQuery('<div class="head ' + (hidden ? 'hide' : '') + '">' + h + '</div>'), headings[h].detach()));
        }
        deadlines.find('.head, .row').not('#new-dates-row').not(deadlines.find('#new-dates-row').prevUntil(':not(.row)')).replaceWith(rows);
    });

    // move the save button to the current row if the window is smaller that the content length
    jQuery(window).scroll(function () {
        if(jQuery(window).height() < deadlines.height())
        {
            // get edit row closest to the center of the window
            var closest = {};
            var keys = [];
            deadlines.find('.row.edit').add(deadlines.find('.row').last()).each(function () {
                var center = Math.abs(jQuery(window).height() / 2 - (jQuery(this).offset().top - jQuery(window).scrollTop()));
                closest[center] = jQuery(this);
                keys[keys.length] = center;
            });
            keys.sort(function(a, b){return a-b});
            deadlines.find('.highlighted-link').detach().insertAfter(closest[keys[0]]);
        }
        else if(deadlines.find('.row').last().next()[0] != deadlines.find('.highlighted-link')[0])
            deadlines.find('.highlighted-link').detach().insertAfter(deadlines.find('.row').last());
    });
    jQuery(window).resize(function () {
        jQuery(window).trigger('scroll');
    });


    deadlines.on('click', 'a[href="#save-dates"]', function (evt) {
        evt.preventDefault();
        var dates = [];
        deadlines.find('.row.edit.valid, .row.valid.edit').each(function () {
            var row = $(this),
                reminders = row.find('input[name*="dates-reminder-"]:checked').map(function (i, x) {return $(x).val();}).get();
                dates[dates.length] = {
                    eid: typeof row.attr('id') != 'undefined' && row.attr('id').substr(0, 4) == 'eid-' ? row.attr('id').substring(4) : null,
                    className: row.find('.field-name-field-class-name select').val(),
                    assignment: row.find('.field-name-field-assignment input').val(),
                    reminders: reminders.join(','),
                    due: row.find('.field-name-field-due-date input').val(),
                    percent: row.find('.field-name-field-percent').is(':visible') ? row.find('.field-name-field-percent input').val() : 0
                };
        });
        if(dates.length > 0)
        {
            $.ajax({
                       url: '/node/save/key_dates',
                       type: 'POST',
                       dataType: 'json',
                       data: {
                           dates: dates
                       },
                       success: function (data) {
                           // clear input form
                           if(deadlines.find('#new-dates-row.edit.valid'))
                           {
                               deadlines.find('#new-dates-row').find('.field-name-field-class-name select').val('_none');
                               deadlines.find('#new-dates-row').find('.field-name-field-assignment input').val('');
                               deadlines.find('#new-dates-row').find('input[name*="dates-reminder-"]').prop('checked', false);
                               deadlines.find('#new-dates-row').find('.field-name-field-due-date input').val('');
                               deadlines.find('#new-dates-row').find('.field-name-field-percent input').val(0);
                           }

                           // update key dates list
                           var invalids = deadlines.find('#new-dates-row').prevAll('.row.invalid').detach();
                           deadlines.find('.row, .head, a[href="#add-deadline"]')
                               .replaceWith(jQuery(data.reminders).find('.row, .head, a[href="#add-deadline"]'));
                           invalids.insertBefore(deadlines.find('#new-dates-row'));

                           // update calendar events
                           window.planEvents = data.events;

                           // update plan tab
                           var plan = jQuery('#plan');
                           plan.find('.row, .head').remove();
                           jQuery(data.plan).find('.row, .head')
                               .insertBefore(plan.find('.pane-content p').last());

                           // update deadline view state
                           deadlines.removeClass('edit-date-only');
                           deadlines.find('.row').not('#new-dates-row').datesFunc();
                           deadlines.find('.field-add-more-submit').show();
                           //if(jQuery(window).width() <= 759)
                           //    row.scrollintoview({padding: {top:120,bottom:200,left:0,right:0}});
                           jQuery('#home-reminders').attr('checked', 'checked');
                           if(jQuery('#home').find('input[type="checkbox"]:checked').length == jQuery('#home').find('input[type="checkbox"]').length - 1)
                               jQuery('#home-tasks-checklist').attr('checked', 'checked');
                       }
                   });
        }
    });

    deadlines.on('click', 'a[href="#edit-reminder"]', function (evt) {
        evt.preventDefault();
        jQuery(this).parents('.row').addClass('edit').scrollintoview().datesFunc();
        jQuery('#deadlines').addClass('edit-date-only');
    });

    $.fn.datesFunc = function () {
        jQuery(this).each(function () {
            var that = jQuery(this),
                error = false;
            if(that.find('select').val() == '_none')
                error = true;
            if(that.find('.field-name-field-assignment input').val().trim() == '')
                error = true;
            if(that.find('.field-name-field-reminder input:checked').length == 0)
                error = true;
            if(that.find('.field-name-field-due-date input').val().trim() == '')
                error = true;

            if(error)
                that.removeClass('valid').addClass('invalid');
            else
                that.removeClass('invalid').addClass('valid');

            if(that.find('.field-name-field-class-name select').val() == 'Nonacademic')
                that.find('.field-name-field-percent').hide();
            else
                that.find('.field-name-field-percent').show();

            that.find('.field-name-field-due-date input').datepicker({
                                                                         minDate: 0,
                                                                         autoPopUp:'focus',
                                                                         changeMonth: true,
                                                                         changeYear: true,
                                                                         closeAtTop: false,
                                                                         dateFormat: 'mm/dd/yy',
                                                                         defaultDate:'0y',
                                                                         firstDay:0,
                                                                         fromTo:false,
                                                                         speed:'immediate',
                                                                         yearRange: '-3:+3'
                                                                     });
        });

        if(deadlines.find('.row.edit.valid').length == 0)
            deadlines.removeClass('valid').addClass('invalid');
        else
            deadlines.removeClass('invalid').addClass('valid');
        // don't need to limit submit because it goes back to edit mode if invalid
        /*jQuery('#deadlines input[value="Save"]').each(function () {
         var that = jQuery(this),
         id = that.attr('id');
         that.unbind(Drupal.ajax[id].event);
         that.bind(Drupal.ajax[id].event, function (event) {
         if(!that.parents('form').is('.invalid'))
         return Drupal.ajax[id].eventResponse(this, event);
         });
         });*/
    };

    if(deadlines.find('.row').not('#new-dates-row').length == 0)
        deadlines.find('a[href="#add-deadline"]').first().trigger('click');
    else
        deadlines.find('.row').not('#new-dates-row').datesFunc();
});

