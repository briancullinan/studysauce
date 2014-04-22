
(function($) {

    var footerLinks = ['terms', 'privacy', 'about-us', 'contact', 'refund'],
        footerOnly = footerLinks.join('-only ') + '-only' + ' user-profile-only awards-only',
        menuOnly,
        selectedAward = null,
        isClosing = false;

    function bubbleResize() {
        var that = $(this);
        var bubble = that.nextUntil('a');
        var thisArrow = bubble.find('.awardArrow');

        var thisPosition = that.offset();
        var thisAlignX = thisPosition.left - $('#badges .awards').offset().left + (that.outerWidth()/2) - (thisArrow.width() / 2);
        thisArrow.css('margin-left', thisAlignX);
    }

    function setBindings()
    {
        jQuery('#home input[type="checkbox"]').each(function () {
            jQuery(this).data('origState', jQuery(this).prop('checked'));
        });
        jQuery('#home').on('change', 'input', function (evt) {
            evt.preventDefault();
            jQuery(this).prop('checked', jQuery(this).data('origState'));
        });

        jQuery('body').on('click', 'a[href="#study-quiz"]', function (evt) {
            evt.preventDefault();
            jQuery('#home').addClass('study-quiz-only').scrollintoview();
        });

        jQuery('body').on('click', 'a[href="#badges"]', function (evt) {
            evt.preventDefault();
            jQuery('body').removeClass(footerOnly).addClass('awards-only');
            window.location.hash = '#badges';
            jQuery(window).trigger('scroll');
            bubbleResize.call(jQuery('#badges .awards > a.selected'));
        });

        jQuery('#study-quiz').on('click', 'a[href="#retry"]', function (evt) {
            evt.preventDefault();
            jQuery('#study-quiz #webform-ajax-wrapper-17').load('/quiz #node-17 #webform-ajax-wrapper-17 > *', function () {
                Drupal.attachBehaviors();
                jQuery('#tips').scrollintoview();
                //Drupal.ajax['edit-webform-ajax-submit-17'] = new Drupal.ajax('edit-webform-ajax-submit-17', jQuery('#study-quiz input#edit-webform-ajax-submit-17'), {"callback":"webform_ajax_callback","wrapper":"webform-ajax-wrapper-17","progress":{"message":"","type":"throbber"},"event":"click","url":"\/system\/ajax","submit":{"_triggering_element_name":"op","_triggering_element_value":"Submit"}});
            });
        });

        jQuery('#main-menu a').each(function () {
            var that = jQuery(this);
            //if(jQuery(that.attr('href') + '.panel-pane').length > 0)
            {
                jQuery('body').on('click', 'a[href="' + that.attr('href') + '"]', function (evt) {
                    var that = jQuery(this);
                    jQuery('.new-award-only').removeClass('new-award-only')
                        .parents('.panel-pane').scrollintoview();
                    if(that.parent().find('ul').length > 0) // this menu has subitems
                    {
                        // cancel #hash change
                        evt.preventDefault();
                        that.toggleClass('selected');
                    }
                    else
                    {
                        jQuery('body').removeClass(footerOnly).removeClass(menuOnly).addClass(that.attr('href').substring(1) + '-only');
                        that.parents('ol,ul').prev('a').addClass('selected');
                    }
                    jQuery(window).trigger('scroll');
                });
            }
            //else
            {

            }
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

        jQuery('#badges').on('click', '.awards > a', function (evt) {
            evt.preventDefault();
            var that = selectedAward = jQuery(this),
                description = null;
            jQuery('#badges .awards > a').removeClass('selected');
            that.addClass('selected');
            jQuery('#badges .awards > div').removeClass('selected').css('display', 'inline-block');
            if(that.is('.awarded'))
                description = that.nextUntil('a').filter('.after-only').addClass('selected');
            else
                description = that.nextUntil('a').filter('.before-only').addClass('selected');
            bubbleResize.call(that);
        });

        jQuery('body').on('click', '#new-award a.fancy-close', function (evt) {
            evt.preventDefault();
            jQuery('.new-award-only').removeClass('new-award-only')
                .parents('.panel-pane').scrollintoview();
        });

        jQuery('#badges .awards > a.not-awarded').first().trigger('click');
    }

    function setPrototypeFunctions()
    {
        // by default show description for next unawarded
        $.fn.relocateAward = function (award, panel)
        {
            if(award == '' && selectedAward != null)
                selectedAward.trigger('click');

            jQuery('#badges .awards > a.not-awarded').first().trigger('click');
            var awardObj = $('.awards a[href="#' + award + '"]'),
                name = awardObj.text();
            $('#new-award strong').text(name.trim());
            $('#new-award')
                // move award to right tab
                .detach().appendTo($(panel).addClass('new-award-only'))
                // set the icon on the award
                .find('.badge').attr('class', 'badge').addClass(award);
            if(panel != '')
                $(panel).parent().scrollintoview();
            $('#new-award').find('a[href="#badges"]')
                .unbind('click')
                .bind('click', function (evt) {
                                      awardObj.trigger('click').scrollintoview({padding: {top:120,bottom:200,left:0,right:0}});
                jQuery('.new-award-only').removeClass('new-award-only');
            });
        };

        if(typeof Drupal.ajax != 'undefined')
        {
            var DrupalError = Drupal.ajax.prototype.error;
            Drupal.ajax.prototype.error = function (response, uri) {
                if(!isClosing)
                    return DrupalError(response, uri);
            };
        }

        if(typeof window.initialAward != 'undefined')
        {
            jQuery('#badges').relocateAward(window.initialAward, '#badges > .pane-content');
        }
    }

    jQuery(document).ready(function($) {

        if(typeof console.error != 'undefined')
            jQuery.error = console.error;

        menuOnly = jQuery('#main-menu a').map(function (i, x) {return jQuery(x).attr('href').substring(1)}).get().join('-only ') + '-only';

        $.error = console.log;

        setBindings();
        setPrototypeFunctions();

        // move arrow when window resizes
        $(window).on('resize', function (evt) {
            jQuery('#badges .awards > a.selected').each(function () {
                bubbleResize.call(this);
            });
        }).trigger('resize');


        // The calendar needs to be in view for sizing information.  This will not initialize when display:none;, so instead
        //   we will activate the calendar only once, when the menu is clicked, this assumes #hash detection works, and
        //   it triggers the menu clicking
        $(window).on('hashchange', function(){
            _gaq.push(['_trackPageview', location.pathname + location.search  + location.hash]);
            if(jQuery('#main-menu a[href="' + location.hash + '"]').length > 0)
            {
                jQuery('#main-menu a[href="' + location.hash + '"]').trigger('click');
            }
        });
        $(window).trigger('hashchange');

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
                // as a student it shows up on plan tab when they first purchase
                //welcome.appendTo(jQuery('#plan > div'))
                window.location.hash = '#plan';
                jQuery('#plan').scrollintoview();
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
                                jQuery('body').removeClass(footerOnly).removeClass(menuOnly);
                            });
                }
                var url = jQuery(this).attr('href');
                jQuery('body').removeClass(footerOnly).removeClass(menuOnly).addClass(x + '-only');
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

    });

})(jQuery);

