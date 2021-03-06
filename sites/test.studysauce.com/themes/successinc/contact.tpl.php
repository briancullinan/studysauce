<?php
global $user;
$account = user_load($user->uid);
?>
<h2>Contact us</h2>
<p>If you have any questions at all, please contact us. &nbsp;We would love to hear from you! &nbsp;We want to help you to get the most out of your study time and your comments and feedback are important to us.</p>
<div class="form-item webform-component webform-component-textfield webform-component--your-name">
    <label for="edit-submitted-your-name">Your name <span class="form-required" title="This field is required.">*</span></label>
    <input type="text" id="edit-submitted-your-name" name="submitted[your_name]" value="<?php print (isset($account->field_first_name['und'][0]['value']) ? $account->field_first_name['und'][0]['value'] : ''); ?> <?php print (isset($account->field_last_name['und'][0]['value']) ? $account->field_last_name['und'][0]['value'] : ''); ?>" size="60" maxlength="128" class="form-text required">
</div>
<div class="form-item webform-component webform-component-email webform-component--your-email">
    <label for="edit-submitted-your-email">Your email <span class="form-required" title="This field is required.">*</span></label>
    <input class="email form-text form-email required" type="email" id="edit-submitted-your-email" name="submitted[your_email]" value="<?php print ($account->uid > 0 ? $account->mail : ''); ?>" size="60">
</div>
<div class="form-item webform-component webform-component-textarea webform-component--message">
    <label for="edit-submitted-message">Message <span class="form-required" title="This field is required.">*</span></label>
    <div class="form-textarea-wrapper resizable textarea-processed resizable-textarea">
        <textarea id="edit-submitted-message" name="submitted[message]" cols="60" rows="4" class="form-textarea required"></textarea>
    </div>
</div>
<div class="highlighted-link">
    <a href="#submit-contact" class="webform-submit button-primary more form-submit">Send</a>
</div>