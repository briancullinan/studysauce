Date.prototype.addHours= function(h){
    this.setHours(this.getHours()+h);
    return this;
};

jQuery(document).ready(function($) {

    var schedule = $('#schedule, .page-path-schedule, .page-path-schedule2').first();

    jQuery('.field-name-field-time input[title]').tooltip({position: {my: 'center top+15', at: 'center bottom'}, open: function (evt, ui) {
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

    $.fn.planFunc = function () {
        jQuery(this).each(function () {
            var row = $(this).closest('.row');
            if(row.find('.field-name-field-class-name input').val().trim() == '' &&
                row.find('.field-name-field-day-of-the-week .form-type-checkboxes .form-type-checkbox input:not([value="weekly"]):checked').length == 0 &&
                row.find('.field-name-field-time input[name="schedule-value-date"]').val().trim() == '' &&
                row.find('.field-name-field-time input[name="schedule-value2-date"]').val().trim() == '')
                row.removeClass('invalid').addClass('valid blank');
            else if(row.find('.field-name-field-class-name input').val().trim() == '' ||
                (row.parent().is('.schedule') &&
                    row.find('.field-name-field-day-of-the-week .form-type-checkboxes .form-type-checkbox input:checked').length == 0) ||
               row.find('.field-name-field-time input[name="schedule-value-time"]').val().trim() == '' ||
               row.find('.field-name-field-time input[name="schedule-value2-time"]').val().trim() == '' ||
               row.find('.field-name-field-time input[name="schedule-value-date"]').val().trim() == '' ||
               row.find('.field-name-field-time input[name="schedule-value2-date"]').val().trim() == '')
                row.removeClass('valid blank').addClass('invalid');
            else
                row.removeClass('invalid blank').addClass('valid');

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
            row.find('input[name="schedule-value-time"][type="text"], input[name="schedule-value2-time"][type="text"]')
                .filter(':not(.is-timeEntry)')
                .timeEntry({
                    defaultTime: new Date(0, 0, 0, 6, 0, 0),
                    ampmNames: ['AM', 'PM'],
                    ampmPrefix: ' ',
                    fromTo: false,
                    show24Hours: false,
                    showSeconds: false,
                    spinnerImage: '',
                    timeSteps: [1,1,"1"]
                })
                .on('keypress', function (event) {
                    var that = jQuery(this),
                        row = that.parents('.row'),
                        from = row.find('input[name="schedule-value-time"]').timeEntry('getTime'),
                        to = row.find('input[name="schedule-value2-time"]').timeEntry('getTime');

                    if(that.data('processing'))
                        return;
                    that.data('processing', true);

                    var chr = String.fromCharCode(event.charCode === undefined ? event.keyCode : event.charCode);
                    if (chr < ' ') {
                        return;
                    }
                    var ampmSet = that.data('ampmSet') || false;
                    if(chr.toLowerCase() == 'a' || chr.toLowerCase() == 'p')
                        that.data('ampmSet', true);
                    else if (chr >= '0' && chr <= '9' && !ampmSet)
                    {
                        var time = that.timeEntry('getTime');
                        var hours = time.getHours();
                        var newTime = time;
                        if(hours < 7)
                            newTime = new Date(0, 0, 0, hours + 12, time.getMinutes(), 0);
                        // check the length in between to see if its longer than 12 hours
                        else if(hours >= 19)
                            newTime = new Date(0, 0, 0, hours - 12, time.getMinutes(), 0);

                        if((that.is('[name="schedule-value-time"]') && to == null || newTime.getTime() < to.getTime()) ||
                            (that.is('[name="schedule-value2-time"]') && from == null || newTime.getTime() > from.getTime()))
                            that.timeEntry('setTime', newTime);
                    }

                    that.data('processing', false);
                });

            // check for invalid time entry
            var from = row.find('input[name="schedule-value-time"]').timeEntry('getTime'),
                to = row.find('input[name="schedule-value2-time"]').timeEntry('getTime');
            if(from != null && to != null && (from.getTime() == to.getTime() || to.getTime() < from.getTime()))
                row.addClass('invalid-time');
            else
                row.removeClass('invalid-time');

            // check if there are any overlaps with the other rows
            var startDate = new Date(row.find('input[name="schedule-value-date"]').val());
            var endDate = new Date(row.find('input[name="schedule-value2-date"]').val());
            var startTime = new Date('1/1/1970 ' + row.find('input[name="schedule-value-time"]').val().replace(/[ap]m$/i, '').replace(/12:/i, '00:'));
            if(row.find('input[name="schedule-value-time"]').val().match(/pm$/i) != null)
                startTime = startTime.addHours(12);
            var endTime = new Date('1/1/1970 ' + row.find('input[name="schedule-value2-time"]').val().replace(/[ap]m$/i, '').replace(/12:/i, '00:'));
            if(row.find('input[name="schedule-value2-time"]').val().match(/pm$/i) != null)
                endTime = endTime.addHours(12);
            var dotw = row.find('.field-name-field-day-of-the-week input[name*="schedule-dotw-"]:checked').map(function (i, x) {return $(x).val();}).get();
            // reset overlaps tag to start
            var overlaps = row.is('.overlaps');
            row.removeClass('overlaps');
            schedule.find('.row').not(row).each(function () {
                var that = jQuery(this);
                // check if dates overlap
                var startDate2 = new Date(that.find('input[name="schedule-value-date"]').val());
                var endDate2 = new Date(that.find('input[name="schedule-value2-date"]').val());
                if(isNaN(startDate.getTime()) || isNaN(endDate.getTime()) ||
                    isNaN(startDate2.getTime()) || isNaN(endDate2.getTime()) ||
                    startDate <= endDate2 || endDate >= startDate2)
                {
                    // check if weekdays overlap
                    var dotwOverlaps = false,
                        dotw2 = that.find('.field-name-field-day-of-the-week input[name*="schedule-dotw-"]:checked').map(function (i, x) {return $(x).val();}).get();
                    for(var i in dotw)
                        if(dotw2.indexOf(dotw[i]) > -1)
                        {
                            dotwOverlaps = true;
                            break;
                        }
                    if(dotwOverlaps)
                    {
                        // check if times overlap
                        var startTime2 = new Date('1/1/1970 ' + that.find('input[name="schedule-value-time"]').val().replace(/[ap]m$/i, '').replace(/12:/i, '00:'));
                        if(that.find('input[name="schedule-value-time"]').val().match(/pm$/i) != null)
                            startTime2 = startTime2.addHours(12);
                        var endTime2 = new Date('1/1/1970 ' + that.find('input[name="schedule-value2-time"]').val().replace(/[ap]m$/i, '').replace(/12:/i, '00:'));
                        if(that.find('input[name="schedule-value2-time"]').val().match(/pm$/i) != null)
                            endTime2 = endTime2.addHours(12);
                        if(startTime < endTime2 && endTime > startTime2)
                        {
                            that.addClass('overlaps');
                            row.addClass('overlaps');
                        }
                    }
                }
            });

            // if it changed, remove other overlaps
            if(overlaps && !row.is('.overlaps'))
            {
                schedule.find('.row.overlaps').planFunc();
            }
        });

        if(window.location.pathname == '/schedule2' &&
            schedule.find('.row.edit.invalid:visible').length == 0 &&
            schedule.find('.row.overlaps:visible').length == 0 &&
            schedule.find('.row.invalid-time:visible').length == 0)
            schedule.removeClass('invalid invalid-only').addClass('valid');
        else if(window.location.pathname == '/schedule' &&
            schedule.find('.row.edit.invalid:visible').length == 0 &&
            schedule.find('.row.overlaps:visible').length == 0 &&
            schedule.find('.row.invalid-time:visible').length == 0 &&
            schedule.find('.row.edit.valid:visible').not('.blank').length > 0 &&
            schedule.find('.field-name-field-university input').val().trim() != '')
            schedule.removeClass('invalid invalid-only').addClass('valid');
        else if(
            schedule.find('.row.edit.invalid:visible').length == 0 &&
            schedule.find('.row.overlaps:visible').length == 0 &&
            schedule.find('.row.invalid-time:visible').length == 0 &&
            schedule.find('.field-name-field-university input').val().trim() != '' &&
                (schedule.find('.row.edit.valid:visible').not('.blank').length > 0 ||
                    (schedule.find('.row.valid').not('.blank').length > 0 &&
                        schedule.find('.field-name-field-university input').val() !=
                            schedule.find('.field-name-field-university input').prop('defaultValue'))))
            schedule.removeClass('invalid invalid-only').addClass('valid');
        else
            schedule.removeClass('valid').addClass('invalid');

        if(schedule.find('.row.overlaps:visible').length > 0 ||
            (window.location.pathname != '/schedule2' && schedule.find('.row.overlaps').length > 0))
            schedule.addClass('overlaps');
        else
            schedule.removeClass('overlaps');

        if(schedule.find('.row.invalid-time:visible').length > 0)
            schedule.addClass('invalid-time');
        else
            schedule.removeClass('invalid-time');

    };

    // set default value for university name
    if(schedule.find('.field-name-field-university input').val().trim() != '')
        schedule.find('.field-name-field-university input').prop('defaultValue', schedule.find('.field-name-field-university input').val().trim());

    schedule.on('click', 'a[href="#edit-class"]', function (evt) {
        evt.preventDefault();
        schedule.removeClass('edit-class-only edit-other-only').addClass('edit-class-only');
        var that = $(this),
            row = that.parents('.row');
        row.addClass('edit').planFunc();
        row.find('.field-name-field-time input[type="text"]').trigger('change');
    });

    function updateTabs(data, row)
    {
        // update calendar events
        jQuery('#calendar').updatePlan(data.events);

        // reset edit mode
        schedule.removeClass('edit-class-only edit-other-only');

        // update class schedule
        jQuery('.schedule .row').remove();
        jQuery(data.schedule).find('.schedule .row')
            .appendTo(jQuery('.schedule'));
        jQuery('.other-schedule .row').remove();
        jQuery(data.schedule).find('.other-schedule .row')
            .appendTo(jQuery('.other-schedule'));
        schedule.find('.schedule .row, .other-schedule .row').not('#add-class-dialog, #add-other-dialog').planFunc();

        // add empty other if there aren't any
        if(schedule.find('.other-schedule .row').not('#add-other-dialog').length == 0)
            schedule.find('a[href="#add-other"]').first().trigger('click');

        // update profile tab
        if(typeof $.fn.profileFunc != 'undefined')
        {
            jQuery('#profile .class-profile').replaceWith(jQuery(data.profile).find('.class-profile'));
            $.fn.profileFunc();
        }

        // update plan tab
        var plan = jQuery('#plan');
        plan.find('.row, .head').remove();
        jQuery(data.plan).find('.row, .head')
            .appendTo(plan.find('.pane-content'));

        // update SDS
        jQuery('#sds-messages .show').removeClass('show');
        if (typeof data.lastSDS != 'undefined')
            jQuery('#sds-messages .' + data.lastSDS).addClass('show');

        window.classNames = [];
        for(var eid in data.classes)
        {
            window.classNames[window.classNames.length] = data.classes[eid];
        }

        // update metrics
        jQuery('#metrics .row:not(.heading)').remove();
        jQuery('#checkins-list').append(data.rows);
        if(typeof $.fn.updateMetrics != 'undefined')
        jQuery('#metrics').updateMetrics(data.times);
        if (data.empty)
        {
            jQuery('#metrics').addClass('empty');
            jQuery('#metrics-empty').dialog();
            // update metrics classes
            var mc = 0;
            jQuery('#metrics ol li').remove();
            for(var eid in data.metricsClasses)
            {
                jQuery('#metrics ol').append('<li><span class="class' + mc + '">&nbsp;</span>' + data.metricsClasses[eid] + '</li>');
                mc++;
            }
        }
        else
        {
            jQuery('#metrics').removeClass('empty');
            jQuery('#metrics-empty').dialog('hide');

            // update metrics key
            var mc = 0;
            jQuery('#metrics ol li').remove();
            for(var eid in data.classes)
            {
                jQuery('#metrics ol').append('<li><span class="class' + mc + '">&nbsp;</span>' + data.metricsClasses[eid] + '</li>');
                mc++;
            }
        }
        jQuery('#study-total').text(data.empty ? data.total : '5.9 hours');
        jQuery('#timeline > h4').html('Goal: ' + (data.hours > 0 ? data.hours : (data.empty ? '20 hours' : '&nbsp;')));

        // update checkin buttons
        jQuery('#checkin .classes a').remove();
        var c = 0,
            checkedIn = jQuery('#checkin .classes a.checked-in').length > 0 ? jQuery('#checkin .classes a.checked-in').attr('id').substring(5) : '';
        for(var eid in data.classes)
        {
            jQuery('#checkin .classes').append('<a href="#class' + c + '" class="class' + c + ' cid' + eid + ' ' + (checkedIn == eid ? 'checked-in' : '') + '"><span>&nbsp;</span>' + (data.classes[eid].length > 20 ? (data.classes[eid] + '...') : data.classes[eid]) + '</a>');
            c++;
        }
        if(c > 0)
        {
            jQuery('#checkin').removeClass('empty');
            jQuery('#empty-schedule').dialog('hide');
            jQuery('#deadlines').removeClass('empty');
            jQuery('#empty-dates').dialog('hide');
            jQuery('#home-schedule').attr('checked', 'checked');
        }
        else
        {
            jQuery('#checkin').addClass('empty');
            jQuery('#empty-schedule').dialog();
            jQuery('#deadlines').addClass('empty');
            jQuery('#empty-dates').dialog();
            jQuery('#checkin .classes').html('<a href="#class0" class="class0"><span>&nbsp;</span>Hist 135</a>' +
                                             '<a href="#class1" class="class1"><span>&nbsp;</span>Chem 151</a>' +
                                             '<a href="#class2" class="class2"><span>&nbsp;</span>Math 125</a>' +
                                             '<a href="#class3" class="class3"><span>&nbsp;</span>Phys 101</a>' +
                                             '<a href="#class4" class="class4"><span>&nbsp;</span>Phys Lab</a>' +
                                             '<a href="#class5" class="class5"><span>&nbsp;</span>Econ 101</a>' +
                                             '<a href="#class6" class="class6"><span>&nbsp;</span>Soc 200</a>' +
                                             '<a href="#class7" class="class7"><span>&nbsp;</span>Law 345</a>' +
                                             '<a href="#class8" class="class8"><span>&nbsp;</span>Hist 136</a>');
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

    }

    schedule.on('click', 'a[href="#add-class"]', function (evt) {
        evt.preventDefault();
        var examples = ['HIST 101', 'CALC 120', 'MAT 200', 'PHY 110', 'BUS 300', 'ANT 350', 'GEO 400', 'BIO 250', 'CHM 180', 'PHIL 102', 'ENG 100'],
            count = schedule.find('.row').length,
            addClass = schedule.find('#add-class-dialog').last(),
            newClass = addClass.clone().attr('id', '').addClass('edit').insertBefore(addClass).show();
        newClass.find('input[type="checkbox"], input[type="radio"]').each(function () {
            var that = jQuery(this),
                oldId = that.attr('id');
            that.attr('id', oldId + count);
            if(that.is('[type="radio"]'))
                that.attr('name', that.attr('name') + count);
            newClass.find('label[for="' + oldId + '"]').attr('for', oldId + count);
        });
        newClass.find('.field-name-field-class-name input')
            .attr('placeholder', examples[Math.floor(Math.random() * examples.length)]);
        newClass.find('.field-name-field-time input[title]').tooltip({position: {my: 'center top+15', at: 'center bottom'}, open: function (evt, ui) {
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
        newClass.planFunc();
        schedule.removeClass('edit-other-only').addClass('edit-class-only');
    });

    schedule.on('click', 'a[href="#add-other"]', function (evt) {
        evt.preventDefault();
        var examples = ['Work', 'Practice', 'Gym', 'Meeting'],
            count = schedule.find('.row').length,
            addOther = schedule.find('#add-other-dialog').last(),
            newClass = addOther.clone().attr('id', '').addClass('edit').insertBefore(addOther).show();
        newClass.find('input[type="checkbox"], input[type="radio"]').each(function () {
            var that = jQuery(this),
                oldId = that.attr('id');
            that.attr('id', oldId + count);
            if(that.is('[type="radio"]'))
                that.attr('name', that.attr('name') + count);
            newClass.find('label[for="' + oldId + '"]').attr('for', oldId + count);
        });
        newClass.find('.field-name-field-class-name input')
            .attr('placeholder', examples[Math.floor(Math.random() * examples.length)]);
        newClass.find('.field-name-field-recurring input[value="weekly"]').prop('checked', true);
        newClass.find('.field-name-field-time input[title]').tooltip({position: {my: 'center top+15', at: 'center bottom'}, open: function (evt, ui) {
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
        newClass.planFunc();
        schedule.removeClass('edit-class-only').addClass('edit-other-only');
    });

    // add empty other if there aren't any
    if(schedule.find('.other-schedule .row').not('#add-other-dialog').length == 0)
        schedule.find('a[href="#add-other"]').first().trigger('click');

    schedule.on('click', 'a[href="#remove-class"]', function (evt) {
        evt.preventDefault();
        var row = jQuery(this).parents('.row');
        schedule.find('#schedule-building').dialog();
        schedule.find('.timer').pietimer('reset');
        schedule.find('.timer').pietimer({
            timerSeconds: 30,
            color: '#09B',
            fill: false,
            showPercentage: true,
            callback: function() {
            }
        });
        schedule.find('.timer').pietimer('start');
        $.ajax({
                   url: '/node/save/schedule',
                   type: 'POST',
                   dataType: 'json',
                   data: {
                       remove: row.attr('id').substr(0, 4) == 'eid-' ? row.attr('id').substring(4) : null
                   },
                   success: function (data) {
                       updateTabs(data);
                       schedule.find('#schedule-building').dialog('hide')
                   }
               });
    });

    schedule.on('click', 'a[href="#save-class"]', function (evt) {
        evt.preventDefault();
        if(schedule.find('.field-name-field-university input').val().trim() == '')
            schedule.find('.field-name-field-university').addClass('error-empty');
        else
            schedule.find('.field-name-field-university').removeClass('error-empty');
        if(schedule.is('.invalid'))
        {
            schedule.addClass('invalid-only');
            schedule.find('.row.edit.invalid .field-name-field-time input').each(function () {
                if(jQuery(this).val().trim() == '')
                    jQuery(this).parents('.row').addClass('invalid-time');
            });
            return;
        }
        schedule.removeClass('valid').addClass('invalid');

        var classes = [];
        schedule.find('.row.edit.valid:visible, .row.valid.edit:visible').each(function () {
            var row = $(this),
                dotw = row.find('.field-name-field-day-of-the-week input[name*="schedule-dotw-"]:checked').map(function (i, x) {return $(x).val();}).get();
            if(row.find('.field-name-field-recurring input[value="weekly"]:checked').length > 0)
                dotw[dotw.length] = 'weekly';
            else if(row.find('.field-name-field-recurring input[value="monthly"]:checked').length > 0)
                dotw[dotw.length] = 'monthly';
            else if(row.find('.field-name-field-recurring input[value="yearly"]:checked').length > 0)
                dotw[dotw.length] = 'yearly';
            classes[classes.length] = {
                cid: typeof row.attr('id') != 'undefined' && row.attr('id').substr(0, 4) == 'eid-' ? row.attr('id').substring(4) : null,
                className: row.find('.field-name-field-class-name input').val(),
                dotw: dotw.join(','),
                start: row.find('.field-name-field-time input[name="schedule-value-date"]').val() + ' ' + row.find('.field-name-field-time input[name="schedule-value-time"]').val(),
                end: row.find('.field-name-field-time input[name="schedule-value2-date"]').val() + ' ' + row.find('.field-name-field-time input[name="schedule-value2-time"]').val(),
                type: row.find('input[name="schedule-type"]').val()
            };
        });

        if(window.location.pathname != '/schedule' &&
            window.location.pathname != '/schedule2')
        {
            schedule.find('#schedule-building').dialog();
            schedule.find('.timer').pietimer('reset');
            schedule.find('.timer').pietimer({
                timerSeconds: 60,
                color: '#09B',
                fill: false,
                showPercentage: true,
                callback: function() {
                }
            });
            schedule.find('.timer').pietimer('start');
        }

        $.ajax({
                   url: '/node/save/schedule',
                   type: 'POST',
                   dataType: 'json',
                   data: {
                       // skip building the schedule if we are in the middle of the buy funnel
                       skipBuild: (window.location.pathname == '/schedule' || window.location.pathname == '/schedule2'),
                       university: schedule.find('.field-name-field-university input').val(),
                       classes: classes
                   },
                   success: function (data) {

                       if(window.location.pathname == '/schedule')
                           window.location = '/schedule2';
                       else if(window.location.pathname == '/schedule2')
                           window.location = '/customization';
                       else
                       {
                           updateTabs(data);
                       }
                       schedule.find('#schedule-building').dialog('hide')
                   },
                   error: function () {
                       schedule.find('#schedule-building').dialog('hide')
                   }
               });
    });

    var checkDate = function () {
        var row = jQuery(this).closest('.row');
        if(row.find('.field-name-field-class-name input').val() != '' &&
            row.find('.field-name-field-time input[name="schedule-value-date"]').val() == '' &&
            row.find('.field-name-field-time input[name="schedule-value2-date"]').val() == '' &&
            schedule.find('.row').first().find('input[name="schedule-value-date"]').val() != '' &&
            schedule.find('.row').first().find('input[name="schedule-value2-date"]').val() != '' &&
            row[0] != schedule.find('.row').first()[0])
        {
            // use first rows dates
            row.find('.field-name-field-time input[name="schedule-value-date"]').val(schedule.find('.row').first().find('input[name="schedule-value-date"]').val());
            row.find('.field-name-field-time input[name="schedule-value2-date"]').val(schedule.find('.row').first().find('input[name="schedule-value2-date"]').val());
        }
    };
    schedule.on('change', '.field-name-field-class-name input', checkDate);
    schedule.on('keyup', '.field-name-field-class-name input', checkDate);
    schedule.on('change', '.field-name-field-time input', function () {
        jQuery(this).parents('.row').nextUntil(':not(.row)').each(function () {
            checkDate.apply(this);
        });
        if(jQuery(this).is('[type="time"]'))
            jQuery(this).parent().find('input[type="text"]').timeEntry('setTime', jQuery(this).val());
        if(jQuery(this).is('.is-timeEntry[type="text"]'))
        {
            var t = jQuery(this).timeEntry('getTime');
            if(typeof t != 'undefined')
                jQuery(this).parent().find('input[type="time"]').val((t.getHours() < 10
                    ? ('0' + t.getHours())
                    : t.getHours()) + ':' + (t.getMinutes() < 10
                    ? ('0' + t.getMinutes())
                    : t.getMinutes()) + ':00');
        }
    });
    schedule.on('keyup', '.field-name-field-time input', function () {
        jQuery(this).parents('.row').nextUntil(':not(.row)').each(function () {
            checkDate.apply(this);
        });
    });
    schedule.on('keyup', '.field-name-field-class-name input, .field-name-field-time input, .field-name-field-university input', function () {
        jQuery(this).parents('.row').planFunc();
    });
    schedule.on('change', '.field-name-field-class-name input, .field-name-field-day-of-the-week input, .field-name-field-time input, .field-name-field-university input', function () {
        jQuery(this).parents('.row').planFunc();
    });

    var safety = false;
    schedule.on('change', '.field-name-field-recurring input', function () {
        var that = $(this),
            row = that.parents('.row');

        if(that.is('[value="monthly"]') && row.find('.field-name-field-recurring input[value="weekly"]').is(':checked'))
            row.find('.field-name-field-recurring input[value="weekly"]').prop('checked', false);
        if(that.is('[value="weekly"]') && row.find('.field-name-field-recurring input[value="monthly"]').is(':checked'))
            row.find('.field-name-field-recurring input[value="monthly"]').prop('checked', false);

        //if(row.find('.field-name-field-recurring input[value="weekly"]').is(':checked'))
        //    row.find('.field-name-field-day-of-the-week .form-checkboxes .form-type-checkbox').css('visibility', 'visible');
        //else
        //    row.find('.field-name-field-day-of-the-week .form-checkboxes .form-type-checkbox').css('visibility', 'hidden');

    });

    schedule.find('.schedule .row, .other-schedule .row').not('#add-class-dialog, #add-other-dialog').planFunc();

});