<?php
drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/invite.js');
?>
<div class="invite-form">
    <h2>Share Study Sauce with a friend.</h2>
    <div class="form-item webform-component webform-component-textfield webform-component--student-first-name">
        <label>First name</label>
        <input type="text" name="invite-first" size="60" maxlength="128" class="form-text required"
            value="">
    </div>
    <div class="form-item webform-component webform-component-textfield webform-component--student-last-name">
        <label>Last name</label>
        <input type="text" name="invite-last" size="60" maxlength="128" class="form-text required"
            value="">
    </div>
    <div class="form-item webform-component webform-component-email">
        <label>Friend's email</label>
        <input class="email form-text form-email required" type="email" name="invite-email" size="60"
            value="">
    </div>
    <div class="highlighted-link">
        <a href="#invite-send" class="more">Invite</a></div>
    <h3 class="students_only">Recommend to your classmates</h3>
    <p style="margin-bottom:0;" class="like-us">
        <a target="_blank" href="https://www.facebook.com/sharer/sharer.php?u=https://www.studysauce.com"> &nbsp;</a>
        <a href="https://plus.google.com/share?url=https://www.studysauce.com">&nbsp;</a>
        <a href="https://twitter.com/intent/tweet?source=webclient&text=Check+out+www.StudySauce.com+for+your+online+study+assistant.">
            &nbsp;</a></p>
</div>

