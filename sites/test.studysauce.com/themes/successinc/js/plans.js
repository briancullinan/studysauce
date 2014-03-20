var calendar = null;

jQuery(document).ready(function($) {

    var planFunc = function () {
        var row = $('#plan .edit.row');
        if($('#plan #schedule-class-name').val().trim() == '' ||
            $('#plan input[name*="schedule-dotw-"]:checked, #plan #schedule-monthly:checked, #plan #schedule-yearly:checked').length == 0 ||
            $('#plan #schedule-value-date').val().trim() == '' ||
            $('#plan #schedule-value-time').val().trim() == '' ||
            $('#plan #schedule-value2-date').val().trim() == '' ||
            $('#plan #schedule-value2-time').val().trim() == '')
            row.removeClass('valid').addClass('invalid');
        else
            row.removeClass('invalid').addClass('valid');
    };

    $('#plan a[href="#save-schedule"]').click(function (evt) {
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

    $('#plan a[href="#save-class"]').click(function (evt) {
        evt.preventDefault();
        if(!$(this).parents('.row').is('.invalid'))
        {
            var dotw = $('#plan input[name*="schedule-dotw-"]:checked').map(function (i, x) {return $(x).val();}).get();
            if($('#plan #schedule-monthly:checked'))
                dotw[dotw.length] = 'Monthly';
            else
                dotw[dotw.length] = 'Yearly';
            $.ajax({
                url: '/node/save/schedule',
                type: 'POST',
                dataType: 'json',
                data: {
                    className: $('#plan #schedule-class-name').val(),
                    dotw: dotw.join(','),
                    start: $('#plan #schedule-value-date').val() + ' ' + $('#plan #schedule-value-time').val(),
                    end: $('#plan #schedule-value2-date').val() + ' ' + $('#plan #schedule-value2-time').val(),
                    type: $('#plan #schedule-type').val()
                },
                success: function (data) {
                    $('#plan #schedule-class-name').val('');
                    $('#plan #schedule-type').val('');
                    $('#plan input[name*="schedule-dotw-"]:checked').prop('checked', false);
                    $('#plan #schedule-value-date').val('');
                    $('#plan #schedule-value-time').val('');
                    $('#plan #schedule-value2-date').val('');
                    $('#plan #schedule-value2-time').val('');
                    $('#plan').removeClass('edit-only');
                    $(this).parents('.row').removeClass('.valid').addClass('.invalid');
                    window.planEvents = data.events;
                    calendar.fullCalendar('refetchEvents');
                }
            });
        }
    });
    $('#plan #schedule-class-name').keyup(planFunc);
    $('#plan #schedule-class-name').change(planFunc);
    $('#plan input[name*="schedule-dotw-"], #plan #schedule-monthly, #plan #schedule-yearly').change(planFunc);
    $('#plan #schedule-value-date').keyup(planFunc);
    $('#plan #schedule-value-time').keyup(planFunc);
    $('#plan #schedule-value2-date').keyup(planFunc);
    $('#plan #schedule-value2-time').keyup(planFunc);
    $('#plan #schedule-value-date').change(planFunc);
    $('#plan #schedule-value-time').change(planFunc);
    $('#plan #schedule-value2-date').change(planFunc);
    $('#plan #schedule-value2-time').change(planFunc);
    planFunc();

    $('#plan #schedule-value-date, #plan #schedule-value2-date')
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
    $('#plan #schedule-value-time, #plan #schedule-value2-time')
        .timeEntry({
                            ampmNames: ['am', 'pm'],
                            fromTo: false,
                            show24Hours: false,
                            showSeconds: false,
                            spinnerImage: '',
                            timeSteps: [1,1,"1"]
                        });

    var date = new Date();
    var d = date.getDate();
    var m = date.getMonth();
    var y = date.getFullYear();

    calendar = $('#calendar').fullCalendar({
        editable: true,
        draggable: true,
        allDaySlot: false,
        aspectRatio: 1.9,
        timeslotsPerHour: 1,
        header: {
            left: 'prev,next today',
            center: 'title',
            right: 'prev,next today'
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
        eventDrop: function(event,dayDelta,minuteDelta,allDay,revertFunc) {

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

});

