var hours = 0,
    minutes = 0,
    TIMER_SECONDS = 3600,
    sessionStart = null,
    clock = null;

function setClock() {
    var clocks = jQuery('#checkin .clock');
    var seconds = new Date().getTime() / 1000 - sessionStart + 59;
    var tmpHours = '' + Math.floor(seconds / 60 / 60);
    var tmpMinutes = '' + Math.floor(seconds / 60 % 60);
    if (tmpHours == hours && tmpMinutes == minutes)
        return;
    hours = tmpHours;
    minutes = tmpMinutes;
    if(hours == 0 && minutes == 0)
        clocks = clocks.add(jQuery('#plan .clock'));
    else
        clocks = clocks.add(jQuery('#plan .mini-checkin > a.checked-in').parent().find('.clock'));
    clocks.each(function () {
        var that = jQuery(this);
        if (hours.length == 1) {
            that.find('ul:first-of-type').find('li')
                .removeClass('flip-clock-before')
                .removeClass('flip-clock-active');

            that.find('ul:nth-of-type(2)').find('li')
                .removeClass('flip-clock-before')
                .removeClass('flip-clock-active')
                .eq(parseInt(hours)).addClass('flip-clock-active');
        }
        else {
            that.find('ul:first-of-type').find('li')
                .removeClass('flip-clock-before')
                .removeClass('flip-clock-active')
                .eq(parseInt(hours.substring(0, 1))).addClass('flip-clock-active');

            that.find('ul:nth-of-type(2)').find('li')
                .removeClass('flip-clock-before')
                .removeClass('flip-clock-active')
                .eq(parseInt(hours.substring(1))).addClass('flip-clock-active');
        }

        if (minutes.length == 1) {
            that.find('ul:nth-of-type(3)').find('li')
                .removeClass('flip-clock-before')
                .removeClass('flip-clock-active');

            that.find('ul:nth-of-type(4)').find('li')
                .removeClass('flip-clock-before')
                .removeClass('flip-clock-active')
                .eq(parseInt(minutes)).addClass('flip-clock-active');
        }
        else {
            that.find('ul:nth-of-type(3)').find('li')
                .removeClass('flip-clock-before')
                .removeClass('flip-clock-active')
                .eq(parseInt(minutes.substring(0, 1))).addClass('flip-clock-active');

            that.find('ul:nth-of-type(4)').find('li')
                .removeClass('flip-clock-before')
                .removeClass('flip-clock-active')
                .eq(parseInt(minutes.substring(1))).addClass('flip-clock-active');
        }
    });

}

function checkinCallback(pos, eid, checkedIn) {
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
                        var that = jQuery('#plan .cid' + data.eid + ' .mini-checkin .class, #checkin .classes .cid' + data.eid);

                        // update clock
                        if (checkedIn) {
                            jQuery('#checkin .clock').stopClock();
                            that.removeClass('checked-in');
                        }
                        else {
                            that.addClass('checked-in');
                            jQuery('#checkin,#plan').removeClass('checklist-only sds-message-only');
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
                        if(data.rows.length > 1)
                            jQuery('#home-tips').attr('checked', 'checked');
                        if(jQuery('#home').find('input[type="checkbox"]:checked').length == jQuery('#home').find('input[type="checkbox"]').length - 1)
                            jQuery('#home-tasks-checklist').attr('checked', 'checked');
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
                                if (data.awards[i] != false && jQuery('#badges #' + i).is('.not-awarded')) {
                                    jQuery('#badges #' + i).removeClass('not-awarded').addClass('awarded');
                                    lastAward = i;
                                }
                            }

                            if (lastAward == 'beginner-checkin' || lastAward == 'beginner-checklist' || lastAward == 'beginner-mix' ||
                                lastAward == 'beginner-breaks' || lastAward == 'beginner-cram' || lastAward == 'beginner-chicken' ||
                                lastAward == 'beginner-commuter' || lastAward == 'intermediate-flier' || lastAward == 'intermediate-cross' ||
                                lastAward == 'intermediate-breaker' || lastAward == 'intermediate-cured' || lastAward == 'beginner-checklist' ||
                                lastAward == 'advanced-magellan' || lastAward == 'advanced-bo' || lastAward == 'advanced-comber' ||
                                lastAward == 'advanced-nocram')
                                jQuery('#badges').relocateAward(lastAward, '#metrics > .pane-content');
                            else if (lastAward != null)
                                jQuery('#badges').relocateAward(lastAward, '#badges > .pane-content');
                        }
                    }
                });
}

function checkinClick(evt)
{
    evt.preventDefault();
    var that = jQuery(this),
        panel = that.parents('#checkin, #plan'),
        id = (/cid([0-9]+)(\s|$)/ig).exec(panel.is('#plan') ? that.parents('.row').attr('class') : that.attr('class'))[1];

    jQuery('#checklist .no-checkboxes').hide();
    jQuery('#checklist .checkboxes').show();
    jQuery('#checklist .checkboxes input').removeAttr('checked');
    panel.find('.pane-content').append(jQuery('#checklist, #sds-messages, #timer-expire').detach());
    // if it is in session always display timer expire
    if (jQuery('#checkin .classes a.checked-in, #plan .mini-checkin > a.checked-in').length > 0 && clock != null)
    {
        clearInterval(clock);
        clock = null;
        sessionStart = new Date().getTime() / 1000;
        setClock();
        // show expire message
        panel.addClass('timer-expire-only').scrollintoview();
    }
    if (that.is('.checked-in'))
    {
        checkinCallback(null, id, true);
        return;
    }
    else if (jQuery('#sds-messages .show').length > 0)
    {
        panel.addClass('sds-message-only').scrollintoview();
    }
    else
    {
        panel.addClass('checklist-only').scrollintoview();
    }

    jQuery('#checklist a[href="#study"], #sds-messages a[href="#study"]').off('click');
    jQuery('#checklist a[href="#study"], #sds-messages a[href="#study"]').on('click', function (evt) {
        evt.preventDefault();
        jQuery('.minplayer-default-play').trigger('click');
        //if(typeof navigator.geolocation != 'undefined')
        //{
        //    locationTimeout = setTimeout(callback, 2000);
        //    navigator.geolocation.getCurrentPosition(callback, callback, {maximumAge: 3600000, timeout:1000});
        //}
        //else
        checkinCallback(null, id, false);
    });
}

jQuery(document).ready(function ($) {

    if(typeof minplayer != 'undefined')
    {
        minplayer.get(function (plugin) {
            if(plugin.name == 'player')
                plugin.media.player.addEventListener('playing', function() {
                    plugin.media.hasFocus = false;
                });
        });
    }

    // perform ajax call when clicked
    jQuery('#checkin').on('click', '.classes a', checkinClick);
    jQuery('#plan').on('click', '.mini-checkin > a', checkinClick);

    jQuery('#checkin,#plan').on('click', 'a[href="#break"]', function (evt) {
        evt.preventDefault();
        var panel = jQuery(this).parents('#checkin,#plan');
        panel.removeClass('timer-expire-only');
        if (!jQuery('#checkin').is('.checklist-only') && !jQuery('#checkin').is('.sds-message-only') &&
            !jQuery('#plan').is('.checklist-only') && !jQuery('#plan').is('.sds-message-only') &&
            !panel.is('#plan'))
            window.location = '#metrics';
    });

    jQuery('#checkin').on('click', '#timer-expire a[href="#badges"]', function (evt) {
        jQuery('#checkin .classes a.checked-in').first().trigger('click');
        jQuery('#checkin').removeClass('timer-expire-only').scrollintoview();
    });

    jQuery('#checkin').on('click', 'a[title="Play"], a[title="Pause"]', function () {
        jQuery('.header-wrapper a[title="' + jQuery(this).attr('title') + '"]').trigger('click');
        jQuery('#checkin').find('a[title="Play"], a[title="Pause"]').hide().not('a[title="' + jQuery(this).attr('title') + '"]').css('display', 'block');
        if(jQuery('#checkin').find('input[name="touchedMusic"]').val() == 0)
            jQuery.ajax({
                            url: '/checkin',
                            type: 'POST',
                            dataType: 'json',
                            data: {
                                touchedMusic: true
                            }
                        });
        jQuery('#home-music').attr('checked', 'checked');
        if(jQuery('#home').find('input[type="checkbox"]:checked').length == jQuery('#home').find('input[type="checkbox"]').length - 1)
            jQuery('#home-tasks-checklist').attr('checked', 'checked');
    });

    sessionStart = new Date().getTime() / 1000;
    setClock();
    jQuery('#checkin .clock, #plan .clock').find('a').click(function (evt) { evt.preventDefault(); });
    $.fn.startClock = function () {
        if (clock != null) {
            clearInterval(clock);
            clock = null;
        }
        sessionStart = new Date().getTime() / 1000;
        setClock();
        jQuery('.minplayer-default-play').trigger('click');
        clock = setInterval(function () {
            setClock();
            if (new Date().getTime() / 1000 - sessionStart >= TIMER_SECONDS - 59) {
                clearInterval(clock);
                clock = null;
                sessionStart = new Date().getTime() / 1000;
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
        sessionStart = new Date().getTime() / 1000;
        setClock();
    };

    jQuery(window).unload(function () {
        jQuery('#checkin .classes a.checked-in, #plan .mini-checkin > a.checked-in').first().trigger('click');
        isClosing = true;
    });

});


