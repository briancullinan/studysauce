

jQuery(document).ready(function($) {

    // TODO: remove this
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

    var setVisible = function () {
            var that = jQuery(this).parents('#field-goals-values > tbody > tr');
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
            var valid = true,
                that = jQuery(this).parents('.row');
            if(that.find('select:visible').val() == '_none' ||
               that.find('.field-name-field-reward textarea').val().trim() == '')
                valid = false;
            if(!valid)
                that.removeClass('valid').addClass('invalid');
            else
                that.removeClass('invalid').addClass('valid');
        };

    jQuery('#incentives').on('change', '.row select', setValid);
    jQuery('#incentives').on('change', '.row textarea', setValid);
    jQuery('#incentives').on('keyup', '.row textarea', setValid);

    jQuery('#incentives').on('click', 'a[href="#edit-reward"]', function (evt) {
        evt.preventDefault();
        var row = jQuery(this).parents('.row');
        row.addClass('edit').find('.field-name-field-read-only input').val(0);
        row.goalsFunc();
        jQuery('#incentives').addClass('edit-goal');
    });

    jQuery('#incentives').on('click', 'a[href="#save-incentive"]', function (evt) {
        evt.preventDefault();
        var row = jQuery(this).parents('.row');
        if(row.is('.valid'))
        {
            row.find('.field-name-field-read-only input').val(1);
            $.ajax({
                       url: '/node/save/goals',
                       type: 'POST',
                       dataType: 'json',
                       data: {
                           value: row.find('select').first().val(),
                           reward: row.find('.field-name-field-reward textarea').val(),
                           type: row.find('select').first().attr('name').substring(5)
                       },
                       success: function (data) {

                           // update tabs
                           jQuery('#incentives').removeClass('edit-goal');
                           jQuery('#incentives .pane-content > div').removeClass('step_1 step_2');
                           jQuery('#incentives .pane-content > div').addClass('step_3');

                           // update incentives rows
                           jQuery('#non-sponsored > *, #parent-sponsored > *').remove();
                           jQuery(data.goals).find('#non-sponsored > *')
                               .appendTo(jQuery('#non-sponsored'));
                           jQuery('#incentives .row').goalsFunc();

                           // update achievements
                           jQuery('#achievements > *').remove();
                           jQuery(data.achievements).find('> *')
                               .appendTo(jQuery('#achievements'));

                           // update awards such as pulse detected for setting up a class
                           if (typeof data.awards != 'undefined') {
                               var lastAward = null;
                               for (var i in data.awards) {
                                   if (data.awards[i] != false && jQuery('#awards #' + i).is('.not-awarded')) {
                                       jQuery('#awards #' + i).removeClass('not-awarded').addClass('awarded');
                                       lastAward = i;
                                   }
                               }

                               if (lastAward == 'setup-hours' || lastAward == 'setup-milestone' || lastAward == 'setup-outcome' ||
                                   lastAward == 'beginner-apples' || lastAward == 'beginner-jackpot' || lastAward == 'intermediate-disco' ||
                                   lastAward == 'intermediate-high' || lastAward == 'advanced=magneto' || lastAward == 'advanced-wall' ||
                                   lastAward == 'advanced-veni')
                                   jQuery('#awards').relocateAward(lastAward, '#incentives > .pane-content');
                               else if (lastAward != null)
                                   jQuery('#awards').relocateAward(lastAward, '#awards > .pane-content');
                               else
                                   jQuery('#awards').relocateAward('');
                           }
                       }
                   });
        }
        if(jQuery(window).outerWidth(true) <= 963) {
            row.find('.field-name-field-type').scrollintoview({padding: {top:120,bottom:200,left:0,right:0}});
        }
    });

    $.fn.goalsFunc = function () {
        jQuery(this).each(function (i, r) {
            var that = jQuery(r);
            that.find('.field-name-field-read-only input').each(function () {
                if(jQuery(this).val() == '0')
                    that.addClass('edit');
            });
            that.find('.field-name-field-type input').trigger('change');
            that.find('select:visible').trigger('change');
        });
    };

    jQuery('#incentives .row').goalsFunc();

});

