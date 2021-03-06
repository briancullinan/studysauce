<?php
drupal_add_css(drupal_get_path('theme', 'successinc') .'/deadlines.css');
drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/dates.js');
?>
<h2>Upcoming deadlines</h2>

<?php
if(!isset($account))
{
    global $user;
    $account = user_load($user->uid);
}
$query = new EntityFieldQuery();
$entities = $query->entityCondition('entity_type', 'node')
    ->propertyCondition('type', 'key_dates')
    ->propertyCondition('title', $account->mail)
    ->propertyCondition('status', 1)
    ->range(0, 1)
    ->execute();

$rebuild = false;
if (!empty($entities['node']))
{
    ?>
    <div class="sort-by">
        <label>Sort by: </label>
        <input type="radio" id="deadlines-by-date" name="deadlines-by" value="date" checked="checked"><label for="deadlines-by-date">Date</label>&nbsp;
        <input type="radio" id="deadlines-by-class" name="deadlines-by" value="class"><label for="deadlines-by-class">Class</label>
        <input type="checkbox" id="deadlines-historic"><label for="deadlines-historic">Past deadlines</label>
    </div>
    <?php
    $nodes = array_keys($entities['node']);
    $nid = array_shift($nodes);
    $node = node_load($nid);

    if (isset($node->field_reminders[LANGUAGE_NONE][0]['value']))
    {
        $classes = _studysauce_get_schedule_classes($account);
        $entities = array();
        foreach($node->field_reminders[LANGUAGE_NONE] as $i => $reminder)
        {
            $eid = $reminder['value'];
            $entity = entity_load('field_collection_item', array($eid));
            if (!empty($entity))
                $entities[$eid] = $entity[$eid];
        }
        uasort($entities, function ($a, $b) {
            if(isset($a->field_due_date['und'][0]['value']) && isset($b->field_due_date['und'][0]['value']))
                return strtotime($a->field_due_date['und'][0]['value']) - strtotime($b->field_due_date['und'][0]['value']);
        });
        $headStr = '';
        $first = true;
        $firstVisible = true;
        foreach ($entities as $eid => $reminder)
        {
            if(!isset($reminder->field_due_date['und'][0]['value']))
                continue;
            $time = strtotime($reminder->field_due_date['und'][0]['value']);
            if($headStr != date('j F', $time))
            {
                $headStr = date('j F', $time);
                ?><div class="head <?php
            print ($time < strtotime(date('Y/m/d')) - 86400 ? 'hide' : ''); ?>"><?php print $headStr; ?></div><?
            }
            $reminders = array_map(function ($x) { return $x['value']; }, $reminder->field_reminder['und']);
            $classI = array_search($reminder->field_class_name['und'][0]['value'], array_values($classes));
            ?>
            <div class="row <?php
            print (($time < strtotime(date('Y/m/d')) - 86400 && $first && !($first = false)) ||
            ($time >= strtotime(date('Y/m/d')) - 86400 && $firstVisible && !($firstVisible = false)) ? 'first' : ''); ?> <?php
            print ($time < strtotime(date('Y/m/d')) - 86400 ? 'hide' : ''); ?>" id="eid-<?php print $eid; ?>">
                <div class="field-type-text field-name-field-class-name field-widget-text-textfield ">
                    <div class="read-only"><span class="class<?php print $classI; ?>">&nbsp;</span><?php print htmlspecialchars($reminder->field_class_name['und'][0]['value'], ENT_QUOTES); ?></div>
                    <div class="form-item form-type-select">
                        <label>&nbsp;</label>
                        <select class="text-full form-select" name="dates-class-name" size="1">
                            <option value="_none">- Select a class -</option>
                            <?php
                            foreach ($classes as $j => $c) {
                                ?>
                                <option value="<?php print htmlspecialchars($c, ENT_QUOTES); ?>" <?php print ($reminder->field_class_name['und'][0]['value'] == $c ? 'selected' : ''); ?>><?php print htmlspecialchars($c, ENT_QUOTES); ?></option><?php
                            }
                            ?>
                            <option value="Nonacademic" <?php print ($reminder->field_class_name['und'][0]['value'] == 'Nonacademic' ? 'selected' : ''); ?>>Nonacademic</option>
                        </select>
                    </div>
                </div>
                <div class="field-type-text field-name-field-assignment field-widget-text-textfield ">
                    <div class="read-only"><label>Assignment</label><?php print htmlspecialchars($reminder->field_assignment['und'][0]['value'], ENT_QUOTES); ?></div>
                    <div class="form-item form-type-textfield">
                        <label for="edit-field-reminders-und-1-field-assignment-und-0-value">Assignment </label>
                        <input class="text-full form-text jquery_placeholder-processed" placeholder="Paper, exam, project, etc."
                               type="text" name="dates-assignment" value="<?php print htmlspecialchars($reminder->field_assignment['und'][0]['value'], ENT_QUOTES); ?>" size="60" maxlength="255">
                    </div>
                </div>
                <div class="field-type-list-integer field-name-field-reminder field-widget-options-buttons ">
                    <div class="read-only"><label>Reminders</label>
                        <span class="<?php print (in_array(1209600, $reminders) ? 'checked' : 'unchecked'); ?>">2 wk</span>
                        <span class="<?php print (in_array(604800, $reminders) ? 'checked' : 'unchecked'); ?>">1 wk</span>
                        <span class="<?php print (in_array(345600, $reminders) ? 'checked' : 'unchecked'); ?>">4 days</span>
                        <span class="<?php print (in_array(172800, $reminders) ? 'checked' : 'unchecked'); ?>">2 days</span>
                        <span class="<?php print (in_array(86400, $reminders) ? 'checked' : 'unchecked'); ?>">1 day</span></div>
                    <div class="form-item form-type-checkboxes">
                        <label>Reminders </label>
                        <div class="form-checkboxes">
                            <div class="form-item form-type-checkbox">
                                <input type="checkbox" id="dates-reminder-1209600-<?php print $eid; ?>"
                                       name="dates-reminder-1209600" value="1209600"
                                       class="form-checkbox" <?php print (in_array(1209600, $reminders) ? 'checked="checked"' : ''); ?>>
                                <label class="option" for="dates-reminder-1209600-<?php print $eid; ?>">2 wk </label>
                            </div>
                            <div class="form-item form-type-checkbox">
                                <input type="checkbox" id="dates-reminder-604800-<?php print $eid; ?>"
                                       name="dates-reminder-604800" value="604800"
                                       class="form-checkbox" <?php print (in_array(604800, $reminders) ? 'checked="checked"' : ''); ?>>
                                <label class="option" for="dates-reminder-604800-<?php print $eid; ?>">1 wk </label>
                            </div>
                            <div class="form-item form-type-checkbox">
                                <input type="checkbox" id="dates-reminder-345600-<?php print $eid; ?>"
                                       name="dates-reminder-345600" value="345600"
                                       class="form-checkbox" <?php print (in_array(345600, $reminders) ? 'checked="checked"' : ''); ?>>
                                <label class="option" for="dates-reminder-345600-<?php print $eid; ?>">4 days </label>
                            </div>
                            <div class="form-item form-type-checkbox">
                                <input type="checkbox" id="dates-reminder-172800-<?php print $eid; ?>"
                                       name="dates-reminder-172800" value="172800"
                                       class="form-checkbox" <?php print (in_array(172800, $reminders) ? 'checked="checked"' : ''); ?>>
                                <label class="option" for="dates-reminder-172800-<?php print $eid; ?>">2 days </label>
                            </div>
                            <div class="form-item form-type-checkbox">
                                <input type="checkbox" id="dates-reminder-86400-<?php print $eid; ?>"
                                       name="dates-reminder-86400" value="86400"
                                       class="form-checkbox" <?php print (in_array(86400, $reminders) ? 'checked="checked"' : ''); ?>>
                                <label class="option" for="dates-reminder-86400-<?php print $eid; ?>">1 day </label>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="field-type-datetime field-name-field-due-date field-widget-date-popup ">
                    <div class="form-item form-type-date-popup">
                        <div class="date-padding">
                            <div class="form-item form-type-textfield">
                                <label for="dates-due">Due date </label>
                                <input class="date-clear form-text jquery_placeholder-processed"
                                       placeholder="Enter date"
                                       type="text"
                                       name="dates-due" value="<?php print date('m/d/Y', $time); ?>"
                                       size="20" maxlength="30">
                            </div>
                        </div>
                    </div>
                </div>
                <?php if($reminder->field_class_name['und'][0]['value'] != 'Nonacademic'): ?>
                    <div class="field-type-number-integer field-name-field-percent field-widget-number ">
                        <div class="read-only"><label>% of grade</label><?php print $reminder->field_percent['und'][0]['value']; ?>%</div>
                        <div class="form-item form-type-textfield">
                            <label for="dates-percent">% of grade </label>
                            <input type="text" name="dates-percent" value="<?php print $reminder->field_percent['und'][0]['value']; ?>" size="12" maxlength="10" class="form-text">
                        </div>
                    </div>
                <?php endif; ?>
                <div class="field-type-list-boolean field-name-field-completed field-widget-options-onoff ">
                    <div class="form-item form-type-checkbox">
                        <input type="checkbox" name="dates-completed" value="1" class="form-checkbox">
                        <label class="option" for="dates-completed">Completed </label>
                    </div>
                </div>
            </div>
        <?php

        }
    }
}
else
{
    ?><h3>Your student has not completed this section yet.</h3><?php
}
?>
<hr style="margin-top:20px;" />

