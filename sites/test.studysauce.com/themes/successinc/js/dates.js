
jQuery(document).ready(function($) {

    var deadlines = $('#deadlines');

    deadlines.find('.sort-by label:last-child').tooltip({position:{my: 'center top+15', at:'center bottom'}, open: function (evt, ui) {
        if(jQuery(ui.tooltip).offset().left + jQuery(ui.tooltip).width() < jQuery(this).offset().left)
        {
            jQuery(this).tooltip('option', 'tooltipClass', 'left');
            ui.tooltip.addClass('left');
        }
        else if(jQuery(ui.tooltip).offset().left > jQuery(this).offset().left + jQuery(this).width())
        {
            jQuery(this).tooltip('option', 'tooltipClass', 'right');
            ui.tooltip.addClass('right');
        }
        else if(jQuery(ui.tooltip).offset().top + jQuery(ui.tooltip).height() < jQuery(this).offset().top)
        {
            jQuery(this).tooltip('option', 'tooltipClass', 'top');
            ui.tooltip.addClass('top');
        }
        else if(jQuery(ui.tooltip).offset().top > jQuery(this).offset().top + jQuery(this).height())
        {
            jQuery(this).tooltip('option', 'tooltipClass', 'bottom');
            ui.tooltip.addClass('bottom');
        }
    }});

    deadlines.on('click', '.row:not(.edit) > div:not(.field-name-field-reminder,.field-name-field.completed)', function (evt) {
        jQuery(this).parents('.row').toggleClass('selected');
    });

    deadlines.on('change', '.field-name-field-class-name select, .field-name-field-reminder input, .field-name-field-due-date input', function () {
        jQuery(this).parents('.row').datesFunc();
    });

    deadlines.on('keyup', '.field-name-field-assignment input', function () {
        jQuery(this).parents('.row').datesFunc();
    });

    deadlines.on('click', 'a[href="#add-deadline"]', function (evt) {
        evt.preventDefault();
        deadlines.find('a[href="#add-deadline"]').hide();
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
        deadlines.find('.highlighted-link').detach().insertAfter(newDeadline);
        deadlines.addClass('edit-date-only');
        deadlines.find('.field-name-field-due-date input').removeClass('hasDatepicker');
        newDeadline.scrollintoview().datesFunc();
        jQuery(window).trigger('scroll');
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
                       deadlines.find('.sort-by').nextAll('.row,.head').remove();
                       jQuery(data.reminders).find('.sort-by').nextAll('.row,.head').insertAfter(deadlines.find('.sort-by'));

                       // update calendar events
                       jQuery('#calendar').updatePlan(data.events);

                       // update plan tab
                       var plan = jQuery('#plan');
                       plan.find('.row, .head').remove();
                       jQuery(data.plan).find('.row, .head')
                           .insertBefore(plan.find('.pane-content p').last());

                       if(deadlines.find('.row').not('#new-dates-row').length == 0)
                           deadlines.find('a[href="#add-deadline"]').first().trigger('click');
                       jQuery(window).trigger('scroll');
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
        var headings = {},
            that = jQuery(this);
        jQuery('#deadlines .head').each(function () {
            var head = jQuery(this);
            head.nextUntil('*:not(.row)').each(function () {
                var row = jQuery(this),
                    that = row.find('.field-name-field-class-name .read-only');
                if(typeof headings[that.text().trim()] == 'undefined')
                    headings[that.text().trim()] = row;
                else
                    headings[that.text().trim()] = jQuery.merge(headings[that.text().trim()], row);
                that.html(that.html().replace(that.text().trim(), head.text().trim()));
            });
        });
        var rows = [];
        // sort headings by class name
        if(that.val() == 'class')
        {
            var keys = [];

            for(var i = 0; i < window.classNames.length; i++)
                if(typeof headings[window.classNames[i]] != 'undefined')
                    keys[keys.length] = window.classNames[i];

            for(var k in headings)
                if(keys.indexOf(k) == -1)
                    keys[keys.length] = k;

            for(var j = 0; j < keys.length; j++)
            {
                var hidden = headings[keys[j]].filter('.row:not(.hide)').length == 0;
                rows = jQuery.merge(rows, jQuery.merge(jQuery('<div class="head ' + (hidden ? 'hide' : '') + '">' + keys[j] + '</div>'), headings[keys[j]].detach()));
            }
        }
        else
        {
            var keys = [];
            for(var h in headings)
                keys[keys.length] = Date.parse(h);

            keys.sort();
            var monthNames = [ "January", "February", "March", "April", "May", "June",
                "July", "August", "September", "October", "November", "December" ];

            for(var j = 0; j < keys.length; j++)
            {
                var d = new Date(keys[j]),
                    h = d.getDate() + ' ' + monthNames[d.getMonth()] + ' ' + d.getFullYear();
                var hidden = headings[h].filter('.row:not(.hide)').length == 0;
                rows = jQuery.merge(rows, jQuery.merge(jQuery('<div class="head ' + (hidden ? 'hide' : '') + '">' + d.getDate() + ' ' + monthNames[d.getMonth()] + ' <span>' + d.getFullYear() + '</span></div>'), headings[h].detach()));
            }
        }
        jQuery('.sort-by').nextAll('.head').remove();
        jQuery(rows).insertAfter(deadlines.find('.sort-by'));
        // reassign first row
        deadlines.find('.first').removeClass('first');
        deadlines.find('.row:not(.hide,#new-dates-row)').first().addClass('first');
        deadlines.find('.row.hide').first().addClass('first');
    });

    deadlines.on('click', 'a[href="#save-dates"]', function (evt) {
        evt.preventDefault();

        if(deadlines.is('.invalid'))
            return;
        deadlines.removeClass('valid').addClass('invalid');

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
                           var invalids = deadlines.find('#new-dates-row').prevAll('.row.invalid').detach();

                           // update key dates list
                           deadlines.find('.sort-by').nextAll('.row,.head').remove();
                           jQuery(data.reminders).find('.sort-by').nextAll('.row,.head').insertAfter(deadlines.find('.sort-by'));

                           // remove valid rows after adding them to list
                           deadlines.find('#new-dates-row').prevAll('.row.valid').remove();
                           invalids.insertBefore(deadlines.find('#new-dates-row'));
                           if(invalids.length > 0)
                           {
                               deadlines.find('.highlighted-link').detach().insertAfter(invalids.last());
                               deadlines.find('a[href="#add-deadline"]').hide();
                           }
                           else
                           {
                               deadlines.find('.highlighted-link').detach().insertAfter(deadlines.find('.row').last());
                               deadlines.find('a[href="#add-deadline"]').show();
                           }

                           // update calendar events
                           jQuery('#calendar').updatePlan(data.events);

                           // update plan tab
                           var plan = jQuery('#plan');
                           plan.find('.row, .head').remove();
                           jQuery(data.plan).find('.row, .head')
                               .appendTo(plan.find('.pane-content'));

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
        var row = jQuery(this).parents('.row');
        row.addClass('edit').scrollintoview().datesFunc();
        jQuery('#deadlines').addClass('edit-date-only');
        deadlines.find('.highlighted-link').detach().insertAfter(row);
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
    };

    if(deadlines.find('.row').not('#new-dates-row').length == 0)
        deadlines.find('a[href="#add-deadline"]').first().trigger('click');
    else
        deadlines.find('.row').not('#new-dates-row').datesFunc();
});

