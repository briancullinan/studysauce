<h2>Enter important dates and we will send you reminders</h2>
<button class="field-add-more-submit ajax-processed" name="field_reminders_add_more" value="Add new"
        onclick="jQuery('#deadlines .row').first().addClass('edit'); jQuery(this).hide(); jQuery('#deadlines').addClass('edit-date-only').scrollintoview(); ">
    Add <span>&nbsp;</span> new
</button>

<div class="row invalid" id="new-dates-row">
    <div class="field-type-text field-name-field-class-name field-widget-text-textfield form-wrapper">
        <div class="form-item form-type-select">
            <label>&nbsp;</label>
            <select class="text-full form-select" id="dates-class-name" name="dates-class-name" size="1">
                <option value="_none">- Select a class -</option>
                <?php
                $classes = _studysauce_get_schedule_classes();

                foreach ($classes as $i => $c) {
                    ?>
                    <option value="<?php print htmlspecialchars($c, ENT_QUOTES); ?>"><?php print $c; ?></option><?php
                }
                ?>
                <option value="Nonacademic">Nonacademic</option>
            </select>
        </div>
    </div>
    <div class="field-type-text field-name-field-assignment field-widget-text-textfield form-wrapper">
        <div class="form-item form-type-textfield">
            <label for="dates-assignment">Assignment </label>
            <input class="text-full form-text jquery_placeholder-processed" placeholder="Paper, exam, project, etc."
                   type="text" id="dates-assignment" name="dates-assignment" value="" size="60" maxlength="255">
        </div>
    </div>
    <div class="field-type-list-integer field-name-field-reminder field-widget-options-buttons form-wrapper">
        <div class="form-item form-type-checkboxes">
            <label>Reminders </label>
            <div class="form-checkboxes">
                <div class="form-item form-type-checkbox">
                    <input type="checkbox" id="dates-reminder-1209600"
                           name="dates-reminder-1209600" value="1209600"
                           class="form-checkbox">
                    <label class="option" for="dates-reminder-1209600">2 wk </label>
                </div>
                <div class="form-item form-type-checkbox">
                    <input type="checkbox" id="dates-reminder-604800"
                           name="dates-reminder-604800" value="604800"
                           class="form-checkbox">
                    <label class="option" for="dates-reminder-604800">1 wk </label>
                </div>
                <div class="form-item form-type-checkbox">
                    <input type="checkbox" id="dates-reminder-345600"
                           name="dates-reminder-345600" value="345600"
                           class="form-checkbox">
                    <label class="option" for="dates-reminder-345600">4 days </label>
                </div>
                <div class="form-item form-type-checkbox">
                    <input type="checkbox" id="dates-reminder-172800"
                           name="dates-reminder-172800" value="172800"
                           class="form-checkbox">
                    <label class="option" for="dates-reminder-172800">2 days </label>
                </div>
                <div class="form-item form-type-checkbox">
                    <input type="checkbox" id="dates-reminder-86400"
                           name="dates-reminder-86400" value="86400"
                           class="form-checkbox">
                    <label class="option" for="dates-reminder-86400">1 day </label>
                </div>
            </div>
        </div>
    </div>
    <div class="field-type-datetime field-name-field-due-date field-widget-date-popup form-wrapper">
        <div class="form-item form-type-date-popup">
            <div class="date-padding">
                <div class="form-item form-type-textfield">
                    <label for="dates-due">Due date </label>
                    <input class="date-clear form-text jquery_placeholder-processed"
                           placeholder="Enter date"
                           type="text"
                           id="dates-due"
                           name="dates-due" value=""
                           size="20" maxlength="30">
                </div>
            </div>
        </div>
    </div>
    <div class="field-type-number-integer field-name-field-percent field-widget-number form-wrapper">
        <div class="form-item form-type-textfield">
            <label for="dates-percent">% of grade </label>
            <input type="text" id="dates-percent"
                   name="dates-percent" value="0" size="12" maxlength="10"
                   class="form-text">
        </div>
    </div>
    <div class="field-type-list-boolean field-name-field-completed field-widget-options-onoff form-wrapper">
        <div class="form-item form-type-checkbox">
            <input type="checkbox" id="dates-completed" name="dates-completed" value="1" class="form-checkbox">
            <label class="option" for="dates-completed">Completed </label>
        </div>
    </div>
    <a href="#cancel-dates" class="more">Cancel</a>
    <div class="highlighted-link">
        <a href="#save-dates" class="more">Save</a>
    </div>
</div>
<?php
global $user;
$query = new EntityFieldQuery();
$entities = $query->entityCondition('entity_type', 'node')
    ->propertyCondition('type', 'key_dates')
    ->propertyCondition('title', $user->mail)
    ->propertyCondition('status', 1)
    ->range(0, 1)
    ->execute();

$rebuild = false;
if (!empty($entities['node'])) {
    $nodes = array_keys($entities['node']);
    $nid = array_shift($nodes);
    $node = node_load($nid);
    
    if (isset($node->field_reminders[LANGUAGE_NONE][0]['value']))
    {
        $entities = array();
        foreach($node->field_reminders[LANGUAGE_NONE] as $i => $reminder)
        {
            $eid = $reminder['value'];
            $entity = entity_load('field_collection_item', array($eid));
            if (!empty($entity))
                $entities[$eid] = $entity[$eid];
        }
        uasort($entities, function ($a, $b) { return strtotime($a->field_due_date['und'][0]['value']) - strtotime($b->field_due_date['und'][0]['value']); });
        $headStr = '';
        $first = true;
        foreach ($entities as $eid => $reminder)
        {
            $time = strtotime($reminder->field_due_date['und'][0]['value']);
            if($headStr != date('j F', $time))
            {
                $headStr = date('j F', $time);
                ?><div class="head"><?php print $headStr; ?></div><?
            }
            $reminders = array_map(function ($x) { return $x['value']; }, $reminder->field_reminder['und']);
            $classI = array_search($reminder->field_class_name['und'][0]['value'], array_values($classes));
            ?>
            <div class="row <?php print ($first && !($first = false) ? 'first' : ''); ?>" id="eid-<?php print $eid; ?>">
                <div class="field-type-text field-name-field-class-name field-widget-text-textfield form-wrapper">
                    <div class="read-only"><span class="class<?php print $classI; ?>">&nbsp;</span><?php print htmlspecialchars($reminder->field_class_name['und'][0]['value'], ENT_QUOTES); ?></div>
                    <div class="form-item form-type-select">
                        <label>&nbsp;</label>
                        <select class="text-full form-select" name="dates-class-name" size="1">
                            <option value="_none">- Select a class -</option>
                            <?php
                            $classes = _studysauce_get_schedule_classes();

                            foreach ($classes as $j => $c) {
                                ?>
                                <option value="<?php print htmlspecialchars($c, ENT_QUOTES); ?>" <?php print $reminder->field_class_name['und'][0]['value'] == $c ? 'selected' : ''; ?>><?php print htmlspecialchars($c, ENT_QUOTES); ?></option><?php
                            }
                            ?>
                            <option value="Nonacademic" <?php print $reminder->field_class_name['und'][0]['value'] == 'Nonacademic' ? 'selected' : ''; ?>>Nonacademic</option>
                        </select>
                    </div>
                </div>
                <div class="field-type-text field-name-field-assignment field-widget-text-textfield form-wrapper">
                    <div class="read-only"><label>Assignment</label><?php print htmlspecialchars($reminder->field_assignment['und'][0]['value'], ENT_QUOTES); ?></div>
                    <div class="form-item form-type-textfield">
                        <label for="edit-field-reminders-und-1-field-assignment-und-0-value">Assignment </label>
                        <input class="text-full form-text jquery_placeholder-processed" placeholder="Paper, exam, project, etc."
                               type="text" name="dates-assignment" value="<?php print htmlspecialchars($reminder->field_assignment['und'][0]['value'], ENT_QUOTES); ?>" size="60" maxlength="255">
                    </div>
                </div>
                <div class="field-type-list-integer field-name-field-reminder field-widget-options-buttons form-wrapper">
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
                <div class="field-type-datetime field-name-field-due-date field-widget-date-popup form-wrapper">
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
                <div class="field-type-number-integer field-name-field-percent field-widget-number form-wrapper">
                    <div class="read-only"><label>% of grade</label><?php print $reminder->field_percent['und'][0]['value']; ?>%</div>
                    <div class="form-item form-type-textfield">
                        <label for="dates-percent">% of grade </label>
                        <input type="text" name="dates-percent" value="<?php print $reminder->field_percent['und'][0]['value']; ?>" size="12" maxlength="10" class="form-text">
                    </div>
                </div>
                <div class="field-type-list-boolean field-name-field-completed field-widget-options-onoff form-wrapper">
                    <div class="read-only"><a href="#edit-reminder">&nbsp;</a><a href="#remove-reminder">&nbsp;</a></div>
                    <div class="form-item form-type-checkbox">
                        <input type="checkbox" name="dates-completed" value="1" class="form-checkbox">
                        <label class="option" for="dates-completed">Completed </label>
                    </div>
                </div>
                <a href="#cancel-dates" class="more">Cancel</a>
                <div class="highlighted-link">
                    <a href="#save-dates" class="more">Save</a>
                </div>
            </div>
        <?php

        }
    }
}

?>
<div id="empty-dates">
    <a href="#schedule"><h2>Click here to set up your class schedule and get started.</h2><small>Then set important dates on the Reminders tab.</small></a>
</div>
<?php

if (count($classes) == 0) :
    ?>
    <script type="text/javascript">
        jQuery(document).ready(function () {
            jQuery('#deadlines,#checkin').addClass('empty edit-schedule');
        });
    </script>
<?php endif; ?>
<p style="clear: both;margin:0;"><a href="#schedule"><span>Edit schedule</span></a></p>