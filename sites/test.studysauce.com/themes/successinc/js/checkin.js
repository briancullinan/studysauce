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
                        date: new Date().toJSON(),
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
                            jQuery('#checkin,#plan').find('#checklist,#sds-messages').removeClass('invalid').dialog('hide');
                            jQuery('#checkin .clock').startClock();
                            //jQuery('#checkin .flip-counter').scrollintoview({padding: {top: 120, bottom: 200, left: 0, right: 0}});
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

                        // update metrics key
                        var mc = 0;
                        jQuery('#metrics ol li').remove();
                        window.classNames = [];
                        for(var eid in data.classes)
                        {
                            window.classNames[window.classNames.length] = data.classes[eid];
                            jQuery('#metrics ol').append('<li><span class="class' + mc + '">&nbsp;</span>' + data.classes[eid] + '</li>');
                            mc++;
                        }

                        // update metrics graphs
                        jQuery('#metrics').updateMetrics(data.times);
                        if (data.times.length == 0)
                        {
                            jQuery('#metrics').addClass('empty');
                            jQuery('#metrics-empty').dialog();
                        }
                        else
                        {
                            jQuery('#metrics').removeClass('empty');
                            jQuery('#metrics-empty').dialog('hide');
                        }
                        jQuery('#study-total').text(data.total);
                        jQuery('#timeline > h4').html(data.hours > 0 ? ('Goal: ' + data.hours + ' hours') : '&nbsp;');

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

    var sessionBegin = function (evt) {
        evt.preventDefault();

        // the default for timer expire is to go to metrics tab
        panel.find('#timer-expire').off()
            .on('click', 'a[href="#close"]', function (evt) {
                evt.preventDefault();
                var panel = jQuery(this).parents('#checkin,#plan');
                panel.find('#timer-expire').dialog('hide');
                if(panel.is('#checkin'))
                    window.location = '#metrics';
            });

        if (jQuery('#sds-messages .show').length > 0)
        {
            panel.find('#sds-messages').dialog();
        }
        else
        {
            panel.find('#checklist').dialog();
        }

        jQuery('#checklist a[href="#study"], #sds-messages a[href="#study"]').off('click');
        jQuery('#checklist a[href="#study"], #sds-messages a[href="#study"]').on('click', function (evt) {
            evt.preventDefault();
            var dialog = jQuery(this).parents('.dialog');
            if(dialog.is('.invalid'))
                return;
            dialog.addClass('invalid');
            jQuery('.minplayer-default-play').trigger('click');
            //if(typeof navigator.geolocation != 'undefined')
            //{
            //    locationTimeout = setTimeout(callback, 2000);
            //    navigator.geolocation.getCurrentPosition(callback, callback, {maximumAge: 3600000, timeout:1000});
            //}
            //else
            checkinCallback(null, id, false);
            that.scrollintoview({padding: {top: 120, bottom: 200, left: 0, right: 0}});
        });
    };

    // move parent fixed-centered
    panel.find('.pane-content').append(jQuery('#checklist, #sds-messages, #timer-expire').parents('.fixed-centered').detach());
    // if it is in session always display timer expire
    if (that.is('.checked-in'))
    {
        if(clock != null)
            clearInterval(clock);
        clock = null;
        sessionStart = new Date().getTime() / 1000;
        setClock();
        panel.find('#timer-expire').dialog();
        checkinCallback(null, id, true);
    }
    else if (jQuery('#checkin .classes a.checked-in, #plan .mini-checkin > a.checked-in').length > 0)
    {
        if(clock != null)
            clearInterval(clock);
        clock = null;
        sessionStart = new Date().getTime() / 1000;
        setClock();

        // switch off other checkin buttons
        var tmpThat = jQuery('#checkin .classes a.checked-in, #plan .mini-checkin > a.checked-in').first(),
            tmpId = (/cid([0-9]+)(\s|$)/ig).exec(tmpThat.parents('#checkin, #plan').is('#plan')
                ? tmpThat.parents('.row').attr('class')
                : tmpThat.attr('class'))[1];
        checkinCallback(null, tmpId, true);

        // show expire message
        panel.find('#timer-expire').off()
            .on('click', 'a[href="#close"]', sessionBegin).dialog();
    }
    else
        sessionBegin(evt);
}

jQuery(document).ready(function ($) {

    setInterval(function () {
        if(typeof window.minplayer != 'undefined')
        {
            var plugins = window.minplayer.plugins;
            for(var p in plugins)
            {
                if(typeof plugins[p].media != 'undefined')
                {
                    for(var i in plugins[p].media)
                    {
                        plugins[p].media[i].hasFocus = false;
                    }
                }
            }
        }
    }, 2000);

    if(typeof minplayer != 'undefined')
    {
        minplayer.get(function (plugin) {
            if(plugin.name == 'player' && typeof plugin.media != 'undefined' &&
                typeof plugin.media.player != 'undefined')
                plugin.media.player.addEventListener('playing', function() {
                    plugin.media.hasFocus = false;
                });
        });
    }

    // perform ajax call when clicked
    jQuery('#checkin').on('click', '.classes a', checkinClick);
    jQuery('#plan').on('click', '.mini-checkin > a', checkinClick);

    jQuery('#checkin').on('click', 'a.minplayer-default-play, a.minplayer-default-pause', function () {
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
        clock = setInterval(function () {
            setClock();
            if (new Date().getTime() / 1000 - sessionStart >= TIMER_SECONDS - 59) {
                clearInterval(clock);
                clock = null;
                sessionStart = new Date().getTime() / 1000;
                setClock();
                // show expire message
                jQuery('.minplayer-default-pause').trigger('click');
                jQuery('#checkin .classes a.checked-in, #plan .mini-checkin > a.checked-in').first().trigger('click');
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


