<?php
drupal_add_css(drupal_get_path('theme', 'successinc') .'/adviser-import.css');
drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/import.js');
?>
<h2>Invite students to Study Sauce</h2>
<h3>Enter their first name, last name, and email below to invite them to Study Sauce.</h3>
<div id="add-user-row" class="row edit invalid">
    <div class="field-name-field-first-name">
        <label>First name</label>
        <input class="text-full form-text" type="text" />
    </div>
    <div class="field-name-field-last-name">
        <label>Last name</label>
        <input class="text-full form-text" type="text" />
    </div>
    <div class="field-name-field-email">
        <label>Email</label>
        <input class="text-full form-text" type="text" />
    </div>
</div>
<h3>Your students will receive an invitation with a link that will finish setting up their account.</h3>
<p class="highlighted-link">
    <a href="#add-user" class="field-add-more-submit ajax-processed" name="field_reminders_add_more">
        Add <span>+</span> user
    </a>
    <a href="#save-group" class="more">Import</a>
</p>
<h3>Voila, you are connected.</h3>
<h2>Use our batch uploader</h2>
<textarea id="user-import" rows="8" placeholder="first,last,email&para;first,last,email"></textarea>

<fieldset id="user-preview">
    <legend>Preview</legend>
    <p class="highlighted-link" style="margin-bottom:0;">
        <a href="#import-group" class="more">Import batch</a>
    </p>
</fieldset>

