
jQuery(document).ready(function($) {

    $('.parent-student-selection input.form-radio').on('change', function () {
        $.ajax({
                   url: '/user/save',
                   type: 'POST',
                   dataType: 'json',
                   data: {
                       "parent-student" : $('.parent-student-selection input.form-radio:checked').val()
                   },
                   success: function (data) {
                       window.location = '/';
                   }
               });
        });
});

