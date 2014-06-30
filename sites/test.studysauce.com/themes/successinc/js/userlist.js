
jQuery(document).ready(function($) {

    var userlist = jQuery('#userlist');

    var status = ['Status', 'Ascending', 'Descending', 'Red', 'Yellow', 'Green'];
    userlist.find('th:nth-child(1)').html('<select><option>' + status.join("</option><option>") + '</option></select>')

    var dates = ['Date', 'Ascending (A-Z)', 'Descending (Z-A)'];
    userlist.find('td:nth-child(2)').each(function () {
        if(dates.indexOf(jQuery(this).text()) == -1)
            dates[dates.length] = jQuery(this).text();
    });
    userlist.find('th:nth-child(2)').html('<select><option>' + dates.join("</option><option>") + '</option></select>')
    var students = ['Student', 'Ascending (A-Z)', 'Descending (Z-A)'];
    userlist.find('td:nth-child(3)').each(function () {
        if(students.indexOf(jQuery(this).text()) == -1)
            students[students.length] = jQuery(this).text();
    });
    userlist.find('th:nth-child(3)').html('<select><option>' + students.join("</option><option>") + '</option></select>')
    var schools = ['School', 'Ascending (A-Z)', 'Descending (Z-A)'];
    userlist.find('td:nth-child(4)').each(function () {
        if(schools.indexOf(jQuery(this).text()) == -1)
            schools[schools.length] = jQuery(this).text();
    });
    userlist.find('th:nth-child(4)').html('<select><option>' + schools.join("</option><option>") + '</option></select>')

    userlist.on('click', 'a[href="#change-status"]', function (evt) {
        evt.preventDefault();
        evt.stopPropagation();
        var row = jQuery(this).parents('tr'),
            selectStatus = jQuery('#select-status');
        if(selectStatus.is(':visible'))
        {
            selectStatus.hide();
            return;
        }
        selectStatus.css('top', row.position().top);
        selectStatus.off();
        selectStatus.on('click', 'a', function (evt) {
            evt.preventDefault();
            var status = jQuery(this).attr('href').substring(1),
                uid = (/uid([0-9]+)(\s|$)/ig).exec(row.attr('class'));
            row.removeClass('status_green status_yellow status_red');
            row.addClass('status_' + status);
            jQuery.ajax({
                url: '/user/save/status',
                type: 'POST',
                dataType: 'json',
                data: {
                    uid: uid[1],
                    status: status
                },
                success: function () {
                    userlist.find('tr.uid' + uid[1]).removeClass('status_green status_yellow status_red')
                        .addClass('status_' + status);
                }
            });
        });
        selectStatus.show();
    });

    userlist.on('click', function (evt) {
        jQuery('#select-status').hide();
    });

    userlist.on('change', 'select', function () {
        jQuery('tr').show();
        userlist.find('select').each(function () {
            if(jQuery(this).val() != 'Status' &&
                jQuery(this).val() != 'Date' &&
                jQuery(this).val() != 'Student' &&
                jQuery(this).val() != 'School')
            {
                jQuery(this).removeClass('unfiltered').addClass('filtered');
                var i = jQuery(this).parents('th').index() + 1,
                    filter = jQuery(this).val();
                // if we are changing the status, select rows by class name
                if(i == 1)
                {
                    userlist.find('tbody tr').hide();
                    userlist.find('tr.status_' + filter.toLowerCase()).show();
                }
                else
                {
                    userlist.find('td:nth-child(' + i + ')').each(function () {
                        if(jQuery(this).text() != filter)
                            jQuery(this).parents('tr').hide();
                    });
                }
            }
            else
                jQuery(this).removeClass('filtered').addClass('unfiltered');
        });
    });

    userlist.on('click', 'a[href^="#uid-"]', function (evt) {
        evt.preventDefault();
        var uid = jQuery(this).attr('href').substring(5);
        jQuery('.user-pane').remove();
        var pane = jQuery('#uid-' + uid + '.panel-pane');
        if(pane.length == 0)
        {
            pane = jQuery('<div id="uid-' + uid + '" class="panel-pane user-pane"><div class="pane-content" /></div>')
                .appendTo(jQuery('.page .grid_12 > div'));
        }

        jQuery('body').removeClass('home-only,userlist-only').addClass('uid-only');
        window.location.hash = '#uid-' + uid;

        pane.find('.pane-content').addClass('loading');
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