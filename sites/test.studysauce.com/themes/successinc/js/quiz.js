
jQuery(document).ready(function() {

    var quiz = jQuery('#study-quiz');


    jQuery('body').on('click', 'a[href="#study-quiz"]', function (evt) {
        evt.preventDefault();
        jQuery('#home').addClass('study-quiz-only').scrollintoview();
    });

    quiz.on('click', 'a[href="#retry"]', function (evt) {
        evt.preventDefault();
        quiz.children('.webform-confirmation').hide();
        quiz.children('div').first().show();
    });

    quiz.on('click', 'a[href="#submit-quiz"]', function (evt) {
        evt.preventDefault();

        jQuery('#home-tasks-quiz, #home-quiz').attr('checked', 'checked');
        var home = jQuery('#home');
        if(home.find('input[type="checkbox"]:checked').length == home.find('input[type="checkbox"]').length - 1)
            jQuery('#home-tasks-checklist').attr('checked', 'checked');

        jQuery.ajax({
            url: 'quiz/save',
            type: 'POST',
            dataType: 'json',
            data: {
                place: quiz.find('input[name="submitted[study_place]"]:checked').val(),
                underlining: quiz.find('input[name="submitted[underlining]"]:checked').val(),
                same_subject: quiz.find('input[name="submitted[same_subject]"]:checked').val(),
                laying_down: quiz.find('input[name="submitted[laying_down]"]:checked').val(),
                longer_sessions: quiz.find('input[name="submitted[longer_sessions]"]:checked').val()
            },
            success: function (data) {
                window.location = '/#home';

                // display results
                quiz.find('.webform-confirmation').remove();
                quiz.append(jQuery(data.results));
                quiz.children('div').first().hide();

                // reset checks
                quiz.find('input[name="submitted[study_place]"]:checked, input[name="submitted[underlining]"]:checked, ' +
                    'input[name="submitted[same_subject]"]:checked' +
                    'input[name="submitted[laying_down]"]:checked, input[name="submitted[longer_sessions]"]:checked').prop('checked', false);
            }
        });

    });

});


