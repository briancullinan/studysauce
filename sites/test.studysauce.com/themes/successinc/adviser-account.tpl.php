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
<div class="plupload" id="account-plupload">
    <div class="plup-list-wrapper">
        <ul class="plup-list clearfix ui-sortable">
            <?php if(isset($user->picture) && !empty($user->picture)):
                $file = !is_object($user->picture) ? ($user->picture = file_load($user->picture)) : $user->picture;
                ?>
                <li>
                    <div class="plup-thumb-wrapper">
                        <img src="<?php print image_style_url('achievement', $file->uri); ?>" title="">
                    </div>
                    <a class="plup-remove-item"></a>
                    <input type="hidden" name="account-plupload[0][fid]" value="<?php print $file->fid; ?>">
                    <input type="hidden" name="account-plupload[0][weight]" value="0">
                    <input type="hidden" name="account-plupload[0][rename]" value="<?php print $file->filename; ?>">
                </li>
            <?php else: ?>
                <img src="/<?php print drupal_get_path('theme', 'successinc'); ?>/images/empty-photo.png" height="200" width="200" alt="Upload" />
            <?php endif; ?>
        </ul>
    </div>
    <div class="plup-filelist" id="account-plupload-filelist">
        <table>
            <tbody>
            <tr class="plup-drag-info">
                <td>
                    <div class="drag-main">Upload photo of yourself</div>
                    <div class="drag-more">
                        <div>You can upload up to <strong>1</strong> files.</div>
                        <div>Allowed files types: <strong>png gif jpg jpeg</strong>.</div>
                    </div>
                </td>
            </tr>
            </tbody>
        </table>
    </div>
    <div class="plup-bar clearfix">
        <input type="hidden" id="account-upload-path" value="<?php print url('user/plup', array('query' => array('plupload_token' => drupal_get_token('plupload-handle-uploads')))); ?>" />
        <a href="#account-select" class="plup-select" id="account-plupload-select">Add</a>
        <a hre="#account-upload" class="plup-upload" id="account-plupload-upload">Upload</a>
        <div class="plup-progress"></div>
    </div>
</div>
<br />
<div class="highlighted-link form-actions form-wrapper">
    <!--<a href="#cancel-account">Delete your account</a> --><a class="more form-submit ajax-processed" href="#save-account">Save</a>
</div>
<?php
$lastOrder = _studysauce_orders_by_uid($user->uid);
$groups = og_get_groups_by_user();
$advised = (in_array('adviser', $user->roles) || in_array('master adviser', $user->roles)) ||
    isset($groups['node']);
?>
<div class="form-item form-type-radios field-name-account-type">
    <label>Account type:</label>
    <div class="form-checkboxes">
        <input readonly="readonly" type="radio" value="free" name="account-type" id="account-type-free" <?php print(!$lastOrder && !$advised ? 'checked="checked"' : ''); ?> />
        <label for="account-type-free">Free</label>
        <input readonly="readonly" type="radio" value="monthly" name="account-type" id="account-type-monthly" <?php print(isset($lastOrder) && is_object($lastOrder) && floatval($lastOrder->order_total) == 9.99000 ? 'checked="checked"' : ''); ?> />
        <label for="account-type-monthly">Monthly</label>
        <input readonly="readonly" type="radio" value="yearly" name="account-type" id="account-type-yearly" <?php print($advised || (isset($lastOrder) && is_object($lastOrder) && floatval($lastOrder->order_total)) > 9.99000 ? 'checked="checked"' : ''); ?> />
        <label for="account-type-yearly">Annual</label>
    </div>
</div>
<div class="form-item form-type-radios field-name-account-renewal">
    <label>Next renewal:</label>
    <label><?php print ($lastOrder
            ? (floatval($lastOrder->order_total) == 9.99000
                ? (date('m', intval($lastOrder->created)) == 12
                    ? ((date('Y', intval($lastOrder->created)) + 1) . '-1-' . date('-d', intval($lastOrder->created)))
                    : (date('Y-', intval($lastOrder->created)) . (date('m', intval($lastOrder->created)) + 1) . '-' . date('d', intval($lastOrder->created))))
                : ((date('Y', intval($lastOrder->created)) + 1) . '-' . date('m-d', intval($lastOrder->created))))
            : 'N/A'); ?></label>
</div>
<br />
<?php
$lastOrder = _studysauce_orders_by_uid($user->uid);
$groups = og_get_groups_by_user();
if(!isset($groups['node']) && !$lastOrder): ?>
<div class="highlighted-link form-actions form-wrapper"><a href="/buy" class="more">Upgrade</a></div>
<?php endif; ?>


