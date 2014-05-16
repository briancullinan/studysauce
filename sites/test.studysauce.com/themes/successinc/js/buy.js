
jQuery(document).ready(function($) {

    jQuery('a[href^="/cart/add/"]').on('click', function (evt) {
        evt.preventDefault();
        var that = jQuery(this);
        jQuery('a[href^="/cart/add/"]').removeClass('checked');
        that.addClass('checked');
        jQuery.ajax({
            url: that.attr('href'),
            type: 'POST',
            success: function (response)
            {
                var jsponce = jQuery(response);
                jsponce.find('input[type="hidden"]').each(function () {
                    var that = jQuery(this);
                    jQuery('input[name="' + that.attr('name') + '"]').val(that.val());
                });
                jQuery('#uc-order-total-preview').replaceWith(jsponce.find('#uc-order-total-preview'));
            }
                    });
    });

});