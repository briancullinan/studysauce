var hours = 0,
    minutes = 0,
    TIMER_SECONDS = 3600,
    seconds = TIMER_SECONDS,
    clock = null;

function setClock() {
    var tmpHours = '' + Math.floor(seconds / 60 / 60);
    var tmpMinutes = '' + Math.floor(seconds / 60 % 60);
    if (tmpHours == hours && tmpMinutes == minutes)
        return;
    hours = tmpHours;
    minutes = tmpMinutes;
    if (hours.length == 1) {
        jQuery('#checkin .clock ul').first().find('li')
            .removeClass('flip-clock-before')
            .removeClass('flip-clock-active');

        jQuery('#checkin .clock ul').eq(1).find('li')
            .removeClass('flip-clock-before')
            .removeClass('flip-clock-active')
            .eq(parseInt(hours)).addClass('flip-clock-active');
    }
    else {
        jQuery('#checkin .clock ul').first().find('li')
            .removeClass('flip-clock-before')
            .removeClass('flip-clock-active')
            .eq(parseInt(hours.substring(0, 1))).addClass('flip-clock-active');

        jQuery('#checkin .clock ul').eq(1).find('li')
            .removeClass('flip-clock-before')
            .removeClass('flip-clock-active')
            .eq(parseInt(hours.substring(1))).addClass('flip-clock-active');
    }

    if (minutes.length == 1) {
        jQuery('#checkin .clock ul').eq(2).find('li')
            .removeClass('flip-clock-before')
            .removeClass('flip-clock-active');

        jQuery('#checkin .clock ul').eq(3).find('li')
            .removeClass('flip-clock-before')
            .removeClass('flip-clock-active')
            .eq(parseInt(minutes)).addClass('flip-clock-active');
    }
    else {
        jQuery('#checkin .clock ul').eq(2).find('li')
            .removeClass('flip-clock-before')
            .removeClass('flip-clock-active')
            .eq(parseInt(minutes.substring(0, 1))).addClass('flip-clock-active');

        jQuery('#checkin .clock ul').eq(3).find('li')
            .removeClass('flip-clock-before')
            .removeClass('flip-clock-active')
            .eq(parseInt(minutes.substring(1))).addClass('flip-clock-active');
    }
}

function checkinCallback(pos, eid, checkedIn) {
    clearTimeout(locationTimeout);
    var checked = [],
        lat = pos != null && typeof pos.coords != 'undefined' ? pos.coords.latitude : '',
        lng = pos != null && typeof pos.coords != 'undefined' ? pos.coords.longitude : '';
    jQuery('#checklist input:checked').each(function () { checked[checked.length] = jQuery(this).attr('name'); });
    jQuery.ajax({
                    url: '/checkin',
                    type: 'POST',
                    dataType: 'json',
                    data: {
                        checkedIn: checkedIn,
                        date: new Date().getTime() / 1000,
                        eid: eid,
                        checklist: checked.join(','),
                        location: lat + ',' + lng
                    },
                    success: function (data) {
                        var that = jQuery('#checkin #class' + data.eid);

                        // update clock
                        if (checkedIn) {
                            jQuery('#checkin .clock').stopClock();
                        }
                        else {
                            jQuery('#checkin').removeClass('checklist-only sds-message-only');
                            jQuery('#checkin .clock').startClock();
                            jQuery('#checkin .flip-counter').scrollintoview({padding: {top: 120, bottom: 200, left: 0, right: 0}});
                        }

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

                        // update awards
                        if (typeof data.awards != 'undefined') {
                            var lastAward = null;
                            for (var i in data.awards) {
                                if (data.awards[i] != false && jQuery('#awards #' + i).is('.not-awarded')) {
                                    jQuery('#awards #' + i).removeClass('not-awarded').addClass('awarded');
                                    lastAward = i;
                                }
                            }

                            if (lastAward == 'beginner-checkin' || lastAward == 'beginner-checklist' || lastAward == 'beginner-mix' ||
                                lastAward == 'beginner-breaks' || lastAward == 'beginner-cram' || lastAward == 'beginner-chicken' ||
                                lastAward == 'beginner-commuter' || lastAward == 'intermediate-flier' || lastAward == 'intermediate-cross' ||
                                lastAward == 'intermediate-breaker' || lastAward == 'intermediate-cured' || lastAward == 'beginner-checklist' ||
                                lastAward == 'advanced-magellan' || lastAward == 'advanced-bo' || lastAward == 'advanced-comber' ||
                                lastAward == 'advanced-nocram')
                                jQuery('#awards').relocateAward(lastAward, '#checkin > .pane-content');
                            else if (lastAward != null)
                                jQuery('#awards').relocateAward(lastAward, '#awards > .pane-content');
                            else
                                jQuery('#awards').relocateAward('');
                        }
                    }
                });
}

jQuery(document).ready(function ($) {

    // perform ajax call when clicked
    jQuery('#checkin').on('click', '.classes a', function (evt) {
        evt.preventDefault();
        var that = jQuery(this);

        jQuery('#checklist .no-checkboxes').hide();
        jQuery('#checklist .checkboxes').show();
        jQuery('#checklist .checkboxes input').removeAttr('checked');
        if (seconds > 59 && seconds <= TIMER_SECONDS &&
            clock != null) {
            clearInterval(clock);
            clock = null;
            seconds = TIMER_SECONDS;
            setClock();
            // show expire message
            jQuery('#checkin').addClass('timer-expire-only').scrollintoview();
        }
        if (that.is('.checked-in')) {
            checkinCallback(null, that.attr('id').substring(5), true);
            that.removeClass('checked-in');
            return;
        }
        else if (jQuery('#sds-messages .show').length > 0) {
            jQuery('#checkin').addClass('sds-message-only').scrollintoview();
        }
        else {
            jQuery('#checkin').addClass('checklist-only').scrollintoview();
        }
        that.addClass('checked-in');

        jQuery('#checkin a[href="#study"]').unbind();
        jQuery('#checkin a[href="#study"]').click(function (evt) {
            evt.preventDefault();
            jQuery('.minplayer-default-play').trigger('click');
            //if(typeof navigator.geolocation != 'undefined')
            //{
            //    locationTimeout = setTimeout(callback, 2000);
            //    navigator.geolocation.getCurrentPosition(callback, callback, {maximumAge: 3600000, timeout:1000});
            //}
            //else
            checkinCallback(null, that.attr('id').substring(5), false);
        });
    });

    jQuery('#checkin').on('click', 'a[href="#break"]', function (evt) {
        evt.preventDefault();
        jQuery('#checkin').removeClass('timer-expire-only');
        jQuery('#checkin .classes a.checked-in').trigger('click');
        if (!jQuery('#checkin').is('.checklist-only') && !jQuery('#checkin').is('.sds-message-only'))
            jQuery('#metrics').scrollintoview({padding: {top: 120, bottom: 200, left: 0, right: 0}});
    });

    jQuery('#checkin').on('click', '#timer-expire a[href="#awards"]', function (evt) {
        jQuery('#checkin .classes a.checked-in').trigger('click');
        jQuery('#checkin').removeClass('timer-expire-only').scrollintoview();
    });

    setClock();
    jQuery('#checkin .clock').find('a').click(function (evt) { evt.preventDefault(); });
    $.fn.startClock = function () {
        if (clock != null) {
            clearInterval(clock);
            clock = null;
        }
        seconds = TIMER_SECONDS;
        setClock();
        jQuery('.minplayer-default-play').trigger('click');
        clock = setInterval(function () {
            seconds--;
            setClock();
            if (seconds == 59) {
                if (clock != null) {
                    clearInterval(clock);
                    clock = null;
                }
                seconds = TIMER_SECONDS;
                setClock();
                // show expire message
                jQuery('.minplayer-default-pause').trigger('click');
                jQuery('#checkin').addClass('timer-expire-only').scrollintoview();
            }
        }, 1000);
    };
    $.fn.stopClock = function () {
        if (clock != null) {
            clearInterval(clock);
            clock = null;
        }
        jQuery('.minplayer-default-pause').trigger('click');
        seconds = TIMER_SECONDS;
        setClock();
    };

    jQuery(window).unload(function () {
        jQuery('#checkin .classes a.checked-in').trigger('click');
        jQuery('#checkin').scrollintoview();
        isClosing = true;
    });

});


