<?php
drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/schedule.js');
drupal_add_css(drupal_get_path('theme', 'successinc') .'/schedule.css');
drupal_add_js(drupal_get_path('module', 'date') .'/date_popup/jquery.timeentry.pack.js');
drupal_add_library('system', 'ui.datepicker');
drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/jquery.scrollintoview.js');
drupal_add_js(drupal_get_path('theme', 'successinc') . '/js/jquery.pietimer.js');
drupal_add_css(drupal_get_path('theme', 'successinc') . '/css/pietimer.css');

if(drupal_get_path_alias(current_path()) == 'schedule' ||
    drupal_get_path_alias(current_path()) == 'schedule2')
    print theme('studysauce-funnel');

global $user;
$query = new EntityFieldQuery();
$nodes = $query->entityCondition('entity_type', 'node')
    ->propertyCondition('type', 'schedule')
    ->propertyCondition('title', isset($user->mail) ? $user->mail : '')
    ->propertyCondition('status', 1)
    ->range(0, 1)
    ->execute();
if (!empty($nodes['node'])) {
    $nodes = array_keys($nodes['node']);
    $nid = array_shift($nodes);
    $node = node_load($nid);
}
list($events, $classes, $others) = studysauce_get_events();
?>

<div class="building-schedule">
    <div class="middle-wrapper">
        <h2>Just a moment while we build your plan.
            <div class="timer"></div></h2>
    </div>
</div>

<h2>Enter your class below</h2>
<div class="field-type-text field-name-field-university field-widget-text-textfield form-wrapper">
    <label for="schedule-university">School name</label>
    <input class="text-full form-text required" type="text" id="schedule-university" name="schedule-university"
           size="60" maxlength="255" value="<?php print (isset($node->field_university['und'][0]['value']) ? $node->field_university['und'][0]['value'] : ''); ?>">
</div>

<div class="schedule">
    <?php
    if(drupal_get_path_alias(current_path()) == 'schedule2' ||
        drupal_get_path_alias(current_path()) == 'schedule' ||
        empty($classes))
    {
        for($i = 0; $i < 5; $i++)
        {
            if(count($classes) < 5)
            {
                $blank = new stdClass();
                $blank->field_class_name['und'][0]['value'] = '';
                $classes[-$i] = $blank;
            }
        }
    }

    $examples = ['HIST 101', 'CALC 120', 'MAT 200', 'PHY 110', 'BUS 300', 'ANT 350', 'GEO 400', 'BIO 250', 'CHM 180', 'PHIL 102', 'ENG 100'];

    $classI = 0;
    foreach($classes as $eid => $c)
    {
        $classConfig[$eid]['className'] = 'class' . $classI;
        $startDate = null;
        $endDate = null;
        if(isset($classes[$eid]->field_time['und'][0]['value']))
            $startDate = strtotime(date('Y/m/d H:i:s', strtotime($classes[$eid]->field_time['und'][0]['value'])) . ' UTC');
        if(isset($classes[$eid]->field_time['und'][0]['value2']))
            $endDate = strtotime(date('Y/m/d H:i:s', strtotime($classes[$eid]->field_time['und'][0]['value2'])) . ' UTC');

        $daysOfTheWeek = array();
        if(isset($classes[$eid]->field_day_of_the_week['und'][0]['value']))
            $daysOfTheWeek = array_map(function ($x) { return $x['value']; }, $classes[$eid]->field_day_of_the_week['und']);

        ?>
        <div class="row <?php print ($c->field_class_name['und'][0]['value'] == '' ? 'edit' : ''); ?>" id="eid-<?php print $eid; ?>">
            <div class="field-type-text field-name-field-class-name field-widget-text-textfield form-wrapper">
                <div class="read-only">
                    <label>&nbsp;</label>
                    <span class="class<?php print $classI; ?>">&nbsp;</span><?php print $c->field_class_name['und'][0]['value']; ?></div>
                <div class="form-item form-type-textfield">
                    <label>Class name</label>
                    <input name="schedule-class-name" value="<?php print $c->field_class_name['und'][0]['value']; ?>"
                           class="text-full form-text jquery_placeholder-processed"
                           type="text" size="60" maxlength="255" placeholder="<?php print $examples[array_rand($examples, 1)]; ?>" value="" autocomplete="off">
                </div>
            </div>
            <div class="field-type-list-text field-name-field-day-of-the-week field-widget-options-buttons form-wrapper">
                <div class="read-only">
                    <label>M</label><label>Tu</label><label>W</label><label>Th</label><label>F</label><label>Sa</label><label>Su</label>
                    <span class="<?php print (in_array('M', $daysOfTheWeek) ? 'checked' : 'unchecked'); ?>">M</span>
                    <span class="<?php print (in_array('Tu', $daysOfTheWeek) ? 'checked' : 'unchecked'); ?>">Tu</span>
                    <span class="<?php print (in_array('W', $daysOfTheWeek) ? 'checked' : 'unchecked'); ?>">W</span>
                    <span class="<?php print (in_array('Th', $daysOfTheWeek) ? 'checked' : 'unchecked'); ?>">Th</span>
                    <span class="<?php print (in_array('F', $daysOfTheWeek) ? 'checked' : 'unchecked'); ?>">F</span>
                    <span class="<?php print (in_array('Sa', $daysOfTheWeek) ? 'checked' : 'unchecked'); ?>">Sa</span>
                    <span class="<?php print (in_array('Su', $daysOfTheWeek) ? 'checked' : 'unchecked'); ?>">Su</span></div>
                <div class="form-item form-type-checkboxes">
                    <label>M</label><label>Tu</label><label>W</label><label>Th</label><label>F</label><label>Sa</label><label>Su</label>
                    <div class="form-checkboxes">
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-M" class="form-checkbox"
                                   id="schedule-dotw-M-<?php print $eid; ?>" type="checkbox"
                                   value="M" <?php print (in_array('M', $daysOfTheWeek) ? 'checked="checked"' : ''); ?>> <label
                                class="option" for="schedule-dotw-M-<?php print $eid; ?>">M </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-Tu" class="form-checkbox"
                                   id="schedule-dotw-Tu-<?php print $eid; ?>" type="checkbox"
                                   value="Tu" <?php print (in_array('Tu', $daysOfTheWeek) ? 'checked="checked"' : ''); ?>> <label
                                class="option" for="schedule-dotw-Tu-<?php print $eid; ?>">Tu </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-W" class="form-checkbox"
                                   id="schedule-dotw-W-<?php print $eid; ?>" type="checkbox"
                                   value="W" <?php print (in_array('W', $daysOfTheWeek) ? 'checked="checked"' : ''); ?>> <label
                                class="option" for="schedule-dotw-W-<?php print $eid; ?>">W </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-Th" class="form-checkbox"
                                   id="schedule-dotw-Th-<?php print $eid; ?>" type="checkbox"
                                   value="Th" <?php print (in_array('Th', $daysOfTheWeek) ? 'checked="checked"' : ''); ?>> <label
                                class="option" for="schedule-dotw-Th-<?php print $eid; ?>">Th </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-F" class="form-checkbox"
                                   id="schedule-dotw-F-<?php print $eid; ?>" type="checkbox"
                                   value="F" <?php print (in_array('F', $daysOfTheWeek) ? 'checked="checked"' : ''); ?>> <label
                                class="option" for="schedule-dotw-F-<?php print $eid; ?>">F </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-Sa" class="form-checkbox"
                                   id="schedule-dotw-Sa-<?php print $eid; ?>" type="checkbox"
                                   value="Sa" <?php print (in_array('Sa', $daysOfTheWeek) ? 'checked="checked"' : ''); ?>> <label
                                class="option" for="schedule-dotw-Sa-<?php print $eid; ?>">Sa </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-Su" class="form-checkbox"
                                   id="schedule-dotw-Su-<?php print $eid; ?>" type="checkbox"
                                   value="Su" <?php print (in_array('Su', $daysOfTheWeek) ? 'checked="checked"' : ''); ?>> <label
                                class="option" for="schedule-dotw-Su-<?php print $eid; ?>">Su </label>

                        </div>
                    </div>
                </div>
            </div>
            <div class="field-type-datetime field-name-field-time field-widget-date-popup form-wrapper">
                <div class="form-item form-type-textfield">
                    <label>Time</label>
                    <input name="schedule-value-time"
                           class="date-clear form-text jquery_placeholder-processed"
                           value="<?php print ($startDate == null ? '' : date('h:ia', $startDate)); ?>"
                           type="text" size="15" maxlength="10" placeholder="Start" value="">
                </div>
                <div class="form-item form-type-textfield">
                    <label>&nbsp;</label>
                    <input name="schedule-value2-time"
                           class="date-clear form-text jquery_placeholder-processed"
                           value="<?php print ($endDate == null ? '' : date('h:ia', $endDate)); ?>"
                           type="text" size="15" maxlength="10" placeholder="End" value="">
                </div>
                <div class="form-item form-type-textfield">
                    <label>Date</label>
                    <input name="schedule-value-date"
                           class="date-clear form-text jquery_placeholder-processed"
                           value="<?php print ($startDate == null ? '' : date('m/d/Y', $startDate)); ?>"
                           type="text" size="20" maxlength="30" placeholder="First class" value="">
                </div>
                <div class="form-item form-type-textfield">
                    <label>&nbsp;</label>
                    <input name="schedule-value2-date"
                           class="date-clear form-text jquery_placeholder-processed"
                           value="<?php print ($endDate == null ? '' : date('m/d/Y', $endDate)); ?>"
                           type="text" size="20" maxlength="30" placeholder="Last class" value="">
                </div>
                <div class="read-only"><label>Time</label><?php print ($startDate == null ? '' : date('h:i A', $startDate)) . ' - ' . ($endDate == null ? '' : date('h:i A', $endDate)); ?></div>
                <div class="read-only"><label>Date</label><?php print ($startDate == null ? '' : date('m/d/y', $startDate)) . ' - ' . ($endDate == null ? '' : date('m/d/y', $endDate)); ?></div>
            </div>
            <input type="hidden" class="field-type-hidden field-name-field-event-type form-wrapper" name="schedule-type" value="c" />
            <div class="read-only">
                <label>&nbsp;</label>
                <a href="#edit-class">&nbsp;</a>
                <a href="#remove-class">&nbsp;</a>
            </div>
        </div>
    <?php
        $classI++;
    }
    ?>

    <div id="add-class-dialog" class="row invalid">
        <div class="field-type-text field-name-field-class-name field-widget-text-textfield form-wrapper">
            <div class="form-item form-type-textfield">
                <label>Class name</label>
                <input name="schedule-class-name"
                       class="text-full form-text jquery_placeholder-processed"
                       type="text" size="60" maxlength="255" placeholder="Hist 101" value="" autocomplete="off">
            </div>
        </div>
        <div class="field-type-list-text field-name-field-day-of-the-week field-widget-options-buttons form-wrapper">
            <div class="form-item form-type-checkboxes">
                <label>M</label><label>Tu</label><label>W</label><label>Th</label><label>F</label><label>Sa</label><label>Su</label>
                <div class="form-checkboxes">
                    <div class="form-item form-type-checkbox">
                        <input name="schedule-dotw-M" class="form-checkbox"
                               id="schedule-dotw-M" type="checkbox" value="M"> <label
                            class="option" for="schedule-dotw-M">M </label>

                    </div>
                    <div class="form-item form-type-checkbox">
                        <input name="schedule-dotw-Tu" class="form-checkbox"
                               id="schedule-dotw-Tu" type="checkbox" value="Tu"> <label
                            class="option" for="schedule-dotw-Tu">Tu </label>

                    </div>
                    <div class="form-item form-type-checkbox">
                        <input name="schedule-dotw-W" class="form-checkbox"
                               id="schedule-dotw-W" type="checkbox" value="W"> <label
                            class="option" for="schedule-dotw-W">W </label>

                    </div>
                    <div class="form-item form-type-checkbox">
                        <input name="schedule-dotw-Th" class="form-checkbox"
                               id="schedule-dotw-Th" type="checkbox" value="Th"> <label
                            class="option" for="schedule-dotw-Th">Th </label>

                    </div>
                    <div class="form-item form-type-checkbox">
                        <input name="schedule-dotw-F" class="form-checkbox"
                               id="schedule-dotw-F" type="checkbox" value="F"> <label
                            class="option" for="schedule-dotw-F">F </label>

                    </div>
                    <div class="form-item form-type-checkbox">
                        <input name="schedule-dotw-Sa" class="form-checkbox"
                               id="schedule-dotw-Sa" type="checkbox" value="Sa"> <label
                            class="option" for="schedule-dotw-Sa">Sa </label>

                    </div>
                    <div class="form-item form-type-checkbox">
                        <input name="schedule-dotw-Su" class="form-checkbox"
                               id="schedule-dotw-Su" type="checkbox" value="Su"> <label
                            class="option" for="schedule-dotw-Su">Su </label>

                    </div>
                </div>
            </div>
        </div>
        <div class="field-type-datetime field-name-field-time field-widget-date-popup form-wrapper">
            <div class="form-item form-type-textfield">
                <label for="schedule-value-time">Time</label>
                <input name="schedule-value-time"
                       class="date-clear form-text jquery_placeholder-processed"
                       type="text" size="15" maxlength="10" placeholder="Start" value="">
            </div>
            <div class="form-item form-type-textfield">
                <label>&nbsp;</label>
                <input name="schedule-value2-time"
                       class="date-clear form-text jquery_placeholder-processed"
                       type="text" size="15" maxlength="10" placeholder="End" value="">
            </div>
            <div class="form-item form-type-textfield">
                <label for="schedule-value-date">Date</label>
                <input name="schedule-value-date"
                       class="date-clear form-text jquery_placeholder-processed"
                       type="text" size="20" maxlength="30" placeholder="First class" value="">
            </div>
            <div class="form-item form-type-textfield">
                <label>&nbsp;</label>
                <input name="schedule-value2-date"
                       class="date-clear form-text jquery_placeholder-processed"
                       type="text" size="20" maxlength="30" placeholder="Last class" value="">
            </div>
        </div>
        <input type="hidden" class="field-type-hidden field-name-field-event-type form-wrapper" name="schedule-type" value="c" />
    </div>
</div>

<p class="class-actions">
    <a class="field-add-more-submit ajax-processed" href="#add-class">Add <span>+</span> class</a>
    <a href="#save-class" class="highlighted-link more"><?php
        print (drupal_get_path_alias(current_path()) == 'schedule' || drupal_get_path_alias(current_path()) == 'schedule2'
            ? 'Next'
            : 'Save'); ?></a>
    <span class="invalid-times">Error - invalid class time</span>
    <span class="overlaps-only">Error - classes cannot overlap</span>
    <span class="invalid-only">Error - please make sure all class information is filled in</span>
</p>

<?php if(drupal_get_path_alias(current_path()) == 'schedule' ||
    drupal_get_path_alias(current_path()) == 'schedule2'): ?>
    <script type="text/javascript">
        jQuery(document).ready(function () {
            jQuery('.page-path-schedule, .page-path-schedule2').addClass('edit-class-only');
            jQuery('.page-path-schedule .row:visible, .page-path-schedule2 .row:visible').not('#add-class-dialog, #add-other-dialog').addClass('edit');
        });
    </script>
<?php endif; ?>
<hr style="margin-bottom:0;line-height: 1px;clear: both;" />
<p style="margin-bottom:0;">&nbsp;</p>
<h2>Enter work or other recurring obligations here</h2>

<div class="other-schedule">
    <?php

    foreach($others as $eid => $entity)
    {
        if(!isset($entity->field_event_type['und'][0]['value']) || $entity->field_event_type['und'][0]['value'] != 'o' ||
            empty($entity->field_day_of_the_week['und']))
            continue;

        $c = $entity->field_class_name['und'][0]['value'];
        $startDate = null;
        $endDate = null;
        if(isset($entity->field_time['und'][0]['value']))
            $startDate = strtotime(date('Y/m/d H:i:s', strtotime($entity->field_time['und'][0]['value'])) . ' UTC');
        if(isset($entity->field_time['und'][0]['value2']))
            $endDate = strtotime(date('Y/m/d H:i:s', strtotime($entity->field_time['und'][0]['value2'])) . ' UTC');

        $daysOfTheWeek = array();
        if(isset($entity->field_day_of_the_week['und'][0]['value']))
            $daysOfTheWeek = array_map(function ($x) { return $x['value']; }, $entity->field_day_of_the_week['und']);

        ?>
        <div  class="row" id="eid-<?php print $eid; ?>">
            <div class="field-type-text field-name-field-class-name field-widget-text-textfield form-wrapper">
                <div class="read-only">
                    <label>&nbsp;</label>
                    <span>&nbsp;</span><?php print $c; ?></div>
                <div class="form-item form-type-textfield">
                    <label>Class name</label>
                    <input name="schedule-class-name" value="<?php print $c; ?>"
                           class="text-full form-text jquery_placeholder-processed"
                           type="text" size="60" maxlength="255" placeholder="Work" value="" autocomplete="off">
                </div>
            </div>
            <div class="field-type-list-text field-name-field-day-of-the-week field-widget-options-buttons form-wrapper">
                <div class="read-only">
                    <label>M</label><label>Tu</label><label>W</label><label>Th</label><label>F</label><label>Sa</label><label>Su</label>
                    <span class="<?php print (in_array('M', $daysOfTheWeek) ? 'checked' : 'unchecked'); ?>">M</span>
                    <span class="<?php print (in_array('Tu', $daysOfTheWeek) ? 'checked' : 'unchecked'); ?>">Tu</span>
                    <span class="<?php print (in_array('W', $daysOfTheWeek) ? 'checked' : 'unchecked'); ?>">W</span>
                    <span class="<?php print (in_array('Th', $daysOfTheWeek) ? 'checked' : 'unchecked'); ?>">Th</span>
                    <span class="<?php print (in_array('F', $daysOfTheWeek) ? 'checked' : 'unchecked'); ?>">F</span>
                    <span class="<?php print (in_array('Sa', $daysOfTheWeek) ? 'checked' : 'unchecked'); ?>">Sa</span>
                    <span class="<?php print (in_array('Su', $daysOfTheWeek) ? 'checked' : 'unchecked'); ?>">Su</span></div>
                <div class="form-item form-type-checkboxes">
                    <label>M</label><label>Tu</label><label>W</label><label>Th</label><label>F</label><label>Sa</label><label>Su</label>
                    <div class="form-checkboxes">
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-M" class="form-checkbox"
                                   id="schedule-dotw-M-<?php print $eid; ?>" type="checkbox"
                                   value="M" <?php print (in_array('M', $daysOfTheWeek) ? 'checked="checked"' : ''); ?>> <label
                                class="option" for="schedule-dotw-M-<?php print $eid; ?>">M </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-Tu" class="form-checkbox"
                                   id="schedule-dotw-Tu-<?php print $eid; ?>" type="checkbox"
                                   value="Tu" <?php print (in_array('Tu', $daysOfTheWeek) ? 'checked="checked"' : ''); ?>> <label
                                class="option" for="schedule-dotw-Tu-<?php print $eid; ?>">Tu </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-W" class="form-checkbox"
                                   id="schedule-dotw-W-<?php print $eid; ?>" type="checkbox"
                                   value="W" <?php print (in_array('W', $daysOfTheWeek) ? 'checked="checked"' : ''); ?>> <label
                                class="option" for="schedule-dotw-W-<?php print $eid; ?>">W </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-Th" class="form-checkbox"
                                   id="schedule-dotw-Th-<?php print $eid; ?>" type="checkbox"
                                   value="Th" <?php print (in_array('Th', $daysOfTheWeek) ? 'checked="checked"' : ''); ?>> <label
                                class="option" for="schedule-dotw-Th-<?php print $eid; ?>">Th </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-F" class="form-checkbox"
                                   id="schedule-dotw-F-<?php print $eid; ?>" type="checkbox"
                                   value="F" <?php print (in_array('F', $daysOfTheWeek) ? 'checked="checked"' : ''); ?>> <label
                                class="option" for="schedule-dotw-F-<?php print $eid; ?>">F </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-Sa" class="form-checkbox"
                                   id="schedule-dotw-Sa-<?php print $eid; ?>" type="checkbox"
                                   value="Sa" <?php print (in_array('Sa', $daysOfTheWeek) ? 'checked="checked"' : ''); ?>> <label
                                class="option" for="schedule-dotw-Sa-<?php print $eid; ?>">Sa </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-Su" class="form-checkbox"
                                   id="schedule-dotw-Su-<?php print $eid; ?>" type="checkbox"
                                   value="Su" <?php print (in_array('Su', $daysOfTheWeek) ? 'checked="checked"' : ''); ?>> <label
                                class="option" for="schedule-dotw-Su-<?php print $eid; ?>">Su </label>

                        </div>
                    </div>
                </div>
                <div class="field-type-list-text field-name-field-recurring field-widget-options-buttons form-wrapper">
                    <label>Recurring</label>
                    <div class="read-only">
                        <span class="<?php print (in_array('weekly', $daysOfTheWeek) ? 'checked' : 'unchecked'); ?>">Weekly</span>
                        <?php /* <span class="<?php print (in_array('monthly', $daysOfTheWeek) ? 'checked' : 'unchecked'); ?>">Monthly</span><br /> */ ?>
                    </div>
                    <div class="form-checkboxes form-type-checkbox">
                        <input type="checkbox" name="schedule-reoccurring-<?php print $eid; ?>" id="other-schedule-weekly-<?php print $eid; ?>"
                               value="weekly" class="form-checkbox" <?php print (in_array('weekly', $daysOfTheWeek) ? 'checked="checked"' : ''); ?>/>
                        <label for="other-schedule-weekly-<?php print $eid; ?>">Weekly</label>
                        <?php /*<input type="checkbox" name="schedule-reoccurring-<?php print $eid; ?>" id="other-schedule-monthly-<?php print $eid; ?>"
                               value="monthly" class="form-checkbox" <?php print (in_array('monthly', $daysOfTheWeek) ? 'checked="checked"' : ''); ?>/>
                        <label for="other-schedule-monthly-<?php print $eid; ?>">Monthly</label> */ ?>
                    </div>
                </div>
            </div>
            <div class="field-type-datetime field-name-field-time field-widget-date-popup form-wrapper">
                <div class="form-item form-type-textfield">
                    <label for="schedule-value-time">Time</label>
                    <input name="schedule-value-time"
                           class="date-clear form-text jquery_placeholder-processed"
                           value="<?php print ($startDate == null ? '' : date('h:ia', $startDate)); ?>"
                           type="text" size="15" maxlength="10" placeholder="Start" value="">
                </div>
                <div class="form-item form-type-textfield">
                    <label>&nbsp;</label>
                    <input name="schedule-value2-time"
                           class="date-clear form-text jquery_placeholder-processed"
                           value="<?php print ($endDate == null ? '' : date('h:ia', $endDate)); ?>"
                           type="text" size="15" maxlength="10" placeholder="End" value="">
                </div>
                <div class="form-item form-type-textfield">
                    <label
                        for="schedule-value-date">Date</label>
                    <input name="schedule-value-date"
                           class="date-clear form-text jquery_placeholder-processed"
                           value="<?php print ($startDate == null ? '' : date('m/d/Y', $startDate)); ?>"
                           type="text" size="20" maxlength="30" placeholder="First class" value="">
                </div>
                <div class="form-item form-type-textfield">
                    <label>&nbsp;</label>
                    <input name="schedule-value2-date"
                           class="date-clear form-text jquery_placeholder-processed"
                           value="<?php print ($endDate == null ? '' : date('m/d/Y', $endDate)); ?>"
                           type="text" size="20" maxlength="30" placeholder="Last class" value="">
                </div>
                <div class="read-only"><label>Time</label><?php print ($startDate == null ? '' : date('h:i A', $startDate)) . ' - ' . ($endDate == null ? '' : date('h:i A', $endDate)); ?></div>
                <div class="read-only"><label>Date</label><?php print ($startDate == null ? '' : date('m/d/y', $startDate)) . ' - ' . ($endDate == null ? '' : date('m/d/y', $endDate)); ?></div>
            </div>
            <input type="hidden" class="field-type-hidden field-name-field-event-type form-wrapper" name="schedule-type" value="o" />
            <div class="read-only">
                <label>&nbsp;</label>
                <a href="#edit-class">&nbsp;</a>
                <a href="#remove-class">&nbsp;</a>
            </div>
        </div>
    <?php
    }
    ?>

    <div id="add-other-dialog" class="row invalid">
        <div class="field-type-text field-name-field-class-name field-widget-text-textfield form-wrapper">
            <div class="form-item form-type-textfield">
                <label>Event title</label>
                <input name="schedule-class-name"
                       class="text-full form-text jquery_placeholder-processed"
                       type="text" size="60" maxlength="255" placeholder="Work" value="" autocomplete="off">
            </div>
        </div>
        <div class="field-type-list-text field-name-field-day-of-the-week field-widget-options-buttons form-wrapper">
            <div class="form-item form-type-checkboxes">
                <label>M</label><label>Tu</label><label>W</label><label>Th</label><label>F</label><label>Sa</label><label>Su</label>
                <div class="form-checkboxes">
                    <div class="form-item form-type-checkbox">
                        <input name="schedule-dotw-M" class="form-checkbox"
                               id="other-schedule-dotw-M" type="checkbox" value="M">
                        <label class="option" for="other-schedule-dotw-M">M</label>
                    </div>
                    <div class="form-item form-type-checkbox">
                        <input name="schedule-dotw-Tu" class="form-checkbox"
                               id="other-schedule-dotw-Tu" type="checkbox" value="Tu">
                        <label class="option" for="other-schedule-dotw-Tu">Tu</label>
                    </div>
                    <div class="form-item form-type-checkbox">
                        <input name="schedule-dotw-W" class="form-checkbox"
                               id="other-schedule-dotw-W" type="checkbox" value="W">
                        <label class="option" for="other-schedule-dotw-W">W</label>
                    </div>
                    <div class="form-item form-type-checkbox">
                        <input name="schedule-dotw-Th" class="form-checkbox"
                               id="other-schedule-dotw-Th" type="checkbox" value="Th">
                        <label class="option" for="other-schedule-dotw-Th">Th</label>
                    </div>
                    <div class="form-item form-type-checkbox">
                        <input name="schedule-dotw-F" class="form-checkbox"
                               id="other-schedule-dotw-F" type="checkbox" value="F">
                        <label class="option" for="other-schedule-dotw-F">F</label>
                    </div>
                    <div class="form-item form-type-checkbox">
                        <input name="schedule-dotw-Sa" class="form-checkbox"
                               id="other-schedule-dotw-Sa" type="checkbox" value="Sa">
                        <label class="option" for="other-schedule-dotw-Sa">Sa</label>
                    </div>
                    <div class="form-item form-type-checkbox">
                        <input name="schedule-dotw-Su" class="form-checkbox"
                               id="other-schedule-dotw-Su" type="checkbox" value="Su">
                        <label class="option" for="other-schedule-dotw-Su">Su</label>
                    </div>
                </div>
                <div class="field-name-field-recurring">
                    <label>Recurring</label>
                    <div class="form-checkboxes form-type-checkbox">
                        <input type="checkbox" name="schedule-reoccurring" id="other-schedule-weekly" value="weekly" class="form-checkbox" checked="checked" />
                        <label for="other-schedule-weekly">Weekly</label>
                        <?php /* <input type="checkbox" name="schedule-reoccurring" id="other-schedule-monthly" value="monthly" class="form-checkbox" />
                        <label for="other-schedule-monthly">Monthly</label> */ ?>
                    </div>
                </div>
            </div>
        </div>
        <div class="field-type-datetime field-name-field-time field-widget-date-popup form-wrapper">
            <div class="form-item form-type-textfield">
                <label>Time</label>
                <input name="schedule-value-time"
                       class="date-clear form-text jquery_placeholder-processed"
                       type="text" size="15" maxlength="10" placeholder="Start" value="">
            </div>
            <div class="form-item form-type-textfield">
                <label>&nbsp;</label>
                <input name="schedule-value2-time"
                       class="date-clear form-text jquery_placeholder-processed"
                       type="text" size="15" maxlength="10" placeholder="End" value="">
            </div>
            <div class="form-item form-type-textfield">
                <label>Date</label>
                <input name="schedule-value-date"
                       class="date-clear form-text jquery_placeholder-processed"
                       type="text" size="20" maxlength="30" placeholder="Start" value="">
            </div>
            <div class="form-item form-type-textfield">
                <label>&nbsp;</label>
                <input name="schedule-value2-date"
                       class="date-clear form-text jquery_placeholder-processed"
                       type="text" size="20" maxlength="30" placeholder="End" value="">
            </div>
        </div>
        <input type="hidden" class="field-type-hidden field-name-field-event-type form-wrapper" name="schedule-type" value="o" />
    </div>

</div>

<p class="other-actions">
    <a class="field-add-more-submit ajax-processed" href="#add-other">Add <span>+</span> other event</a>
    <a href="#save-class" class="highlighted-link more"><?php
        print (drupal_get_path_alias(current_path()) == 'schedule' || drupal_get_path_alias(current_path()) == 'schedule2'
            ? 'Next'
            : 'Save'); ?></a>
    <span class="invalid-times">Error - invalid class time</span>
    <span class="overlaps-only">Error - classes cannot overlap</span>
    <span class="invalid-only">Error - please make sure all class information is filled in</span>
</p>

<p style="margin-bottom:0;clear:both;line-height: 1px">&nbsp;</p>