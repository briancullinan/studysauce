
jQuery(document).ready(function($) {

    $.fn.planFunc = function () {
        jQuery(this).each(function () {
            var row = $(this).closest('.row');
            if(row.find('.field-name-field-class-name input').val().trim() == '' ||
               row.find('.field-name-field-day-of-the-week .form-type-checkboxes .form-type-checkbox input:checked, ' +
                        '.field-name-field-day-of-the-week .form-type-checkboxes.form-type-checkbox input[value="monthly"]:checked, ' +
                        '.field-name-field-day-of-the-week .form-type-checkboxes.form-type-checkbox input[value="yearly"]:checked').length == 0 ||
               row.find('.field-name-field-time input[name="schedule-value-time"]').val().trim() == '' ||
               row.find('.field-name-field-time input[name="schedule-value2-time"]').val().trim() == '' ||
               row.find('.field-name-field-time input[name="schedule-value-date"]').val().trim() == '' ||
               row.find('.field-name-field-time input[name="schedule-value2-date"]').val().trim() == '')
                row.removeClass('valid').addClass('invalid');
            else
                row.removeClass('invalid').addClass('valid');

            row.find('input[name="schedule-value-date"], input[name="schedule-value2-date"]')
                .datepicker({
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

            row.find('input[name="schedule-value-time"], input[name="schedule-value2-time"]')
                .timeEntry({
                               defaultTime: new Date(0, 0, 0, (new Date()).getHours(), 0, 0),
                               ampmNames: ['am', 'pm'],
                               fromTo: false,
                               show24Hours: false,
                               showSeconds: false,
                               spinnerImage: '',
                               timeSteps: [1,1,"1"]
                           });

        });
    };

    $('#schedule').on('click', 'a[href="#edit-class"]', function (evt) {
        evt.preventDefault();
        jQuery('#schedule').removeClass('edit-class-only edit-other-only').addClass('edit-class-only');
        var that = $(this),
            row = that.parents('.row');
        row.addClass('edit').planFunc();
    });

    $('#schedule').on('click', 'a[href="#cancel-class"]', function (evt) {
        evt.preventDefault();
        jQuery(this).parents('.row').removeClass('edit');
        jQuery('#schedule').removeClass('edit-class-only edit-other-only').scrollintoview();
    });

    function updateTabs(data, row)
    {
        // update calendar events
        window.planEvents = data.events;
        if(calendar && typeof calendar.fullCalendar != 'undefined')
            calendar.fullCalendar('refetchEvents');

        // update class schedule
        jQuery('.schedule .row').remove();
        jQuery(data.schedule).find('.schedule .row')
            .appendTo(jQuery('.schedule'));
        jQuery('.other-schedule .row').remove();
        jQuery(data.schedule).find('.other-schedule .row')
            .appendTo(jQuery('.other-schedule'));

        // update plan tab
        var plan = jQuery('#plan');
        plan.find('.row, .head').remove();
        jQuery(data.plan).find('.row, .head')
            .insertBefore(plan.find('.pane-content p').last());

        // update SDS
        jQuery('#sds-messages .show').removeClass('show');
        if (typeof data.lastSDS != 'undefined')
            jQuery('#sds-messages .' + data.lastSDS).addClass('show');

        // update metrics
        jQuery('#metrics .row:not(.heading)').remove();
        jQuery('#checkins-list').append(data.rows);
        jQuery('#metrics').updateMetrics(data.times);
        if (data.times.length == 0)
            jQuery('#metrics').addClass('empty');
        else
            jQuery('#metrics').removeClass('empty');
        jQuery('#study-total').text(data.total);
        jQuery('#timeline > h4').html(data.hours > 0 ? ('Goal: ' + data.hours + ' hours') : '&nbsp;');

        // update checkin buttons
        jQuery('#checkin .classes a').remove();
        var c = 0,
            checkedIn = jQuery('#checkin .classes a.checked-in').length > 0 ? jQuery('#checkin .classes a.checked-in').attr('id').substring(5) : '';
        for(var eid in data.classes)
        {
            jQuery('#checkin .classes').append('<a id="class' + eid + '" href="#class' + c + '" class="class' + c + ' ' + (checkedIn == eid ? 'checked-in' : '') + '"><span>&nbsp;</span>' + data.classes[eid] + '</a>');
            c++;
        }
        if(c > 0)
        {
            jQuery('#checkin').removeClass('empty edit-schedule');
            jQuery('#deadlines').removeClass('empty');
        }
        else
        {
            jQuery('#checkin').addClass('empty edit-schedule');
            jQuery('#deadlines').addClass('empty');
        }

        // fix class list on key dates tab
        jQuery('#deadlines .row').each(function (i, r) {
            var row = jQuery(r),
                selected = row.find('.field-name-field-class-name select').val();
            row.find('.field-name-field-class-name option:not([value="_none"]):not([value="Nonacademic"])').remove();
            for(var eid in data.classes)
            {
                jQuery('<option value="' + data.classes[eid] + '" ' + (data.classes[eid] == selected ? 'selected' : '') + '>' + data.classes[eid] + '</option>').insertBefore(row.find('.field-name-field-class-name option').last());
            }
        });

        // update awards such as pulse detected for setting up a class
        if (typeof data.awards != 'undefined') {
            var lastAward = null;
            for (var i in data.awards) {
                if (data.awards[i] != false && jQuery('#badges #' + i).is('.not-awarded')) {
                    jQuery('#badges #' + i).removeClass('not-awarded').addClass('awarded');
                    lastAward = i;
                }
            }

            if (lastAward == 'setup-pulse')
                jQuery('#badges').relocateAward(lastAward, '#schedule > .pane-content');
            else if (lastAward != null)
                jQuery('#badges').relocateAward(lastAward, '#badges > .pane-content');
            else
                jQuery('#badges').relocateAward('');
        }

        // scroll back to tab they clicked edit on
    }

    /*
    jQuery('body').on('click', 'a[href="#schedule"]', function (evt) {
        jQuery('#schedule').removeClass('edit-other-only').addClass('edit-class-only');
        jQuery('#schedule #schedule-type').val('c');
        jQuery('#schedule #schedule-weekly').prop('checked', true);
        jQuery('#schedule .field-add-more-submit').hide();
        jQuery('#schedule #add-class-dialog').addClass('edit').scrollintoview({padding: {top:120,bottom:200,left:0,right:0}});
    });
    */
    $('#schedule').on('click', 'a[href="#add-class"]', function (evt) {
        evt.preventDefault();
        jQuery('#schedule #add-class-dialog').addClass('edit');
        jQuery('#schedule').removeClass('edit-other-only').addClass('edit-class-only');
    });

    $('#schedule').on('click', 'a[href="#add-other"]', function (evt) {
        evt.preventDefault();
        jQuery('#schedule #add-other-dialog').addClass('edit');
        jQuery('#schedule').removeClass('edit-class-only').addClass('edit-other-only');
    });

    $('#schedule').on('click', 'a[href="#remove-class"]', function (evt) {
        evt.preventDefault();
        var row = jQuery(this).parents('.row');
        $.ajax({
                   url: '/node/save/schedule',
                   type: 'POST',
                   dataType: 'json',
                   data: {
                       remove: row.attr('id').substr(0, 4) == 'eid-' ? row.attr('id').substring(4) : null
                   },
                   success: function (data) {
                       updateTabs(data);
                   }
               });
    });

    function clearInputs()
    {
        $('#schedule #schedule-class-name').val('');
        $('#schedule #schedule-type').val('');
        $('#schedule #add-class-dialog input[name*="schedule-dotw-"]:checked').prop('checked', false);
        $('#schedule #schedule-value-date').val('');
        $('#schedule #schedule-value-time').val('');
        $('#schedule #schedule-value2-date').val('');
        $('#schedule #schedule-value2-time').val('');
    }

    $('#schedule').on('click', 'a[href="#save-class"]', function (evt) {
        evt.preventDefault();
        var that = $(this),
            row = that.parents('.row');
        if(!row.is('.invalid'))
        {
            var dotw = row.find('.field-name-field-day-of-the-week input[name*="schedule-dotw-"]:checked').map(function (i, x) {return $(x).val();}).get();
            if(row.find('.field-name-field-recurring input[value="monthly"]:checked').length > 0)
                dotw[dotw.length] = 'monthly';
            else if(row.find('.field-name-field-recurring input[value="yearly"]:checked').length > 0)
                dotw[dotw.length] = 'yearly';
            $.ajax({
                       url: '/node/save/schedule',
                       type: 'POST',
                       dataType: 'json',
                       data: {
                           className: row.find('.field-name-field-class-name input').val(),
                           dotw: dotw.join(','),
                           start: row.find('.field-name-field-time input[name="schedule-value-date"]').val() + ' ' + row.find('.field-name-field-time input[name="schedule-value-time"]').val(),
                           end: row.find('.field-name-field-time input[name="schedule-value2-date"]').val() + ' ' + row.find('.field-name-field-time input[name="schedule-value2-time"]').val(),
                           type: row.find('input[name="schedule-type"]').val()
                       },
                       success: function (data) {
                           clearInputs();

                           // reset edit mode on saved row, in-case it is the add row
                           $('#schedule').removeClass('edit-class-only edit-other-only');
                           row.removeClass('edit valid').addClass('invalid');
                           jQuery('#schedule .field-add-more-submit').show();

                           updateTabs(data);
                       }
                   });
        }
    });

    $('#schedule').on('keyup', '.field-name-field-class-name input', function () {
        jQuery(this).parents('.row').planFunc();
    });
    $('#schedule').on('change', '.field-name-field-class-name input', function () {
        jQuery(this).parents('.row').planFunc();
    });
    $('#schedule').on('change', '.field-name-field-day-of-the-week input', function () {
        jQuery(this).parents('.row').planFunc();
    });
    $('#schedule').on('change', '.field-name-field-day-of-the-week .form-checkboxes.form-type-checkbox input', function () {
        var that = $(this),
            row = that.parents('.row');
        if(that.is(':checked') && that.val() == 'daily')
            row.find('input[name*="schedule-dotw-"]').prop('checked', true);
        else
            row.find('input[name*="schedule-dotw-"]').prop('checked', false);

        if(that.is(':checked') && that.val() == 'weekly')
            row.find('.field-name-field-day-of-the-week .form-checkboxes .form-type-checkbox').show();
        else
            row.find('.field-name-field-day-of-the-week .form-checkboxes .form-type-checkbox').hide();
    });
    $('#schedule').on('keyup', '.field-name-field-time input[type="text"]', function () {
        jQuery(this).parents('.row').planFunc();
    });
    $('#schedule').on('change', '.field-name-field-time input[type="text"]', function () {
        jQuery(this).parents('.row').planFunc();
    });
    $('#schedule .schedule .row, #schedule .other-schedule .row, #add-class-dialog, #add-other-dialog').planFunc();

});