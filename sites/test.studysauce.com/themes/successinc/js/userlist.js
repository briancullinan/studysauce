
jQuery(document).ready(function($) {

    jQuery('body').on('click', 'a[href="#userlist"]', function (evt) {
        jQuery('body').removeClass('uid-only');
        jQuery('.user-pane').remove();
    });

    var status = ['- Status -'];
    jQuery('#userlist').find('td:nth-child(1)').each(function () {
        if(status.indexOf(jQuery(this).text()) == -1)
            status[status.length] = jQuery(this).text();
    });
    jQuery('#userlist').find('th:nth-child(1)').html('<select><option>' + status.join("</option><option>") + '</option></select>')
    var dates = ['- Date -'];
    jQuery('#userlist').find('td:nth-child(2)').each(function () {
        if(dates.indexOf(jQuery(this).text()) == -1)
            dates[dates.length] = jQuery(this).text();
    });
    jQuery('#userlist').find('th:nth-child(2)').html('<select><option>' + dates.join("</option><option>") + '</option></select>')
    var students = ['- Student -'];
    jQuery('#userlist').find('td:nth-child(3)').each(function () {
        if(students.indexOf(jQuery(this).text()) == -1)
            students[students.length] = jQuery(this).text();
    });
    jQuery('#userlist').find('th:nth-child(3)').html('<select><option>' + students.join("</option><option>") + '</option></select>')
    var schools = ['- School -'];
    jQuery('#userlist').find('td:nth-child(4)').each(function () {
        if(schools.indexOf(jQuery(this).text()) == -1)
            schools[schools.length] = jQuery(this).text();
    });
    jQuery('#userlist').find('th:nth-child(4)').html('<select><option>' + schools.join("</option><option>") + '</option></select>')

    jQuery('#userlist').on('change', 'select', function () {
        jQuery('tr').show();
        if(jQuery(this).val() != '- Status -' &&
            jQuery(this).val() != '- Date -' &&
            jQuery(this).val() != '- Student -' &&
            jQuery(this).val() != '- School -')
        {
            var i = jQuery(this).parents('th').index() + 1,
                filter = jQuery(this).val();
            jQuery('td:nth-child(' + i + ')').each(function () {
                if(jQuery(this).text() != filter)
                    jQuery(this).parents('tr').hide();
            });
        }
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