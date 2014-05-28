
jQuery(document).ready(function($) {

    jQuery('body').on('click', 'a[href="#userlist"]', function (evt) {
        jQuery('body').removeClass('uid-only');
        jQuery('.user-pane').remove();
    });

    jQuery('#userlist').on('click', 'a[href^="#uid-"]', function (evt) {
        evt.preventDefault();
        var uid = jQuery(this).attr('href').substring(5);
        var pane = jQuery('#' + uid + '.panel-pane');
        if(pane.length == 0)
        {
            pane = jQuery('<div id="uid-' + uid + '" class="panel-pane user-pane"><div class="pane-content" /></div>')
                .appendTo(jQuery('.page .grid_12 > div'));
        }

        jQuery('body').removeClass('home-only,userlist-only').addClass('uid-only');
        window.location.hash = '#uid-' + uid;

        pane.show().find('.pane-content').addClass('loading');
        jQuery.ajax({
            url: '/adviser?uid=' + uid,
            dataType: 'json',
            type: 'GET',
            success: function (response) {
                pane.find('.pane-content').html(response.content);

                jQuery(response.styles).each(function () {
                    var url = jQuery(this).attr('href');
                    if(typeof url != 'undefined' && jQuery('link[href="' + url + '"]').length == 0)
                        $('head').append('<link href="' + url + '" type="text/css" rel="stylesheet" />');
                    else
                    {
                        var re = (/url\("(.*?)"\)/ig),
                            match,
                            media = jQuery(this).attr('media');
                        while (match = re.exec(jQuery(this).html())) {
                            if(jQuery('link[href="' + match[1] + '"]').length == 0 &&
                               jQuery('style:contains("' + match[1] + '")').length == 0)
                            {
                                if(typeof media == 'undefined' || media == 'all')
                                    $('head').append('<link href="' + match[1] + '" type="text/css" rel="stylesheet" />');
                                else
                                    $('head').append('<style media="' + media + '">@import url("' + match[1] + '");');
                            }
                        }
                    }
                });

                jQuery(response.scripts).each(function () {
                    var url = jQuery(this).attr('src');
                    if(typeof url != 'undefined' && jQuery('script[src="' + url + '"]').length == 0)
                        jQuery.getScript(url);
                });

                pane.find('.pane-content').removeClass('loading');
            }
                    });
    });

});