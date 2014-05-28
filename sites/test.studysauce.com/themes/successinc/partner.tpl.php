<?php
drupal_add_css(drupal_get_path('theme', 'successinc') .'/partner.css');
drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/partner.js');

global $user;
$user = user_load($user->uid);
if(isset($user->field_partners['und'][0]['value']))
{
    $partner = entity_load('field_collection_item', array($user->field_partners['und'][0]['value']));
    $partner = $partner[$user->field_partners['und'][0]['value']];
    $permissions = isset($partner->field_permissions['und']) && is_array($partner->field_permissions['und'])
        ? array_map(function ($x) { return $x['value']; }, $partner->field_permissions['und'])
        : array();
}

?>
<h2>Choosing an accountability partner can be invaluable to achieving your goals</h2>
<br />
<div class="partner-setup">
    <h3>I am accountable to:</h3>

    <div class="plupload" id="partner-plupload">
        <div class="plup-list-wrapper">
            <ul class="plup-list clearfix ui-sortable">
                <?php if(isset($partner->field_partner_photo['und'][0]['fid'])):
                    $file = file_load($partner->field_partner_photo['und'][0]['fid']);
                    ?>
                    <li class="ui-state-default">
                        <div class="plup-thumb-wrapper">
                            <img src="<?php print image_style_url('achievement', $file->uri); ?>" title="">
                        </div>
                        <a class="plup-remove-item"></a>
                        <input type="hidden" name="partner-plupload[0][fid]" value="<?php print $file->fid; ?>">
                        <input type="hidden" name="partner-plupload[0][weight]" value="0">
                        <input type="hidden" name="partner-plupload[0][rename]" value="<?php print $file->filename; ?>">
                    </li>
                <?php else: ?>
                <img src="/<?php print drupal_get_path('theme', 'successinc'); ?>/images/empty-photo.png" height="200" width="200" alt="Upload" />
                <?php endif; ?>
            </ul>
        </div>
        <div class="plup-filelist" id="partner-plupload-filelist">
            <table>
                <tbody>
                <tr class="plup-drag-info">
                    <td>
                        <div class="drag-main">Upload photo of your partner</div>
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
            <input type="hidden" id="partner-upload-path" value="<?php print url('node/plup/partner', array('query' => array('plupload_token' => drupal_get_token('plupload-handle-uploads')))); ?>" />
            <a href="#partner-select" class="plup-select" id="partner-plupload-select">Add</a>
            <a hre="#partner-upload" class="plup-upload" id="partner-plupload-upload">Upload</a>
            <div class="plup-progress"></div>
        </div>
    </div>

    <div class="partner-invite">
        <div
            class="form-item webform-component webform-component-textfield">
            <input type="text" id="partner-first" name="partner-first"
                   value="<?php print (isset($partner->field_first_name['und'][0]['value']) ? $partner->field_first_name['und'][0]['value'] : ''); ?>" size="60" maxlength="128" class="form-text required" placeholder="First name">
        </div>
        <div
            class="form-item webform-component webform-component-textfield">
            <input type="text" id="partner-last" name="partner-last"
                   value="<?php print (isset($partner->field_last_name['und'][0]['value']) ? $partner->field_last_name['und'][0]['value'] : ''); ?>"
                   size="60" maxlength="128" class="form-text required" placeholder="Last name">
        </div>
        <div class="form-item webform-component webform-component-email">
            <input class="email form-text form-email required" type="email" id="partner-email"
                   value="<?php print (isset($partner->field_email['und'][0]['value']) ? $partner->field_email['und'][0]['value'] : ''); ?>"
                   name="partner-email" size="60" placeholder="Email address">
        </div>
        <div class="highlighted-link form-actions">
            <a href="#partner-save" class="webform-submit button-primary more form-submit ajax-processed">Save</a></div>
    </div>

    <h3>My partner is allowed to see:</h3>
    <ul class="partner-permissions">
        <li><input type="checkbox" value="goals" id="partner-goals" name="partner-goals"
                <?php print (isset($permissions) && in_array('goals', $permissions) ? 'checked' : 'unchecked'); ?> />
            <label for="partner-goals">My goals</label></li>
        <li><input type="checkbox" value="metrics" id="partner-metrics" name="partner-metrics"
                <?php print (isset($permissions) && in_array('metrics', $permissions) ? 'checked' : 'unchecked'); ?> />
            <label for="partner-metrics">My study metrics</label></li>
        <li><input type="checkbox" value="deadlines" id="partner-deadlines" name="partner-deadlines"
                <?php print (isset($permissions) && in_array('deadlines', $permissions) ? 'checked' : 'unchecked'); ?> />
            <label for="partner-deadlines">My deadlines</label></li>
        <li><input type="checkbox" value="uploads" id="partner-uploads" name="partner-uploads"
                <?php print (isset($permissions) && in_array('uploads', $permissions) ? 'checked' : 'unchecked'); ?> />
            <label for="partner-uploads">My uploaded content <sup class="premium">Premium</sup></label></li>
        <li><input type="checkbox" value="plan" id="partner-plan" name="partner-plan"
                <?php print (isset($permissions) && in_array('plan', $permissions) ? 'checked' : 'unchecked'); ?> />
            <label for="partner-plan">My study plan <sup class="premium">Premium</sup></label></li>
        <li><input type="checkbox" value="profile" id="partner-profile" name="partner-profile"
                <?php print (isset($permissions) && in_array('profile', $permissions) ? 'checked' : 'unchecked'); ?> />
            <label for="partner-profile">My study profiles <sup class="premium">Premium</sup></label></li>
    </ul>
</div>
<div class="partner-faqs">
    <h3>FAQs:</h3>
    <h4>Why do I need an accountability partner?</h4>
    <p>
        Research shows that simply writing down your goals makes you more likely to achieve them.  Having an accountability partner takes it to a new level.  We all have ups and downs in school and finding someone to help motivate and challenge you along the way can be invaluable.
    </p>
    <h4>How do I choose an accountability partner?</h4>
    <p>
        An accountability partner is someone that will keep you on track to achieve your goals.  Here are some attributes to consider as you decide.  Choose someone that:
    </p>
    <ul>
        <li>Will challenge you (you will need more than just encouragement)</li>
        <li>Will celebrate your successes with you</li>
        <li>Is invested in your education</li>
        <li>You trust</li>
    </ul>
    <p>Take a few minutes to think about who best fits this description.  Sometimes a parent or best friend are not your best options.  Maybe some other family member, classmate, or even a non-family mentor can be the ideal choice.</p>
    <h4>Now that I have chosen my accountability partner, what should I do?</h4>
    <p>Communication is the key!  Outline your expectations and ask to be held accountable.  Set up regular check-ins (try to talk at least once every week).  Be transparent about your struggles and your successes during the conversations.</p>
    <h4>Can I change my accountability partner in Study Sauce?</h4>
    <p>Sure you can.  You can change your accountability partner or what they can see at any time.  Just use the edit function next to the photograph on the Accountability partner tab.</p>
</div>