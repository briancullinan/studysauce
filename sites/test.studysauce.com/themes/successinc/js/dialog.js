jQuery(document).ready(function () {
    var dialogTimeout;

    // look at every link with a hash, store with google analytics and search for dialogs.
    function doDialogResize()
    {
        jQuery(this).parent('.fixed-centered').css('margin-top', -jQuery(this).height()/2);
        if(jQuery(window).height() - 200 < jQuery(this).height())
        {
            jQuery(this).parent('.fixed-centered').css('position', 'absolute');
            jQuery(this).scrollintoview({padding: {top:120,bottom:100,left:0,right:0}});
        }
        else
            jQuery(this).parent('.fixed-centered').css('position', 'fixed');
    }

    jQuery.fn.dialog = function (mode)
    {
        var that = jQuery(this);
        if(!mode)
        {
            var countSteps = 0;
            if((countSteps = jQuery(this).parent('.fixed-centered').find('.dialog').length) > 1)
            {
                var steps = jQuery(this).find('.steps');
                if(steps.length == 0)
                    steps = jQuery('<div class="steps"><ol>' +
                        Array.apply(null, Array(countSteps)).map(function (_, i) {return '<li>&nbsp;</li>';}).join('') +
                        '</ol></div>').appendTo(that.find('.highlighted-link'));
                steps.find('li.pop').removeClass('pop');
                steps.find('li').eq(that.index()).addClass('pop');
            }
            that.show().parent('.fixed-centered').show();
            doDialogResize.apply(this);
        }
        else if(mode == 'hide')
        {
            that.hide().parent('.fixed-centered').hide();
        }
    };

    jQuery(window).resize(function () {
        if(dialogTimeout)
            clearTimeout(dialogTimeout);
        dialogTimeout = setTimeout(function () {
            jQuery('.dialog:visible').each(doDialogResize);
        }, 100);
    });

    jQuery('body').on('click', 'a[href^="#"]', function (evt)
    {
        var that = jQuery(this),
            link = that.attr('href');
        if(jQuery(link).length > 0 && jQuery(link).is('.dialog'))
        {
            evt.preventDefault();
            that.parents('.dialog').dialog('hide');
            jQuery(link).dialog();
        }
    });

    jQuery('body').on('click', '.dialog a[href="#close"]', function (evt)
    {
        evt.preventDefault();
        jQuery(this).parents('.dialog').dialog('hide');
    });

});