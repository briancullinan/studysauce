(function($) {

    var selectedAward = null,
        isClosing = false;

    /*function bubbleResize() {
        var that = $(this);
        var bubble = that.nextUntil('a');
        var thisArrow = bubble.find('.awardArrow');

        var thisPosition = that.offset();
        var thisAlignX = thisPosition.left - $('#badges .awards').offset().left + (that.outerWidth()/2) - (thisArrow.width() / 2);
        thisArrow.css('margin-left', thisAlignX);
    }*/

    function setBindings()
    {
        jQuery('.page-dashboard .header-wrapper a[href="/user/logout"],' +
            '.page-dashboard #checkin .minplayer-default-play,' +
            '.page-dashboard #checkin .minplayer-default-pause').tooltip({position:{my: 'center top+15', at:'center bottom'}, open: function (evt, ui) {
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

        $(window).on('hashchange', function(){
            _gaq.push(['_trackPageview', location.pathname + location.search  + location.hash]);
            var allMenuItems = jQuery('#main-menu a[href="' + location.hash + '"], .subfooter a[href="/' + location.hash.substring(1) + '"], #subfooter  a[href="/' + location.hash.substring(1) + '"]');
            if(allMenuItems.length > 0)
            {
                allMenuItems.first().trigger('click');
            }
        });

        jQuery('#home input[type="checkbox"]').each(function () {
            jQuery(this).data('origState', jQuery(this).prop('checked'));
        });
        jQuery('#home').on('change', 'input[type="checkbox"]', function (evt) {
            evt.preventDefault();
            if(jQuery(this).prop('checked') != jQuery(this).data('origState'))
                jQuery(this).prop('checked', jQuery(this).data('origState'));
        });

        jQuery('#main-menu a').each(function () {
            var that = jQuery(this);
            // check if panel exists, we still want to allow items that lead to other pages
            //if(jQuery(that.attr('href') + '.panel-pane').length > 0)
            {
                jQuery('body').on('click', 'a[href="' + that.attr('href') + '"]', function (evt) {
                    var that = jQuery(this);
                    // only show last open menu in mobile mode if they click on subitems
                    that.parents('ol,ul').find('li').removeClass('last-open');
                    if(that.parent().find('ul').length > 0) // this menu has subitems
                    {
                        // cancel #hash change
                        evt.preventDefault();
                        that.toggleClass('selected').parent().addClass('last-open');
                    }
                    else
                    {
                        jQuery('body').removeClass('menu-open');
                        jQuery('body .page .panel-pane').hide().filter(that.attr('href')).show();
                        that.parents('ol,ul').prev('a').addClass('selected').parent().addClass('last-open');
                    }
                    jQuery(window).trigger('scroll');
                });
            }
            //else
            {

            }
        });

        jQuery('#profile, #plan, #premium').on('click', '.dialog[id*="bill"] a[href="#close"]', function () {
            jQuery(this).parents('.panel-pane').find('.dialog[id*="upgrade"]').dialog();
        });
        jQuery('#profile, #plan, #premium').on('click', 'a[href="#bill-send"]', function (evt) {
            evt.preventDefault();
            var billing = jQuery(this).parents('.dialog'),
                tab = jQuery(this).parents('.panel-pane');
            if(billing.is('.invalid'))
                return;
            billing.addClass('invalid');
            jQuery.ajax({
                url: 'billing/send',
                type: 'POST',
                dataType: 'json',
                data: {
                    first: billing.find('input[name="invite-first"]').val(),
                    last: billing.find('input[name="invite-last"]').val(),
                    email: billing.find('input[name="invite-email"]').val()
                },
                success: function (data) {
                    billing.find('input[name="invite-first"]').val('');
                    billing.find('input[name="invite-last"]').val('');
                    billing.find('input[name="invite-email"]').val('');
                    billing.removeClass('invalid').dialog('hide');
                    billing.next('.dialog').dialog();
                }
            });
        });

        // setup footer menu loading
        jQuery('.subfooter .secondary-menu a')
            .map(function (i, x) { return jQuery(x).attr('href').substr(1); })
            .each(function (i, x) {
                // skip logout because we want that to redirect
                if(x == 'user/logout')
                    return true;
            jQuery('body').on('click', 'a[href="/' + x + '"], a[href="#' + x + '"]', function (evt) {
                evt.preventDefault();

                // add a panel for us to load in to
                jQuery('body .page .panel-pane').hide();
                var pane = jQuery('#' + x + '.panel-pane').show();
                if(pane.length == 0)
                {
                    pane = jQuery('<div id="' + x + '" class="panel-pane"><div class="pane-content" /></div>')
                        .appendTo(jQuery('.page .grid_12 > div')).show()
                        .on('click', 'a[href="/"]', function (evt) {
                                evt.preventDefault();
                                jQuery('body .page .panel-pane').hide().first().show();
                            });
                }

                // load the panel content
                var url = jQuery(this).attr('href');
                window.location.hash = '#' + x;
                pane.find('.pane-content').addClass('loading')
                    .load(url + ' #page .content', function () {
                              pane.find('.pane-content').removeClass('loading');
                          });

                // TODO: use script loader from userlist
                if(x == 'contact')
                    jQuery.getScript('/' + pathToTheme + '/js/contact.js');
                if(x == 'about-us')
                    $('head').append( $('<link rel="stylesheet" type="text/css" />').attr('href', '/' + pathToTheme + '/about.css') );
                jQuery(window).trigger('scroll');
            });
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
    }

    function setPrototypeFunctions()
    {
        if(typeof Drupal.ajax != 'undefined')
        {
            var DrupalError = Drupal.ajax.prototype.error;
            Drupal.ajax.prototype.error = function (response, uri) {
                if(!isClosing)
                    return DrupalError(response, uri);
            };
        }
    }

    jQuery(document).ready(function($) {

        if(typeof console.error != 'undefined')
            jQuery.error = console.error;

        $.error = console.log;

        setBindings();
        setPrototypeFunctions();
        $(window).trigger('hashchange');


        jQuery(window).scroll(function () {
            if(jQuery('#main-menu').length == 0)
                return;
            jQuery('#main-menu a').removeClass('skrollable-between');
            jQuery('.page .panel-pane .pane-content').each(function () {
                var that = jQuery(this),
                    menu = jQuery('#main-menu li:visible a[href="#' + that.parent().attr('id') + '"]');
                if(menu.length == 0)
                    return;
                if(that.offset().top <= menu.offset().top +
                    (jQuery(window).width() <= 759
                        ? 140
                        : 0) &&
                    that.offset().top + that.height() >= menu.offset().top + menu.height())
                    menu.addClass('skrollable-between');
            });
        });


        // move arrow when window resizes
        $(window).on('resize', function (evt) {
            jQuery('#badges .awards > a.selected').each(function () {
                bubbleResize.call(this);
            });
        }).trigger('resize');


        // The calendar needs to be in view for sizing information.  This will not initialize when display:none;, so instead
        //   we will activate the calendar only once, when the menu is clicked, this assumes #hash detection works, and
        //   it triggers the menu clicking

        jQuery('body').bind('dragover', function (evt) {
            jQuery('body').addClass('file-drag');
        }).bind('dragleave', function (evt) {
                    jQuery('body').removeClass('file-drag');
                }).bind('drop', function (evt) {
                            jQuery('body').removeClass('file-drag');
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
                jQuery('#main-menu').find('li').removeClass('last-open');
                jQuery(window).trigger('scroll');
            }
        });

    });

})(jQuery);

