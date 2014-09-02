<?php

drupal_add_css(drupal_get_path('theme', 'successinc') .'/account.css');
drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/account.js');
global $user;

?>
<h2>User settings</h2>
<div class="field-type-text field-name-field-first-name field-widget-text-textfield form-wrapper">
    <div class="form-item form-type-textfield">
        <label>First name:</label>
        <input class="text-full form-text required" type="text" name="account-first-name"
               value="<?php print (isset($user->field_first_name['und'][0]['value']) ? $user->field_first_name['und'][0]['value'] : ''); ?>" size="60" maxlength="255">
    </div>
</div>
<div class="field-type-text field-name-field-last-name field-widget-text-textfield form-wrapper">
    <div class="form-item form-type-textfield">
        <label>Last name:</label>
        <input class="text-full form-text required" type="text" name="account-last-name"
               value="<?php print (isset($user->field_first_name['und'][0]['value']) ? $user->field_last_name['und'][0]['value'] : ''); ?>" size="60" maxlength="255">
    </div>
</div>
<div class="form-item form-type-textfield form-item-mail">
    <label>E-mail address:</label>
    <input type="text" name="mail" value="<?php print (isset($user->mail) ? $user->mail : ''); ?>" size="60" maxlength="254"
           class="form-text required">
    <div class="description">All emails from Study Sauce will be sent to this address.</div>
</div>
<br />
<p style="margin-bottom:0;">Enter your current password to change Email or set a New Password.</p>

<div class="form-item form-type-password form-item-current-pass">
    <label>Current password:</label>
    <input autocomplete="off" type="password" name="current_pass" size="25" maxlength="128"
           class="form-text">
</div>
<div class="form-item form-type-password form-item-pass">
    <label>New password:</label>
    <input type="password" name="pass" size="25" maxlength="128" class="form-text">
</div>
<br />
<div class="highlighted-link form-actions form-wrapper">
    <!--<a href="#cancel-account">Delete your account</a> --><a class="more form-submit ajax-processed" href="#save-account">Save</a>
</div>
<?php
$lastOrder = _studysauce_orders_by_uid($user->uid);
$product = $lastOrder != null && is_array($lastOrder->products) ? array_pop($lastOrder->products) : null;
$attributes = $product != null && isset($product->data['attributes'])
    ? $product->data['attributes']
    : array();
$yearly = !empty($attributes) && ($attributes = array_pop($attributes)) == 'Yearly' || ($attributes = array_pop($attributes)) == 'Yearly';
$groups = og_get_groups_by_user();
$advised = (in_array('adviser', $user->roles) || in_array('master adviser', $user->roles)) || isset($groups['node']);
?>
<div class="form-item form-type-radios field-name-account-type">
    <label>Account type:</label>
    <div class="form-checkboxes">
        <input readonly="readonly" type="radio" value="free" name="account-type" id="account-type-free" <?php print(!$lastOrder && !$advised ? 'checked="checked"' : ''); ?> />
        <label for="account-type-free">Free</label>
        <input readonly="readonly" type="radio" value="monthly" name="account-type" id="account-type-monthly" <?php print(isset($lastOrder) && !$yearly ? 'checked="checked"' : ''); ?> />
        <label for="account-type-monthly">Monthly</label>
        <input readonly="readonly" type="radio" value="yearly" name="account-type" id="account-type-yearly" <?php print($advised || (isset($lastOrder) && $yearly) ? 'checked="checked"' : ''); ?> />
        <label for="account-type-yearly">Annual</label>
    </div>
</div>
<div class="form-item form-type-radios field-name-account-renewal">
    <label>Next renewal:</label>
    <label><?php print ($lastOrder
            ? (!$yearly
                ? (date('m', intval($lastOrder->created)) == 12
                    ? ((date('Y', intval($lastOrder->created)) + 1) . '-1-' . date('-d', intval($lastOrder->created)))
                    : (date('Y-', intval($lastOrder->created)) . (date('m', intval($lastOrder->created)) + 1) . '-' . date('d', intval($lastOrder->created))))
                : ((date('Y', intval($lastOrder->created)) + 1) . '-' . date('m-d', intval($lastOrder->created))))
            : 'N/A'); ?></label>
</div>
<br />
<?php
if(!isset($groups['node']) && !$lastOrder): ?>
<div class="highlighted-link form-actions form-wrapper"><a href="/cart/add/e-p13_q1_a4o14_s?destination=cart/checkout" class="more">Upgrade</a></div>
<?php endif; ?>


