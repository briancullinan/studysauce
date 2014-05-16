
jQuery(document).ready(function($) {

    var deadlines = $('#deadlines');
    deadlines.on('click', '.row .field-name-field-assignment .read-only', function () {
        jQuery(this).parents('.row').toggleClass('selected');
    });

    deadlines.on('change', '.field-name-field-class-name select, .field-name-field-reminder input, .field-name-field-due-date input', function () {
        jQuery(this).parents('.row').datesFunc();
    });

    deadlines.on('click', 'a[href="#cancel-dates"]', function (evt) {
        evt.preventDefault();
        deadlines.removeClass('edit-date-only').scrollintoview();
        deadlines.find('.field-add-more-submit').show();
        jQuery(this).parents('.row').removeClass('edit');
    });

    deadlines.on('keyup', '.field-name-field-assignment input', function () {
        jQuery(this).parents('.row').datesFunc();
    });

    deadlines.on('click', 'a[href="#remove-reminder"]', function (evt) {
        evt.preventDefault();
        var row = jQuery(this).parents('.row');
        $.ajax({
                   url: '/node/save/key_dates',
                   type: 'POST',
                   dataType: 'json',
                   data: {
                       remove: row.attr('id').substr(0, 4) == 'eid-' ? row.attr('id').substring(4) : null
                   },
                   success: function (data) {
                       deadlines.find('#new-dates-row ~ .row, #new-dates-row ~ .head')
                           .replaceWith(jQuery(data.reminders).find('#new-dates-row ~ .row, #new-dates-row ~ .head'));
                   }
               });
    });

    deadlines.on('click', 'a[href="#save-dates"]', function (evt) {
        evt.preventDefault();
        if(!jQuery(this).parents('.row').is('.invalid'))
        {
            var row = jQuery(this).parents('.row'),
                reminders = row.find('input[name*="dates-reminder-"]:checked').map(function (i, x) {return $(x).val();}).get();
            $.ajax({
                       url: '/node/save/key_dates',
                       type: 'POST',
                       dataType: 'json',
                       data: {
                           eid: row.attr('id').substr(0, 4) == 'eid-' ? row.attr('id').substring(4) : null,
                           className: row.find('.field-name-field-class-name select').val(),
                           assignment: row.find('.field-name-field-assignment input').val(),
                           reminders: reminders.join(','),
                           due: row.find('.field-name-field-due-date input').val(),
                           percent: row.find('.field-name-field-percent').is(':visible') ? row.find('.field-name-field-percent input').val() : 0
                       },
                       success: function (data) {
                           // clear input form
                           if(row.is('#new-dates-row'))
                           {
                               row.find('.field-name-field-class-name select').val('_none');
                               row.find('.field-name-field-assignment input').val('');
                               row.find('input[name*="dates-reminder-"]').prop('checked', false);
                               row.find('.field-name-field-due-date input').val('');
                               row.find('.field-name-field-percent input').val(0);
                           }

                           // update key dates list
                           deadlines.find('#new-dates-row ~ .row, #new-dates-row ~ .head')
                               .replaceWith(jQuery(data.reminders).find('#new-dates-row ~ .row, #new-dates-row ~ .head'));

                           // update plan tab
                           var plan = jQuery('#plan');
                           plan.find('.row, .head').remove();
                           jQuery(data.plan).find('.row, .head')
                               .insertBefore(plan.find('.pane-content p').last());

                           // update deadline view state
                           deadlines.removeClass('edit-date-only');
                           row.removeClass('edit').datesFunc();
                           deadlines.find('.field-add-more-submit').show();
                           if(jQuery(window).width() <= 759)
                               row.scrollintoview({padding: {top:120,bottom:200,left:0,right:0}});
                       }
                   });
        }
    });

    deadlines.on('click', 'a[href="#edit-reminder"]', function (evt) {
        evt.preventDefault();
        jQuery(this).parents('.row').addClass('edit').scrollintoview().datesFunc();
        jQuery('#deadlines').addClass('edit-date-only');
    });

    $.fn.datesFunc = function () {
        jQuery(this).each(function () {
            var that = jQuery(this),
                error = false;
            if(that.find('select').val() == '_none')
                error = true;
            if(that.find('.field-name-field-assignment input').val().trim() == '')
                error = true;
            if(that.find('.field-name-field-reminder input:checked').length == 0)
                error = true;
            if(that.find('.field-name-field-due-date input').val().trim() == '')
                error = true;
            if(error)
            {
                that.removeClass('valid').addClass('invalid');
                that.parents('form').removeClass('valid').addClass('invalid');
            }
            else
            {
                that.removeClass('invalid').addClass('valid');
                that.parents('form').removeClass('invalid').addClass('valid');
            }

            if(that.find('.field-name-field-class-name select').val() == 'Nonacademic')
                that.find('.field-name-field-percent').hide();
            else
                that.find('.field-name-field-percent').show();

            that.find('.field-name-field-due-date input').datepicker({
                                                                         minDate: 0,
                                                                         autoPopUp:'focus',
                                                                         changeMonth: true,
                                                                         changeYear: true,
                                                                         closeAtTop: false,
                                                                         dateFormat: 'mm/dd/yy',
                                                                         defaultDate:'0y',
                                                                         firstDay:0,
                                                                         fromTo:false,
                                                                         speed:'immediate',
                                                                         yearRange: '-3:+3'
                                                                     });
        });

        // don't need to limit submit because it goes back to edit mode if invalid
        /*jQuery('#deadlines input[value="Save"]').each(function () {
         var that = jQuery(this),
         id = that.attr('id');
         that.unbind(Drupal.ajax[id].event);
         that.bind(Drupal.ajax[id].event, function (event) {
         if(!that.parents('form').is('.invalid'))
         return Drupal.ajax[id].eventResponse(this, event);
         });
         });*/
    };

    deadlines.find('.row').datesFunc();
});

