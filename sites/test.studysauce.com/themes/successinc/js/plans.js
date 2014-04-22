var calendar = null;

jQuery(document).ready(function ($) {

    $.propHooks.checked = {
        set: function (elem, value, name) {
            var ret = (elem[ name ] = value);
            $(elem).trigger("change");
            return ret;
        }
    };

    jQuery('#plan').on('change', '#schedule-historic', function () {
        if(jQuery(this).prop('checked'))
            jQuery('#plan .row.hide, #plan .head.hide').show();
        else
            jQuery('#plan .row.hide, #plan .head.hide').hide();
    });

    jQuery('#plan').on('change', '.page-dashboard #plan .field-name-field-completed input', function () {
        var that = jQuery(this),
            row = that.parents('.row');
        if(that.is(':checked'))
            row.addClass('done');
        else
            row.removeClass('done');
    });

    jQuery('#plan').on('change', 'select[name="strategy-select"]', function () {
        var that = jQuery(this),
            row = that.parents('.row');
        if(that.val() == 'teach')
        {
            row.find('.strategy-teach').show();
            row.find('.strategy-spaced').hide();
            row.find('.strategy-active').hide();
        }
        if(that.val() == 'spaced')
        {
            row.find('.strategy-teach').hide();
            row.find('.strategy-spaced').show();
            row.find('.strategy-active').hide();
        }
        if(that.val() == 'active')
        {
            row.find('.strategy-teach').hide();
            row.find('.strategy-spaced').hide();
            row.find('.strategy-active').show();
        }
    });

    jQuery('#plan').on('click', '.page-dashboard #plan .field-name-field-assignment,' +
                                '.page-dashboard #plan .field-name-field-class-name,' +
                                '.page-dashboard #plan .field-name-field-percent', function () {
        var row = $(this).parents('.row');
        row.toggleClass('selected');
        row.find('.field-select-strategy select').trigger('change');
        if(!row.is('.selected'))
            row.find('.strategy-teach, .strategy-spaced, .strategy-active').hide();
        else
            row.find('.strategy-teach:visible, .strategy-spaced:visible, .strategy-active:visible').first().scrollintoview({padding: {top:120,bottom:100,left:0,right:0}});
    });


    var date = new Date();
    var isInitialized = false,
        initialize = function () {
            if (!isInitialized)
            {
                calendar = $('#calendar').fullCalendar(
                    {
                        titleFormat: 'MMMM',
                        editable: true,
                        draggable: true,
                        aspectRatio: 1.9,
                        height:500,
                        timeslotsPerHour: 4,
                        slotMinutes: 15,
                        firstHour: new Date().getHours(),
                        eventRender: function (event, element) {
                            element.find('.fc-event-title').html(event.title);
                            return true;
                        },
                        header: {
                            left: '',
                            center: '',
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
                        eventDragStart: function (event, jsEvent, ui, view) {
                            var prev, next;
                            for (var i in window.planEvents)
                                if (window.planEvents[i].start < event.start &&
                                    window.planEvents[i].className[1] == event.className[0])
                                    prev = window.planEvents[i];
                                else if (window.planEvents[i].className[1] == event.className[0]) {
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
                        eventDrop: function (event, dayDelta, minuteDelta, allDay, revertFunc) {
                            //calendar.fullCalendar( 'removeEventSource',  dragSource);
                            var prev, next,
                                lastTime = event.start.getTime() - dayDelta * 86400 * 1000 - minuteDelta * 60 * 1000;
                            for (var i in window.planEvents) {
                                if ((window.planEvents[i].className[1] == event.className[0] ||
                                     window.planEvents[i].className[0] == event.className[0]) &&
                                    window.planEvents[i] != event) {
                                    // TODO: update this if classes are draggable
                                    if (window.planEvents[i].start.getTime() < lastTime &&
                                        (prev == null || window.planEvents[i].end.getTime() > prev.end.getTime()))
                                        prev = window.planEvents[i];

                                    if (window.planEvents[i].start.getTime() > lastTime &&
                                        (next == null || window.planEvents[i].start.getTime() < next.start.getTime()))
                                        next = window.planEvents[i];
                                }
                            }
                            // + dayDelta * 86400 * 1000 + minuteDelta * 60 * 1000
                            if (event.start.getTime() < prev.end.getTime() ||
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
                                           type: event['title'].substring(0, 3) == '(P)'
                                               ? 'p'
                                               : (event['title'].substring(0, 4) == '(SR)'
                                               ? 'sr'
                                               : (event['title'].substring(0, 3) == '(F)'
                                               ? 'f'
                                               : ''))
                                       },
                                       error: revertFunc
                                   });
                        }
                    });

                //calendar.find('.fc-agenda-slots tr:nth-child(4n-3) .fc-agenda-axis').eq(new Date().getHours()).scrollintoview({padding: {top:120,bottom:500,left:0,right:0}});

                isInitialized = true;
            }
        },
        d = date.getDate(),
        m = date.getMonth(),
        y = date.getFullYear(),
        dragSource = [];

    // The calendar needs to be in view for sizing information.  This will not initialize when display:none;, so instead
    //   we will activate the calendar only once, when the menu is clicked, this assumes #hash detection works, and
    //   it triggers the menu clicking
    if(jQuery('#plan:visible').length > 0)
        setTimeout(initialize, 100);
    else
        jQuery('body').on('click', 'a[href="#plan"]', initialize);

});

