<?php
drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/profile.js');
drupal_add_css(drupal_get_path('theme', 'successinc') .'/profile.css');

global $user;
$lastOrder = _studysauce_orders_by_uid($user->uid);

if($lastOrder):

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

if(drupal_get_path_alias(current_path()) == 'profile' ||
    drupal_get_path_alias(current_path()) == 'customization')
    print theme('studysauce-funnel');
?>
<div class="study-preferences">
    <h2>Please tell us more about your study preferences <span>1/2</span></h2>
<?php /*    <div class="field-type-text field-name-field-university field-widget-text-textfield form-wrapper">
        <label for="schedule-university">School name</label>
        <input class="text-full form-text required" type="text" id="schedule-university" name="schedule-university"
               size="60" maxlength="255" value="<?php print (isset($node->field_university['und'][0]['value']) ? $node->field_university['und'][0]['value'] : ''); ?>">
    </div> */ ?>
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
</div>
<?php
global $user;

$query = new EntityFieldQuery();
$nodes = $query->entityCondition('entity_type', 'node')
    ->propertyCondition('type', 'profile')
    ->propertyCondition('title', isset($user->mail) ? $user->mail : '')
    ->propertyCondition('status', 1)
    ->range(0, 1)
    ->execute();
if (!empty($nodes['node']))
{
    $nodes = array_keys($nodes['node']);
    $nid = array_shift($nodes);
    $node = node_load($nid);
    $node->revision = 1;
}

?>
<div class="field-name-field-profile-question-mindset">
    <h2>Which of the following do you agree with most? <span>Q: 1/5</span></h2>
    <input type="radio" name="profile-question-mindset" id="profile-question-mindset-answer-1" value="born"
        <?php print (isset($node->field_mindset['und'][0]['value']) && $node->field_mindset['und'][0]['value'] == 'born' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-mindset-answer-1">People are born smart.</label>
    <input type="radio" name="profile-question-mindset" id="profile-question-mindset-answer-2" value="practice"
        <?php print (isset($node->field_mindset['und'][0]['value']) && $node->field_mindset['und'][0]['value'] == 'practice' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-mindset-answer-2">People become smart through practice.</label>
</div>
<div class="field-name-field-profile-question-time-management">
    <h2>How do you manage your time studying for exams? <span>Q: 2/5</span></h2>
    <input type="radio" name="profile-question-time-management" id="profile-question-time-management-answer-1" value="advance"
        <?php print (isset($node->field_time_management['und'][0]['value']) && $node->field_time_management['und'][0]['value'] == 'advance' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-time-management-answer-1">I have to space out my studying far in advance of my exam.  Otherwise, I get too stressed out.</label>
    <input type="radio" name="profile-question-time-management" id="profile-question-time-management-answer-2" value="cram"
        <?php print (isset($node->field_time_management['und'][0]['value']) && $node->field_time_management['und'][0]['value'] == 'cram' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-time-management-answer-2">I try to space it out, but usually end up cramming a day or two before my exam.</label>
    <input type="radio" name="profile-question-time-management" id="profile-question-time-management-answer-3" value="pressure"
        <?php print (isset($node->field_time_management['und'][0]['value']) && $node->field_time_management['und'][0]['value'] == 'pressure' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-time-management-answer-3">I do my best work under pressure and plan to cram before each exam.</label>
</div>
<div class="field-name-field-profile-question-devices">
    <h2>How do you manage your electronic devices when you study? <span>Q: 3/5</span></h2>
    <input type="radio" name="profile-question-devices" id="profile-question-devices-answer-1" value="on"
        <?php print (isset($node->field_devices['und'][0]['value']) && $node->field_devices['und'][0]['value'] == 'on' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-devices-answer-1">I keep them nearby.  I will respond to texts, etc. if I get them, and then I right back to work.</label>
    <input type="radio" name="profile-question-devices" id="profile-question-devices-answer-2" value="off"
        <?php print (isset($node->field_devices['und'][0]['value']) && $node->field_devices['und'][0]['value'] == 'off' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-devices-answer-2">I turn them off or put them somewhere so they won't distract me.</label>
</div>
<div class="field-name-field-profile-question-education">
    <h2>What grade are you in? <span>Q: 4/5</span></h2>
    <input type="radio" name="profile-question-education" id="profile-question-education-answer-1" value="middle"
        <?php print (isset($node->field_education['und'][0]['value']) && $node->field_education['und'][0]['value'] == 'middle' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-education-answer-1">Grades 6-8</label>
    <input type="radio" name="profile-question-education" id="profile-question-education-answer-2" value="freshman"
        <?php print (isset($node->field_education['und'][0]['value']) && $node->field_education['und'][0]['value'] == 'freshman' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-education-answer-2">High school Freshman</label>
    <input type="radio" name="profile-question-education" id="profile-question-education-answer-3" value="sophomore"
        <?php print (isset($node->field_education['und'][0]['value']) && $node->field_education['und'][0]['value'] == 'sophomore' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-education-answer-3">High school Sophomore</label>
    <input type="radio" name="profile-question-education" id="profile-question-education-answer-4" value="junior"
        <?php print (isset($node->field_education['und'][0]['value']) && $node->field_education['und'][0]['value'] == 'junior' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-education-answer-4">High school Junior</label>
    <input type="radio" name="profile-question-education" id="profile-question-education-answer-5" value="senior"
        <?php print (isset($node->field_education['und'][0]['value']) && $node->field_education['und'][0]['value'] == 'senior' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-education-answer-5">High school Senior</label>
    <input type="radio" name="profile-question-education" id="profile-question-education-answer-6" value="college-freshmen"
        <?php print (isset($node->field_education['und'][0]['value']) && $node->field_education['und'][0]['value'] == 'college-freshmen' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-education-answer-6">College Freshman</label>
    <input type="radio" name="profile-question-education" id="profile-question-education-answer-7" value="college-sophomore"
        <?php print (isset($node->field_education['und'][0]['value']) && $node->field_education['und'][0]['value'] == 'college-sophomore' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-education-answer-7">College Sophomore</label>
    <input type="radio" name="profile-question-education" id="profile-question-education-answer-8" value="college-senior"
        <?php print (isset($node->field_education['und'][0]['value']) && $node->field_education['und'][0]['value'] == 'college-senior' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-education-answer-8">College Junior/Senior</label>
    <input type="radio" name="profile-question-education" id="profile-question-education-answer-9" value="graduate"
        <?php print (isset($node->field_education['und'][0]['value']) && $node->field_education['und'][0]['value'] == 'graduate' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-education-answer-9">College graduate</label>
</div>
<div class="field-name-field-profile-question-study-much">
    <h2>How much do you study per day? <span>Q: 5/5</span></h2>
    <input type="radio" name="profile-question-study-much" id="profile-question-study-much-answer-1" value="one"
        <?php print (isset($node->field_study_much['und'][0]['value']) && $node->field_study_much['und'][0]['value'] == 'one' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-study-much-answer-1">0-1 hour</label>
    <input type="radio" name="profile-question-study-much" id="profile-question-study-much-answer-2" value="two"
        <?php print (isset($node->field_study_much['und'][0]['value']) && $node->field_study_much['und'][0]['value'] == 'two' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-study-much-answer-2">1-2 hours</label>
    <input type="radio" name="profile-question-study-much" id="profile-question-study-much-answer-3" value="four"
        <?php print (isset($node->field_study_much['und'][0]['value']) && $node->field_study_much['und'][0]['value'] == 'four' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-study-much-answer-3">2-4 hours</label>
    <input type="radio" name="profile-question-study-much" id="profile-question-study-much-answer-4" value="more"
        <?php print (isset($node->field_study_much['und'][0]['value']) && $node->field_study_much['und'][0]['value'] == 'more' ? 'checked="checked"': ''); ?>>
    <label class="option" for="profile-question-study-much-answer-4">4+ hours</label>
</div>
<div class="class-profile">
    <h2>Tell us a little more about your classes <span>2/2</span></h2>
    <div class="field-name-type-of-studying">
        <label>What is the primary type of studying you expect to do in this class?</label>
    </div>
    <div class="field-name-difficulty-level">
        <label>How difficult will this class be?</label>
    </div>
    <div class="field-name-type-of-studying">
        <label>Memorization</label>
        <label>Reading/writing</label>
        <label>Conceptual application</label>
    </div>
    <div class="field-name-difficulty-level">
        <label>Easy</label>
        <label>Average</label>
        <label>Tough</label>
    </div>
    <?php
    $classes = _studysauce_get_schedule_classes();
    foreach($classes as $eid => $c)
    {
        $classI = array_search($eid, array_keys($classes));
        $entity = entity_load('field_collection_item', array($eid));

        ?>
        <div class="row" id="eid-<?php print $eid; ?>">
            <div class="field-type-text field-name-field-class-name field-widget-text-textfield form-wrapper">
                <div class="read-only">
                    <span class="class<?php print $classI; ?>">&nbsp;</span><?php print $c; ?>
                </div>
            </div>
            <label class="mobile-only">What is the primary type of studying you expect to do in this class?</label>
            <div class="form-radios field-name-type-of-studying">
                <input type="radio" id="study-type-class-<?php print $eid; ?>-memorization"
                       name="study-type-class-<?php print $eid; ?>" value="memorization" class="form-radio"
                    <?php print (isset($entity[$eid]->field_study_type['und'][0]['value']) && $entity[$eid]->field_study_type['und'][0]['value'] == 'memorization' ? 'checked' : ''); ?> />
                <label class="option" for="study-type-class-<?php print $eid; ?>-memorization">Memorization</label>
                <input type="radio" id="study-type-class-<?php print $eid; ?>-reading"
                       name="study-type-class-<?php print $eid; ?>" value="reading" class="form-radio"
                    <?php print (isset($entity[$eid]->field_study_type['und'][0]['value']) && $entity[$eid]->field_study_type['und'][0]['value'] == 'reading' ? 'checked' : ''); ?> />
                <label class="option" for="study-type-class-<?php print $eid; ?>-reading">Reading/writing</label>
                <input type="radio" id="study-type-class-<?php print $eid; ?>-conceptual"
                       name="study-type-class-<?php print $eid; ?>" value="conceptual" class="form-radio"
                    <?php print (isset($entity[$eid]->field_study_type['und'][0]['value']) && $entity[$eid]->field_study_type['und'][0]['value'] == 'conceptual' ? 'checked' : ''); ?> />
                <label class="option" for="study-type-class-<?php print $eid; ?>-conceptual">Conceptual application</label>
            </div>
            <label class="mobile-only">How difficult will this class be?</label>
            <div class="form-radios field-name-difficulty-level">
                <input type="radio" id="study-difficulty-class-<?php print $eid; ?>-easy"
                       name="study-difficulty-class-<?php print $eid; ?>" value="easy" class="form-radio"
                    <?php print (isset($entity[$eid]->field_study_difficulty['und'][0]['value']) && $entity[$eid]->field_study_difficulty['und'][0]['value'] == 'easy' ? 'checked' : ''); ?> />
                <label class="option" for="study-difficulty-class-<?php print $eid; ?>-easy">Easy</label>
                <input type="radio" id="study-difficulty-class-<?php print $eid; ?>-average"
                       name="study-difficulty-class-<?php print $eid; ?>" value="average" class="form-radio"
                    <?php print (isset($entity[$eid]->field_study_difficulty['und'][0]['value']) && $entity[$eid]->field_study_difficulty['und'][0]['value'] == 'average' ? 'checked' : ''); ?> />
                <label class="option" for="study-difficulty-class-<?php print $eid; ?>-average">Average</label>
                <input type="radio" id="study-difficulty-class-<?php print $eid; ?>-tough"
                       name="study-difficulty-class-<?php print $eid; ?>" value="tough" class="form-radio"
                    <?php print (isset($entity[$eid]->field_study_difficulty['und'][0]['value']) && $entity[$eid]->field_study_difficulty['und'][0]['value'] == 'tough' ? 'checked' : ''); ?> />
                <label class="option" for="study-difficulty-class-<?php print $eid; ?>-tough">Tough</label>
            </div>
        </div>
    <?php
    }
    ?>
    <h3>Tip - you can change your answers later</h3>
    <h3>Note: Your study sessions will change when you click save</h3>
</div>

<p style="clear:both; margin-bottom:0; text-align: right;" class="highlighted-link">
    <a href="#save-profile" class="more"><?php print (drupal_get_path_alias(current_path()) == 'customization2' ? 'Finish' : (drupal_get_path_alias(current_path()) == 'customization' || drupal_get_path_alias(current_path()) == 'profile' ? 'Next' : 'Save')); ?></a></p>

<?php endif; ?>
<?php if(!$lastOrder): ?>
    <div class="buy-plan">
        <a href="#premium"><h2>Upgrade to premium to discover your unique study profile.</h2></a>
    </div>
<?php endif; ?>