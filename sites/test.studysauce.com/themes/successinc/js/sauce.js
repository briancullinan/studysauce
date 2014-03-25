var TIMER_SECONDS = 3600,
    seconds = TIMER_SECONDS,
    clock = null,
    className = '',
    checkedIn = false,
    footerLinks = ['terms', 'privacy', 'about-us', 'contact', 'refund'],
    footerOnly = footerLinks.join('-only ') + '-only' + ' user-profile-only',
    selectedAward = null,
    lastRow = -1,
    locationTimeout,
    lastScheduleEdit = null,
    isClosing = false;;


(function($) {
    var hours = 0,
        minutes = 0;
    function setClock() {
        var tmpHours = '' + Math.floor(seconds / 60 / 60);
        var tmpMinutes = '' + Math.floor(seconds / 60 % 60);
        if(tmpHours == hours && tmpMinutes == minutes)
            return;
        hours = tmpHours;
        minutes = tmpMinutes;
        if(hours.length == 1)
        {
            jQuery('#checkin .clock ul').first().find('li')
                .removeClass('flip-clock-before')
                .removeClass('flip-clock-active');

            jQuery('#checkin .clock ul').eq(1).find('li')
                .removeClass('flip-clock-before')
                .removeClass('flip-clock-active')
                .eq(parseInt(hours)).addClass('flip-clock-active');
        }
        else
        {
            jQuery('#checkin .clock ul').first().find('li')
                .removeClass('flip-clock-before')
                .removeClass('flip-clock-active')
                .eq(parseInt(hours.substring(0, 1))).addClass('flip-clock-active');

            jQuery('#checkin .clock ul').eq(1).find('li')
                .removeClass('flip-clock-before')
                .removeClass('flip-clock-active')
                .eq(parseInt(hours.substring(1))).addClass('flip-clock-active');
        }

        if(minutes.length == 1)
        {
            jQuery('#checkin .clock ul').eq(2).find('li')
                .removeClass('flip-clock-before')
                .removeClass('flip-clock-active');

            jQuery('#checkin .clock ul').eq(3).find('li')
                .removeClass('flip-clock-before')
                .removeClass('flip-clock-active')
                .eq(parseInt(minutes)).addClass('flip-clock-active');
        }
        else
        {
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

    function bubbleResize(evt) {
        var that = $(this);
        var bubble = that.nextUntil('a');
        var thisArrow = bubble.find('.awardArrow');

        var thisPosition = that.offset();
        var thisAlignX = thisPosition.left - $('#awards .awards').offset().left + (that.outerWidth()/2) - (thisArrow.width() / 2);
        thisArrow.css('margin-left', thisAlignX);
    }

    function setBindings()
    {
        jQuery('body').on('click', 'a[href="#invite"]', function (evt) {
            evt.preventDefault();
            jQuery('#incentives').addClass('invite-only').scrollintoview();
        });

        jQuery('body').on('click', 'a[href="#edit-schedule"]', function (evt) {
            evt.preventDefault();
            lastScheduleEdit = jQuery(this).parents('.panel-pane').attr('id');
            if(jQuery('#dates #schedule').length > 0)
                jQuery('#dates').addClass('edit-schedule').scrollintoview();
            else
            {
                jQuery('#plan').removeClass('edit-other-only').addClass('edit-class-only').scrollintoview();
                jQuery('#plan #schedule-type').val('c');
                jQuery('#plan #schedule-weekly').prop('checked', true);
            }
        });

        jQuery('body').on('click', 'a[href="#study-quiz"]', function (evt) {
            evt.preventDefault();
            jQuery('#tips').addClass('study-quiz-only').scrollintoview();
        });

        jQuery('#study-quiz').on('click', 'a[href="#retry"]', function (evt) {
            evt.preventDefault();
            jQuery('#study-quiz #webform-ajax-wrapper-17').load('/quiz #node-17 #webform-ajax-wrapper-17 > *', function () {
                Drupal.attachBehaviors();
                jQuery('#tips').scrollintoview();
                //Drupal.ajax['edit-webform-ajax-submit-17'] = new Drupal.ajax('edit-webform-ajax-submit-17', jQuery('#study-quiz input#edit-webform-ajax-submit-17'), {"callback":"webform_ajax_callback","wrapper":"webform-ajax-wrapper-17","progress":{"message":"","type":"throbber"},"event":"click","url":"\/system\/ajax","submit":{"_triggering_element_name":"op","_triggering_element_value":"Submit"}});
            });
        });

        // perform ajax call when clicked
        jQuery('#checkin').on('click', '.classes a', function (evt) {
            evt.preventDefault();
            var that = jQuery(this);
            jQuery('#checklist .no-checkboxes').hide();
            jQuery('#checklist .checkboxes').show();
            jQuery('#checklist .checkboxes input').removeAttr('checked');
            if(seconds > 59 && seconds <= TIMER_SECONDS &&
               clock != null)
            {
                clearInterval(clock);
                clock = null;
                seconds = TIMER_SECONDS;
                setClock();
                // show expire message
                jQuery('#checkin').addClass('timer-expire-only').scrollintoview();
            }
            if(that.is('.checked-in'))
            {
                window.checkedIn = true;
                window.className = that.text().substring(1);
                Drupal.ajax['classes'].eventResponse(this, evt);
            }
            else if(jQuery('#sds-messages .show').length > 0)
            {
                jQuery('#checkin').find('a[href="#study"]').data('className', that.text().substring(1));
                jQuery('#checkin').addClass('sds-message-only').scrollintoview();
            }
            else
            {
                jQuery('#checkin').find('a[href="#study"]').data('className', that.text().substring(1));
                jQuery('#checkin').addClass('checklist-only').scrollintoview();
            }
        });

        jQuery('#checkin').on('click', 'a[href="#break"]', function (evt) {
            evt.preventDefault();
            jQuery('#checkin .classes a.checked-in').trigger('click');
            jQuery('#metrics').scrollintoview({padding: {top:120,bottom:200,left:0,right:0}});
        });

        jQuery('#checkin').on('click', '#timer-expire a[href="#awards"]', function (evt) {
            jQuery('#checkin .classes a.checked-in').trigger('click');
            jQuery('#checkin').removeClass('timer-expire-only').scrollintoview();
        });

        jQuery('#checkin').on('mousedown', 'a[href="#study"]', function (evt) {
            var that = jQuery(this),
                position = null,
                callback = function (pos) {
                    clearTimeout(locationTimeout);
                    position = pos;
                    window.checkedIn = false;
                    window.className = that.data('className');
                    Drupal.ajax['classes'].eventResponse(this, evt);
                };
            jQuery('.minplayer-default-play').trigger('click');
            evt.preventDefault();
            if(typeof Drupal.ajax['classes'] == 'undefined')
                Drupal.ajax['classes'] = new Drupal.ajax(that.attr('href'), that, {
                        url: '/checkin/',
                        event: 'submit',
                        submit: { },
                        progress: {
                            type: 'throbber',
                            message: ''
                        },
                        beforeSend: function (xhr, options) {
                            var checked = [],
                                lat = position != null && typeof position.coords != 'undefined' ? position.coords.latitude : '',
                                lng = position != null && typeof position.coords != 'undefined' ? position.coords.longitude : '';
                            jQuery('#checklist input:checked').each(function () { checked[checked.length] = jQuery(this).attr('name'); });
                            options.data = 'checkedIn=' + window.checkedIn + '&date=' + encodeURIComponent(new Date().getTime()/1000) +
                                           '&className=' + encodeURIComponent(window.className) + '&checklist=' + checked.join(',') + '&location=' + lat + ',' + lng;

                        }
                    });
            //if(typeof navigator.geolocation != 'undefined')
            //{
            //    locationTimeout = setTimeout(callback, 2000);
            //    navigator.geolocation.getCurrentPosition(callback, callback, {maximumAge: 3600000, timeout:1000});
            //}
            //else
                callback();
        });

        jQuery('#main-menu').on('click', 'a', function () {
            jQuery('body').removeClass(footerOnly);
            jQuery('.new-award-only').removeClass('new-award-only')
                .parents('.panel-pane').scrollintoview();
            jQuery(window).trigger('scroll');
        });

        jQuery('#invite').on('click', '.like-us a', function (evt) {
            var href = jQuery(this).attr('href');
            /*if(href.indexOf('facebook') > 0)
            {
                FB.ui({
                          method: 'feed',
                          link: 'https://www.studysauce.com',
                          // picture: 'http://fbrell.com/f8.jpg',
                          name: 'Study Sauce',
                          caption: 'Awesomesauce'
                          // description: 'Must read daily!'
                      });
            }*/
            if(typeof window.open != 'undefined')
            {
                evt.preventDefault();
                evt.stopPropagation();
                // center the window above the current window
                var win = jQuery(window),
                    top = (win.height() / 2) - 250,
                    left = (win.width() / 2) - 275;
                window.open(href, 'like-us', 'height=500,width=550,location=no,menubar=no,status=no,toolbar=no,top=' + top + ',left=' + left);
            }
        });

        jQuery('#awards').on('click', '.awards > a', function (evt) {
            evt.preventDefault();
            var that = selectedAward = jQuery(this),
                description = null;
            jQuery('#awards .awards > a').removeClass('selected');
            that.addClass('selected');
            jQuery('#awards .awards > div').removeClass('selected').css('display', 'inline-block');
            if(that.is('.awarded'))
                description = that.nextUntil('a').filter('.after-only').addClass('selected');
            else
                description = that.nextUntil('a').filter('.before-only').addClass('selected');
            bubbleResize.call(that, evt);
        });

        jQuery('body').on('click', '#new-award a.fancy-close', function (evt) {
            evt.preventDefault();
            jQuery('.new-award-only').removeClass('new-award-only')
                .parents('.panel-pane').scrollintoview();
        });

        jQuery('#awards .awards > a.not-awarded').first().trigger('click');
    }

    function setPrototypeFunctions()
    {
        // by default show description for next unawarded
        $.fn.relocateAward = function (award, panel)
        {
            if(award == '' && selectedAward != null)
                selectedAward.trigger('click');

            jQuery('#awards .awards > a.not-awarded').first().trigger('click');
            var awardObj = $('.awards a[href="#' + award + '"]'),
                name = awardObj.text();
            $('#new-award strong').text(name.trim());
            $('#new-award')
                // move award to right tab
                .detach().appendTo($(panel).addClass('new-award-only'))
                // set the icon on the award
                .find('.badge').attr('class', 'badge').addClass(award);
            $(panel).parent().scrollintoview();
            $('#new-award').find('a[href="#awards"]')
                .unbind('click')
                .bind('click', function (evt) {
                                      awardObj.trigger('click').scrollintoview({padding: {top:120,bottom:200,left:0,right:0}});
                jQuery('.new-award-only').removeClass('new-award-only');
            });
        }

        var DrupalError = Drupal.ajax.prototype.error;
        Drupal.ajax.prototype.error = function (response, uri) {
            if(!isClosing)
                return DrupalError(response, uri);
        };

        if(typeof window.initialAward != 'undefined')
        {
            jQuery('#awards').relocateAward(window.initialAward, '#awards > .pane-content');
        }
    }

    jQuery(document).ready(function($) {
        $.error = console.log;

        setBindings();
        setPrototypeFunctions();

        // move arrow when window resizes
        $(window).on('resize', function (evt) {
            jQuery('#awards .awards > a.selected').each(function () {
                bubbleResize.call(this, evt);
            });
        }).trigger('resize');

        setClock();
        jQuery('#checkin .clock').find('a').click(function (evt) { evt.preventDefault(); });
        $.fn.startClock = function () {
            if(clock != null)
            {
                clearInterval(clock);
                clock = null;
            }
            seconds = TIMER_SECONDS;
            setClock();
            jQuery('.minplayer-default-play').trigger('click');
            clock = setInterval(function () {
                seconds--;
                setClock();
                if(seconds == 59)
                {
                    if(clock != null)
                    {
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
            if(clock != null)
            {
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

})(jQuery);

jQuery(document).ready(function($) {

    $(window).on('hashchange', function(){
        _gaq.push(['_trackPageview', location.pathname + location.search  + location.hash]);
    });
    $(window).trigger('hashchange');

    var lastAchievement = jQuery('#achievements .grid_6.highlighted, #achievements .grid_3.highlighted').last();

    $.fn.reorderSponsorship = function ()
    {
        jQuery('.page-home.parent #parent-sponsored').detach().insertBefore('#incentives #achievements')
    }
    jQuery('.page-home.parent #parent-sponsored').reorderSponsorship();

    var setParentStudent = function () {
        if(jQuery('.field-name-field-parent-student .form-type-radio:gt(0) input:checked, .field-name-field-parent-student input[type="hidden"]').length == 0)
        {
            jQuery('#user_profile input.form-radio, .parent-student-selection input.form-radio').change(function () {
                jQuery('#user_profile #submit-profile, .parent-student-selection #submit-profile').trigger('mousedown').trigger('click');
            });
            window.location.hash = '#home';
            jQuery('#home').scrollintoview();
        }
        else if(window.location.hash != '#home' && window.location.hash != '#awards')
        {
            if(jQuery('.field-name-field-parent-student .form-type-radio:gt(0) input[value="student"]:checked').length > 0 ||
               jQuery('.field-name-field-parent-student input[type="hidden"]').val() == 'student')
            {
                window.location.hash = '#dates';
                jQuery('#dates').scrollintoview();
            }
            else
            {
                window.location.hash = '#incentives';
                jQuery('.page #checkin').detach().insertAfter(jQuery('.page #tips'));
                jQuery('#main-menu a[href="#checkin"]').parent().detach().insertAfter(jQuery('#main-menu a[href="#tips"]').parent().last());
                jQuery('.page #metrics').detach().insertAfter(jQuery('.page #tips'));
                jQuery('#main-menu a[href="#metrics"]').parent().detach().insertAfter(jQuery('#main-menu a[href="#tips"]').parent().last());
                jQuery('#incentives').scrollintoview();
            }
            if(lastAchievement)
                lastAchievement.scrollintoview({padding: {top:120,bottom:200,left:0,right:0}});
        }
    };

    if(window.location.hash == '#welcome')
    {
        var welcome = jQuery('#welcome');
        if(welcome.length == 0)
            welcome = jQuery('<div id="welcome"><div>' +
                             '<a href="#" onclick="jQuery(\'#welcome\').remove(); return false;"></a>' +
                             '<a href="#" onclick="jQuery(\'#welcome\').remove(); return false;" class="fancy-close">&nbsp;</a>' +
                             '</div></div>');
        var url = '/welcome';
        if(jQuery('.field-name-field-parent-student .form-type-radio:gt(0) input[value="student"]:checked').length > 0 ||
           jQuery('.field-name-field-parent-student input[type="hidden"]').val() == 'student')
        {
            welcome.appendTo(jQuery('#awards > div'))
            window.location.hash = '#awards';
            jQuery('#awards').scrollintoview();
        }
        else
        {
            welcome.appendTo(jQuery('#home > div'))
            window.location.hash = '#home';
            jQuery('#home').scrollintoview();
        }
        welcome.addClass('loading').find('a').first().load(url + ' .page-inside .pane-content > *', function () {
            welcome.removeClass('loading');
        });
    }
    else if(jQuery('.new-award-only').length > 0)
    {
        window.location.hash = '#' + jQuery('.new-award-only').parent().scrollintoview().attr('id');
    }
    else
        setParentStudent();

    jQuery('body').bind('dragover', function (evt) {
        jQuery('body').addClass('file-drag');
    }).bind('dragleave', function (evt) {
                jQuery('body').removeClass('file-drag');
            }).bind('drop', function (evt) {
                        jQuery('body').removeClass('file-drag');
                    });

    jQuery('.page-dashboard .header-wrapper .user-account').click(function (evt) {
        evt.preventDefault();
        jQuery('body').removeClass(footerOnly).addClass('user-profile-only');
        window.location.hash = '#user-profile';
        jQuery(window).trigger('scroll');
    });

    jQuery(footerLinks).each(function (i, x) {
        jQuery('#subfooter a[href="/' + x + '"], .subfooter a[href="/' + x + '"]').click(function (evt) {
            evt.preventDefault();

            var pane = jQuery('#' + x + '.panel-pane');
            if(pane.length == 0)
            {
                pane = jQuery('<div id="' + x + '" class="panel-pane"><div class="pane-content" /></div>')
                    .appendTo(jQuery('.page .grid_12 > div'))
                    .on('click', 'a[href="/"]', function (evt) {
                        evt.preventDefault();
                        jQuery('body').removeClass(footerOnly);
                    });
            }
            var url = jQuery(this).attr('href');
            jQuery('body').removeClass(footerOnly).addClass(x + '-only');
            window.location.hash = '#' + x;
            pane.find('.pane-content').addClass('loading')
                .load(url + ' #page .content', function () {
                    pane.find('.pane-content').removeClass('loading');
                });
            jQuery(window).trigger('scroll');
        });
    });

    jQuery(window).scroll(function () {
        if(jQuery('#main-menu').length == 0)
            return;
        jQuery('#main-menu a').removeClass('skrollable-between');
        jQuery('.page .panel-pane .pane-content').each(function () {
            var that = jQuery(this),
                menu = jQuery('#main-menu li:visible a[href="#' + that.parent().attr('id') + '"]');
            if(menu.length == 0)
                return;
            if(that.offset().top < menu.offset().top +
                                   (jQuery(window).width() <= 759
                                       ? 140
                                       : 0) &&
                that.offset().top + that.height() > menu.offset().top + menu.height())
                menu.addClass('skrollable-between');
        });
    });

    jQuery(window).resize(function () {
        jQuery(window).trigger('scroll');
        if(jQuery('#read-more-incentives .grid_6').length == 0 &&
            jQuery(window).outerWidth(true) <= 963)
            jQuery('#student_step_1 .grid_6').detach().appendTo('#read-more-incentives');
        else if (jQuery('#student_step_1 .grid_6').length == 0 &&
            jQuery(window).outerWidth(true) > 963)
            jQuery('#read-more-incentives .grid_6').detach().appendTo('#student_step_1');
    });
    jQuery(window).trigger('resize');

    jQuery('.header a[href="/"]').click(function (evt) {
        if(jQuery(window).width() <= 759)
        {
            evt.preventDefault();
            jQuery('body').toggleClass('menu-open');
            jQuery(window).trigger('scroll');
        }
    });

    $.fn.returnScroll = function () {
        if(lastScheduleEdit)
        {
            jQuery('#' + lastScheduleEdit).scrollintoview();
        }
    };

    $.fn.refreshUploaders = function () {
        if(typeof plupload == 'undefined')
            return;
        // TODO: check if uploader has already been bound using actual IDs from the uploader
        for(var k in plupload.uploaders)
        {
            plupload.uploaders[k].bind('FilesAdded', function (up) {
                up.start();
                jQuery('#' + up.settings.browse_button).parents('.plupload-container').addClass('uploaded');
            });
            plupload.uploaders[k].bind('FileUploaded', function (up) {
            });
        }
    };
    $.fn.refreshUploaders();

    $.fn.goalsFunc = function () {
        jQuery(this).each(function () {
            var that = jQuery(this),
                setVisible = function () {
                    if(that.find('.field-name-field-type input:checked').length == 0)
                        return;
                    that.find('.field-name-field-hours, .field-name-field-gpa, .field-name-field-other, .field-name-field-grade').hide();
                    if(that.find('.field-name-field-type input[value="behavior"]:checked').length > 0)
                        that.find('.field-name-field-hours').show();
                    else if(that.find('.field-name-field-type input[value="milestone"]:checked').length > 0)
                        that.find('.field-name-field-grade').show();
                    else if(that.find('.field-name-field-type input[value="outcome"]:checked').length > 0)
                        that.find('.field-name-field-gpa').show();
                    else if(that.find('.field-name-field-type input[value="_none"]:checked').length > 0)
                        that.find('.field-name-field-other').show();
                },
                setValid = function () {
                    var form = that.parents('form'),
                        valid = true;
                    form.find('select:visible').each(function () {
                        if(jQuery(this).val() == '_none')
                            valid = false;
                    });
                    if(!valid)
                        form.removeClass('valid').addClass('invalid');
                    else
                        form.removeClass('invalid').addClass('valid');
                };
            that.find('.field-name-field-type input').change(setVisible);
            setVisible();

            that.find('select').change(setValid);
            setValid();

            that.find('.field-name-field-read-only input').each(function () {
                if(jQuery(this).val() == '0')
                    jQuery(this).parents('tr').addClass('edit');
            });

        });
        jQuery('.node-incentive-form input.form-submit').each(function () {
            var that = jQuery(this),
                id = that.attr('id');
            if(typeof Drupal.ajax[id] != 'undefined')
            {
                that.unbind(Drupal.ajax[id].event);
                that.bind(Drupal.ajax[id].event, function (event) {
                    if(!that.parents('form').is('.invalid'))
                        return Drupal.ajax[id].eventResponse(this, event);
                });
            }
        });
    };

    jQuery('.field-name-field-goals tr').goalsFunc();

});