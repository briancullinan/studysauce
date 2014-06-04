
jQuery(document).ready(function($) {

    var importTab = jQuery('#import');

    jQuery.fn.importFunc = function () {
        jQuery(this).each(function () {
            var that = jQuery(this),
                isValid = true;

            if(that.find('.field-name-field-first-name input').val().trim() == '' ||
                that.find('.field-name-field-last-name input').val().trim() == '' ||
                that.find('.field-name-field-email input').val().trim() == '')
                isValid = false;

            if(isValid)
                that.removeClass('invalid').addClass('valid');
            else
                that.removeClass('valid').addClass('invalid');
        });
        if(importTab.find('.row.edit.valid').length == 0)
            importTab.removeClass('valid').addClass('invalid');
        else
            importTab.removeClass('invalid').addClass('valid');
    };

    var rowImport = function (append)
        {
            this.forEach(function (x) {
                // parse first last and email
                var parser = (/(.+?)\s*[\t,]\s*(.+?)\s*[\t,]\s*(.+?)\s*(\t|,|$)\s*/ig).exec(x);
                if(x.trim() == '' || parser == null)
                    return true;

                var count = importTab.find('.row').length,
                    addUser = importTab.find('#add-user-row').last(),
                    newUser = addUser.clone().attr('id', '').addClass('edit');
                if(append != null)
                    newUser.appendTo(append);
                else
                    newUser.insertBefore(addUser);
                if(count == 1)
                    newUser.addClass('first');
                newUser.find('input[type="checkbox"], input[type="radio"]').each(function () {
                    var that = jQuery(this),
                        oldId = that.attr('id');
                    that.attr('id', oldId + count);
                    if(that.is('[type="radio"]'))
                        that.attr('name', that.attr('name') + count);
                    newUser.find('label[for="' + oldId + '"]').attr('for', oldId + count);
                });

                // fill in values automatically
                newUser.find('.field-name-field-first-name input').val(parser[1]);
                newUser.find('.field-name-field-last-name input').val(parser[2]);
                newUser.find('.field-name-field-email input').val(parser[3]);

                newUser.importFunc();
                importTab.addClass('edit-user-only').scrollintoview();
            });
        },
        previewTimeout = null,
        previewImport = function () {
            // select the first couple rows or limit to 1000 characters
            var rows;
            var first1000 = /[\s\S]{0,1000}/i;
            var match = first1000.exec(importTab.find('textarea').val());
            var preview = importTab.find('fieldset');
            preview.find('.row').remove();
            if (match != null) {
                rows = match[0].split((/\s*\n\s*/ig));
            } else {
                rows = []
            }
            rowImport.apply(rows, preview);
        };

    importTab.on('mousedown', 'textarea', function () {
        if(previewTimeout != null)
            clearTimeout(previewTimeout);
        previewTimeout = setTimeout(previewImport, 1000);
    });
    importTab.on('mouseup', 'textarea', function () {
        if(previewTimeout != null)
            clearTimeout(previewTimeout);
        previewTimeout = setTimeout(previewImport, 1000);
    });
    importTab.on('change', 'textarea', function () {
        if(previewTimeout != null)
            clearTimeout(previewTimeout);
        previewTimeout = setTimeout(previewImport, 1000);
    });

    importTab.on('click', 'a[href="#import-group"]', function (evt) {
        evt.preventDefault();
        var rows = importTab.find('textarea').val().split((/\s*\n\s*/ig));
        importTab.find('fieldset').find('.row').remove();
        rowImport.apply(rows);
        importTab.find('textarea').val('');
    });

    importTab.on('click', 'a[href="#add-user"]', function (evt) {
        evt.preventDefault();
        var count = importTab.find('.row').length,
            addUser = importTab.find('#add-user-row').last(),
            newUser = addUser.clone().attr('id', '').addClass('edit').insertBefore(addUser);
        if(count == 1)
            newUser.addClass('first');
        newUser.find('input[type="checkbox"], input[type="radio"]').each(function () {
            var that = jQuery(this),
                oldId = that.attr('id');
            that.attr('id', oldId + count);
            if(that.is('[type="radio"]'))
                that.attr('name', that.attr('name') + count);
            newUser.find('label[for="' + oldId + '"]').attr('for', oldId + count);
        });
        newUser.importFunc();
        importTab.addClass('edit-user-only');
    });

    if(importTab.find('.row').not('#add-user-row').length == 0)
        importTab.find('a[href="#add-user"]').first().trigger('click').trigger('click');

    importTab.on('click', 'a[href="#save-group"]', function (evt) {
        evt.preventDefault();
        var users = [],
            rows = importTab.find('.row.edit.valid').not('fieldset .row');
        rows.each(function () {
            var that = jQuery(this);
            users[users.length] = {
                first: that.find('.field-name-field-first-name input').val(),
                last: that.find('.field-name-field-last-name input').val(),
                email: that.find('.field-name-field-email input').val()
            };
        });
        jQuery.ajax({
            url: '/group/save',
            type: 'POST',
            dataType: 'json',
            data: {
                users: users
            },
            success: function ()
            {
                // clear rows
                importTab.find('.row').not('#add-user-row').remove();
            }
        });
    });

});