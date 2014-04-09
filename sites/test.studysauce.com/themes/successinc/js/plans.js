var calendar = null;

jQuery(document).ready(function($) {

    $.propHooks.checked = {
        set: function(elem, value, name) {
            var ret = (elem[ name ] = value);
            $(elem).trigger("change");
            return ret;
        }
    };

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

    $('#plan').on('click', 'a[href="#edit-class"]', function (evt) {
        jQuery('#plan').removeClass('edit-class-only edit-other-only').addClass('edit-class-only');
        var that = $(this),
            row = that.parents('.row');
        row.addClass('edit').planFunc();
    });

    $('#plan').on('click', 'a[href="#save-schedule"]', function (evt) {
        evt.preventDefault();
        if(!$(this).parents('.row').is('.invalid'))
        {
            $.ajax({
                       url: '/node/save/schedule',
                       type: 'POST',
                       data: {
                           university: $('#plan #schedule-university').val(),
                           grades: $('#plan .field-name-field-grades input:checked').val(),
                           weekends: $('#plan .field-name-field-weekends input:checked').val(),
                           '6-am-11-am': $('#plan .field-name-field-6-am-11-am input:checked').val(),
                           '11-am-4-pm': $('#plan .field-name-field-11-am-4-pm input:checked').val(),
                           '4-pm-9-pm': $('#plan .field-name-field-4-pm-9-pm input:checked').val(),
                           '9-pm-2-am': $('#plan .field-name-field-9-pm-2-am input:checked').val()
                       },
                       success: function (data) {
                           $('#study-preferences').remove();
                       }
                   });
        }
    });

    function updateTabs(data, row)
    {
        // update calendar events
        window.planEvents = data.events;
        calendar.fullCalendar('refetchEvents');

        // update class list below calendar
        jQuery('.schedule .row').remove();
        jQuery(data.schedule).find('.schedule .row')
            .appendTo(jQuery('.schedule'));

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
            jQuery('#dates').removeClass('empty');
        }
        else
        {
            jQuery('#checkin').addClass('empty edit-schedule');
            jQuery('#dates').addClass('empty');
        }

        // fix class list on key dates tab
        jQuery('#dates .row').each(function (i, r) {
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
                if (data.awards[i] != false && jQuery('#awards #' + i).is('.not-awarded')) {
                    jQuery('#awards #' + i).removeClass('not-awarded').addClass('awarded');
                    lastAward = i;
                }
            }

            if (lastAward == 'setup-pulse')
                jQuery('#awards').relocateAward(lastAward, '#plan > .pane-content');
            else if (lastAward != null)
                jQuery('#awards').relocateAward(lastAward, '#awards > .pane-content');
            else
                jQuery('#awards').relocateAward('');
        }

        // scroll back to tab they clicked edit on
    }

    jQuery('body').on('click', 'a[href="#edit-schedule"]', function (evt) {
        evt.preventDefault();
        if(jQuery('#study-preferences').length > 0)
        {
            jQuery('#plan').scrollintoview();
            return;
        }
        jQuery('#plan').removeClass('edit-other-only').addClass('edit-class-only');
        jQuery('#plan #schedule-type').val('c');
        jQuery('#plan #schedule-weekly').prop('checked', true);
        jQuery('#plan .field-add-more-submit').hide();
        jQuery('#plan #add-class-dialog').addClass('edit').scrollintoview({padding: {top:120,bottom:200,left:0,right:0}});
    });

    $('#plan').on('click', 'a[href="#remove-class"]', function (evt) {
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
        $('#plan #schedule-class-name').val('');
        $('#plan #schedule-type').val('');
        $('#plan #add-class-dialog input[name*="schedule-dotw-"]:checked').prop('checked', false);
        $('#plan #schedule-value-date').val('');
        $('#plan #schedule-value-time').val('');
        $('#plan #schedule-value2-date').val('');
        $('#plan #schedule-value2-time').val('');
    }

    $('#plan').on('click', 'a[href="#save-class"]', function (evt) {
        evt.preventDefault();
        var that = $(this),
            row = that.parents('.row');
        if(!row.is('.invalid'))
        {
            var dotw = row.find('.field-name-field-day-of-the-week input[name*="schedule-dotw-"]:checked').map(function (i, x) {return $(x).val();}).get();
            if(row.find('.field-name-field-day-of-the-week input[value="monthly"]:checked').length > 0)
                dotw[dotw.length] = 'Monthly';
            else if(row.find('.field-name-field-day-of-the-week input[value="yearly"]:checked').length > 0)
                dotw[dotw.length] = 'Yearly';
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
                    $('#plan').removeClass('edit-class-only edit-other-only');
                    row.removeClass('edit valid').addClass('invalid');
                    jQuery('#plan .field-add-more-submit').show();

                    updateTabs(data);
                }
            });
        }
    });

    $('#plan').on('keyup', '.field-name-field-class-name input', function () {
        jQuery(this).parents('.row').planFunc();
    });
    $('#plan').on('change', '.field-name-field-class-name input', function () {
        jQuery(this).parents('.row').planFunc();
    });
    $('#plan').on('change', '.field-name-field-day-of-the-week input', function () {
        jQuery(this).parents('.row').planFunc();
    });
    $('#plan').on('change', '.field-name-field-day-of-the-week .form-checkboxes.form-type-checkbox input', function () {
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
    $('#plan').on('keyup', '.field-name-field-time input[type="text"]', function () {
        jQuery(this).parents('.row').planFunc();
    });
    $('#plan').on('change', '.field-name-field-time input[type="text"]', function () {
        jQuery(this).parents('.row').planFunc();
    });
    $('#plan .schedule .row, #add-class-dialog').planFunc();

    var date = new Date();
    var d = date.getDate();
    var m = date.getMonth();
    var y = date.getFullYear();
    var dragSource = [];

    calendar = $('#calendar').fullCalendar({
        titleFormat: 'MMMM',
        editable: true,
        draggable: true,
        allDaySlot: false,
        aspectRatio: 1.9,
        timeslotsPerHour: 4,
        slotMinutes: 15,
        eventRender: function (event, element) {
            element.find('.fc-event-title').html(event.title);
            return true;
        },
        header: {
            left: 'prev,next today title',
            center: '',
            right: ''
        },
        defaultView: 'agendaWeek',
        selectable: false,
        events: function (start, end, callback) {
            callback(window.planEvents.map(function (e) {
                e.start = new Date(e.start);
                e.end = new Date(e.end);
                return e;
            }));
        },
        eventDragStart: function (event, jsEvent, ui, view){
            var prev, next;
            for(var i in window.planEvents)
                if(window.planEvents[i].start < event.start &&
                   window.planEvents[i].className[1] == event.className[0])
                    prev = window.planEvents[i];
                else if(window.planEvents[i].className[1] == event.className[0])
                {
                    next = window.planEvents[i];
                    break;
                }
            /*var dragDropEvent = {
                allDay:false,
                className: ['drag-event'],
                start: prev.end,
                end: next.start,
                editable: false,
                title: 'Drag &amp; Drop',
                _id: 'drag-and-drop'
            };*/
            //dragSource = [dragDropEvent];
            //calendar.fullCalendar( 'renderEvent',  dragDropEvent, true);
            //view.setEventData(events); // for View.js, TODO: unify with renderEvents
            //window.planEvents.push(dragDropEvent);
            //view.renderEvents(window.planEvents, 'drag-and-drop'); // actually render the DOM elements

        },
        eventDragStop: function (event, jsEvent, ui, view) {
        },
        eventDrop: function(event,dayDelta,minuteDelta,allDay,revertFunc) {
            //calendar.fullCalendar( 'removeEventSource',  dragSource);
            var prev, next,
                lastTime = event.start.getTime() - dayDelta * 86400 * 1000 - minuteDelta * 60 * 1000;
            for(var i in window.planEvents)
            {
                if((window.planEvents[i].className[1] == event.className[0] ||
                    window.planEvents[i].className[0] == event.className[0]) &&
                    window.planEvents[i] != event)
                {
                    // TODO: update this if classes are draggable
                    if(window.planEvents[i].start.getTime() < lastTime &&
                        (prev == null || window.planEvents[i].end.getTime() > prev.end.getTime()))
                        prev = window.planEvents[i];

                    if(window.planEvents[i].start.getTime() > lastTime &&
                        (next == null || window.planEvents[i].start.getTime() < next.start.getTime()))
                        next = window.planEvents[i];
                }
            }
            // + dayDelta * 86400 * 1000 + minuteDelta * 60 * 1000
            if(event.start.getTime() < prev.end.getTime() ||
               event.end.getTime() > next.start.getTime())
                revertFunc();
            //alert(
            //    event.title + " was moved " +
            //    dayDelta + " days and " +
            //    minuteDelta + " minutes."
            //);

            //if (allDay) {
            //alert("Event is now all-day");
            //}else{
            //alert("Event has a time-of-day");
            //}

            //if (!confirm("Are you sure about this change?")) {
            //revertFunc();
            //}
            $.ajax({
                url: '/node/save/schedule',
                type: 'POST',
                data: {
                    className: event['title'],
                    start: event['start'].toJSON(),
                    end: event['end'].toJSON(),
                    dotw: '',
                    type: event['title'].substring(0, 3) == '(P)' ? 'p' : (event['title'].substring(0, 4) == '(SR)' ? 'sr' : (event['title'].substring(0, 3) == '(F)' ? 'f' : ''))
                },
                error: revertFunc
            });
        }
    });

    calendar.find('.fc-agenda-slots tr:nth-child(4n-3) .fc-agenda-axis').eq(new Date().getHours()).scrollintoview();

});

