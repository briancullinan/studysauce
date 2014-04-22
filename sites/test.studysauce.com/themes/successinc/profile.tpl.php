<?php
global $user;
$query = new EntityFieldQuery();
$nodes = $query->entityCondition('entity_type', 'node')
    ->propertyCondition('type', 'schedule')
    ->propertyCondition('title', isset($user->mail) ? $user->mail : '')
    ->propertyCondition('status', 1)
    ->range(0, 1)
    ->execute();
$node = new StdClass();
if (!empty($nodes['node'])) {
    $nodes = array_keys($nodes['node']);
    $nid = array_shift($nodes);
    $node = node_load($nid);
}
?>
<h1>Thank you</h1>
<h3>Before we build your study plan, we need to know a few things about you.</h3>
<div class="field-type-text field-name-field-university field-widget-text-textfield form-wrapper">
    <label for="schedule-university">School name</label>
    <input class="text-full form-text required" type="text" id="schedule-university" name="schedule-university"
           size="60" maxlength="255" value="<?php print (isset($node->field_university['und'][0]['value']) ? $node->field_university['und'][0]['value'] : ''); ?>">
</div>
<div class="field-type-list-text field-name-field-grades field-widget-options-buttons form-wrapper">
    <label>What kind of grades do you want?</label>
    <input type="radio" id="schedule-grades-as-only" name="field_grades" value="as_only" class="form-radio"
        <?php print (isset($node->field_grades['und'][0]['value']) && $node->field_grades['und'][0]['value'] == 'as_only' ? 'checked="checked"' : ''); ?>>
    <label class="option" for="schedule-grades-as-only">Nothing but As </label>
    <input type="radio" id="schedule-grades-has-life" name="field_grades" value="has_life" class="form-radio"
        <?php print (isset($node->field_grades['und'][0]['value']) && $node->field_grades['und'][0]['value'] == 'has_life' ? 'checked="checked"' : ''); ?>>
    <label class="option" for="schedule-grades-has-life">I want to do well, but don't want to live in the
        library </label>
</div>
<div class="field-type-list-text field-name-field-weekends field-widget-options-buttons form-wrapper">
    <label>How do your manage your weekends?</label>
    <input type="radio" id="schedule-weekends-hit-hard" name="field_weekends" value="hit_hard" class="form-radio"
        <?php print (isset($node->field_weekends['und'][0]['value']) && $node->field_weekends['und'][0]['value'] == 'hit_hard' ? 'checked="checked"' : ''); ?>>
    <label class="option" for="schedule-weekends-hit-hard">Hit hard, keep weeks open </label>
    <input type="radio" id="schedule-weekends-light-work" name="field_weekends" value="light_work" class="form-radio"
        <?php print (isset($node->field_weekends['und'][0]['value']) && $node->field_weekends['und'][0]['value'] == 'light_work' ? 'checked="checked"' : ''); ?>>
    <label class="option" for="schedule-weekends-light-work">Light work, focus during the week </label>
</div>
<div class="field-name-field-time-preference">
    <label>On a scale of 0-5 (5 being the best), rate how mentally sharp you feel during the following time
        periods:</label>

    <div class="field-type-list-integer field-name-field-6-am-11-am field-widget-options-buttons form-wrapper">
        <label>6 AM - 11 AM</label>

        <div class="form-radios">
            <input type="radio" id="schedule-6-am-11-am-0" name="field_6_am_11_am" value="0" class="form-radio"
                <?php print (isset($node->field_6_am_11_am['und'][0]['value']) && $node->field_6_am_11_am['und'][0]['value'] == 0 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-6-am-11-am-0">0 </label>
            <input type="radio" id="schedule-6-am-11-am-1" name="field_6_am_11_am" value="1" class="form-radio"
                <?php print (isset($node->field_6_am_11_am['und'][0]['value']) && $node->field_6_am_11_am['und'][0]['value'] == 1 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-6-am-11-am-1">1 </label>
            <input type="radio" id="schedule-6-am-11-am-2" name="field_6_am_11_am" value="2" class="form-radio"
                <?php print (isset($node->field_6_am_11_am['und'][0]['value']) && $node->field_6_am_11_am['und'][0]['value'] == 2 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-6-am-11-am-2">2 </label>
            <input type="radio" id="schedule-6-am-11-am-3" name="field_6_am_11_am" value="3" class="form-radio"
                <?php print (isset($node->field_6_am_11_am['und'][0]['value']) && $node->field_6_am_11_am['und'][0]['value'] == 3 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-6-am-11-am-3">3 </label>
            <input type="radio" id="schedule-6-am-11-am-4" name="field_6_am_11_am" value="4" class="form-radio"
                <?php print (isset($node->field_6_am_11_am['und'][0]['value']) && $node->field_6_am_11_am['und'][0]['value'] == 4 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-6-am-11-am-4">4 </label>
            <input type="radio" id="schedule-6-am-11-am-5" name="field_6_am_11_am" value="5" class="form-radio"
                <?php print (isset($node->field_6_am_11_am['und'][0]['value']) && $node->field_6_am_11_am['und'][0]['value'] == 5 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-6-am-11-am-5">5 </label>
        </div>
    </div>
    <div class="field-type-list-integer field-name-field-11-am-4-pm field-widget-options-buttons form-wrapper">
        <label for="schedule-11-am-4-pm">11 AM - 4 PM</label>

        <div class="form-radios">
            <input type="radio" id="schedule-11-am-4-pm-0" name="field_11_am_4_pm" value="0" class="form-radio"
                <?php print (isset($node->field_11_am_4_pm['und'][0]['value']) && $node->field_11_am_4_pm['und'][0]['value'] == 0 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-11-am-4-pm-0">0 </label>
            <input type="radio" id="schedule-11-am-4-pm-1" name="field_11_am_4_pm" value="1" class="form-radio"
                <?php print (isset($node->field_11_am_4_pm['und'][0]['value']) && $node->field_11_am_4_pm['und'][0]['value'] == 1 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-11-am-4-pm-1">1 </label>
            <input type="radio" id="schedule-11-am-4-pm-2" name="field_11_am_4_pm" value="2" class="form-radio"
                <?php print (isset($node->field_11_am_4_pm['und'][0]['value']) && $node->field_11_am_4_pm['und'][0]['value'] == 2 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-11-am-4-pm-2">2 </label>
            <input type="radio" id="schedule-11-am-4-pm-3" name="field_11_am_4_pm" value="3" class="form-radio"
                <?php print (isset($node->field_11_am_4_pm['und'][0]['value']) && $node->field_11_am_4_pm['und'][0]['value'] == 3 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-11-am-4-pm-3">3 </label>
            <input type="radio" id="schedule-11-am-4-pm-4" name="field_11_am_4_pm" value="4" class="form-radio"
                <?php print (isset($node->field_11_am_4_pm['und'][0]['value']) && $node->field_11_am_4_pm['und'][0]['value'] == 4 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-11-am-4-pm-4">4 </label>
            <input type="radio" id="schedule-11-am-4-pm-5" name="field_11_am_4_pm" value="5" class="form-radio"
                <?php print (isset($node->field_11_am_4_pm['und'][0]['value']) && $node->field_11_am_4_pm['und'][0]['value'] == 5 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-11-am-4-pm-5">5 </label>
        </div>
    </div>
    <div class="field-type-list-integer field-name-field-4-pm-9-pm field-widget-options-buttons form-wrapper">
        <label for="schedule-4-pm-9-pm">4 PM - 9 PM</label>

        <div class="form-radios">
            <input type="radio" id="schedule-4-pm-9-pm-0" name="field_4_pm_9_pm" value="0" class="form-radio"
                <?php print (isset($node->field_4_pm_9_pm['und'][0]['value']) && $node->field_4_pm_9_pm['und'][0]['value'] == 0 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-4-pm-9-pm-0">0 </label>
            <input type="radio" id="schedule-4-pm-9-pm-1" name="field_4_pm_9_pm" value="1" class="form-radio"
                <?php print (isset($node->field_4_pm_9_pm['und'][0]['value']) && $node->field_4_pm_9_pm['und'][0]['value'] == 1 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-4-pm-9-pm-1">1 </label>
            <input type="radio" id="schedule-4-pm-9-pm-2" name="field_4_pm_9_pm" value="2" class="form-radio"
                <?php print (isset($node->field_4_pm_9_pm['und'][0]['value']) && $node->field_4_pm_9_pm['und'][0]['value'] == 2 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-4-pm-9-pm-2">2 </label>
            <input type="radio" id="schedule-4-pm-9-pm-3" name="field_4_pm_9_pm" value="3" class="form-radio"
                <?php print (isset($node->field_4_pm_9_pm['und'][0]['value']) && $node->field_4_pm_9_pm['und'][0]['value'] == 3 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-4-pm-9-pm-3">3 </label>
            <input type="radio" id="schedule-4-pm-9-pm-4" name="field_4_pm_9_pm" value="4" class="form-radio"
                <?php print (isset($node->field_4_pm_9_pm['und'][0]['value']) && $node->field_4_pm_9_pm['und'][0]['value'] == 4 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-4-pm-9-pm-4">4 </label>
            <input type="radio" id="schedule-4-pm-9-pm-5" name="field_4_pm_9_pm" value="5" class="form-radio"
                <?php print (isset($node->field_4_pm_9_pm['und'][0]['value']) && $node->field_4_pm_9_pm['und'][0]['value'] == 5 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-4-pm-9-pm-5">5 </label>
        </div>
    </div>
    <div class="field-type-list-integer field-name-field-9-pm-2-am field-widget-options-buttons form-wrapper">
        <label for="schedule-9-pm-2-am">9 PM - 2 AM</label>

        <div class="form-radios">
            <input type="radio" id="schedule-9-pm-2-am-0" name="field_9_pm_2_am" value="0" class="form-radio"
                <?php print (isset($node->field_9_pm_2_am['und'][0]['value']) && $node->field_9_pm_2_am['und'][0]['value'] == 0 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-9-pm-2-am-0">0 </label>
            <input type="radio" id="schedule-9-pm-2-am-1" name="field_9_pm_2_am" value="1" class="form-radio"
                <?php print (isset($node->field_9_pm_2_am['und'][0]['value']) && $node->field_9_pm_2_am['und'][0]['value'] == 1 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-9-pm-2-am-1">1 </label>
            <input type="radio" id="schedule-9-pm-2-am-2" name="field_9_pm_2_am" value="2" class="form-radio"
                <?php print (isset($node->field_9_pm_2_am['und'][0]['value']) && $node->field_9_pm_2_am['und'][0]['value'] == 2 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-9-pm-2-am-2">2 </label>
            <input type="radio" id="schedule-9-pm-2-am-3" name="field_9_pm_2_am" value="3" class="form-radio"
                <?php print (isset($node->field_9_pm_2_am['und'][0]['value']) && $node->field_9_pm_2_am['und'][0]['value'] == 3 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-9-pm-2-am-3">3 </label>
            <input type="radio" id="schedule-9-pm-2-am-4" name="field_9_pm_2_am" value="4" class="form-radio"
                <?php print (isset($node->field_9_pm_2_am['und'][0]['value']) && $node->field_9_pm_2_am['und'][0]['value'] == 4 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-9-pm-2-am-4">4 </label>
            <input type="radio" id="schedule-9-pm-2-am-5" name="field_9_pm_2_am" value="5" class="form-radio"
                <?php print (isset($node->field_9_pm_2_am['und'][0]['value']) && $node->field_9_pm_2_am['und'][0]['value'] == 5 ? 'checked="checked"' : ''); ?>>
            <label class="option" for="schedule-9-pm-2-am-5">5 </label>
        </div>
    </div>
</div>
<p style="clear:both; margin-bottom:0; text-align: right;" class="highlighted-link">
    <a href="#save-profile" class="more">Save</a></p>
