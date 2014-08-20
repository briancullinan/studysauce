<?php
drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/quiz.js');

global $user;
$user = user_load($user->uid);
$classes = _studysauce_get_schedule_classes();

$lastOrder = _studysauce_orders_by_uid($user->uid);
$groups = og_get_groups_by_user();

$quizSubmissions = array();
$quiz = db_select('webform_submissions', 's')
    ->fields('s', array('sid', 'submitted'))
    ->condition('uid', $user->uid, '=')
    ->condition('nid', 17, '=')
    ->execute();
while(($sub = $quiz->fetchAssoc()) && isset($sub['sid']))
{
    $quizSubmissions[$sub['sid']] = $sub['submitted'];
}

// get checkins
$checkins = array();
$checkouts = array();
$query = new EntityFieldQuery();
$entities = $query->entityCondition('entity_type', 'node')
    ->propertyCondition('type', 'schedule')
    ->propertyCondition('title', isset($user->mail) ? $user->mail : '')
    ->propertyCondition('status', 1)
    ->range(0,1)
    ->execute();
if (!empty($entities['node']))
{
    $nodes = array_keys($entities['node']);
    $nid = array_shift($nodes);
    // used to check profile and checkin times
    $node = node_load($nid);
    if(isset($node->field_classes[LANGUAGE_NONE][0]))
    {
        $validLocations = array();
        $classes = array_map(function ($class) {
            $entity = entity_load('field_collection_item', array($class['value']));
            if(!empty($entity))
                return $entity[$class['value']];
            else
                return null;
        }, $node->field_classes[LANGUAGE_NONE]);

        list($checkins, $checkouts) = studysauce_clean_checkins($classes);
    }
}

$query = new EntityFieldQuery();
$entities = $query->entityCondition('entity_type', 'node')
    ->propertyCondition('type', 'strategies')
    ->propertyCondition('title', isset($user->mail) ? $user->mail : '')
    ->propertyCondition('status', 1)
    ->range(0,1)
    ->execute();
if (!empty($entities['node']))
{
    $nodes = array_keys($entities['node']);
    $nid = array_shift($nodes);
    // used to check profile and checkin times
    $strategies = node_load($nid);
}

// check if goals is filled out
list($b) = _studysauce_unsponsored_goals();

if(isset($user->field_partners['und'][0]['value']))
{
    $partner = entity_load('field_collection_item', array($user->field_partners['und'][0]['value']));
    $partner = $partner[$user->field_partners['und'][0]['value']];
}

?>
<div id="study-quiz">
    <div>
        <div class="form-item webform-component webform-component-radios webform-component--study-place">
            <label for="edit-submitted-study-place">1.  I always study in the same place <span class="form-required" title="This field is required.">*</span></label>
            <div class="form-radios"><div class="form-item form-type-radio form-item-submitted-study-place">
                    <input type="radio" id="edit-submitted-study-place-1" name="submitted[study_place]" value="True" class="form-radio">  <label class="option" for="edit-submitted-study-place-1">True </label>

                </div>
                <div class="form-item form-type-radio form-item-submitted-study-place">
                    <input type="radio" id="edit-submitted-study-place-2" name="submitted[study_place]" value="False" class="form-radio">  <label class="option" for="edit-submitted-study-place-2">False </label>

                </div>
            </div>
        </div>
        <div class="form-item webform-component webform-component-radios webform-component--underlining">
            <label for="edit-submitted-underlining">2.  I underline and highlight materials to help me remember information <span class="form-required" title="This field is required.">*</span></label>
            <div class="form-radios"><div class="form-item form-type-radio form-item-submitted-underlining">
                    <input type="radio" id="edit-submitted-underlining-1" name="submitted[underlining]" value="True" class="form-radio">  <label class="option" for="edit-submitted-underlining-1">True </label>

                </div>
                <div class="form-item form-type-radio form-item-submitted-underlining">
                    <input type="radio" id="edit-submitted-underlining-2" name="submitted[underlining]" value="False" class="form-radio">  <label class="option" for="edit-submitted-underlining-2">False </label>

                </div>
            </div>
        </div>
        <div class="form-item webform-component webform-component-radios webform-component--same-subject">
            <label for="edit-submitted-same-subject">3.  I focus on one subject until I am sure I understand everything <span class="form-required" title="This field is required.">*</span></label>
            <div class="form-radios"><div class="form-item form-type-radio form-item-submitted-same-subject">
                    <input type="radio" id="edit-submitted-same-subject-1" name="submitted[same_subject]" value="True" class="form-radio">  <label class="option" for="edit-submitted-same-subject-1">True </label>

                </div>
                <div class="form-item form-type-radio form-item-submitted-same-subject">
                    <input type="radio" id="edit-submitted-same-subject-2" name="submitted[same_subject]" value="False" class="form-radio">  <label class="option" for="edit-submitted-same-subject-2">False </label>

                </div>
            </div>
        </div>
        <div class="form-item webform-component webform-component-radios webform-component--laying-down">
            <label for="edit-submitted-laying-down">4.  I frequently study while lying down or in a comfortable place <span class="form-required" title="This field is required.">*</span></label>
            <div class="form-radios"><div class="form-item form-type-radio form-item-submitted-laying-down">
                    <input type="radio" id="edit-submitted-laying-down-1" name="submitted[laying_down]" value="True" class="form-radio">  <label class="option" for="edit-submitted-laying-down-1">True </label>

                </div>
                <div class="form-item form-type-radio form-item-submitted-laying-down">
                    <input type="radio" id="edit-submitted-laying-down-2" name="submitted[laying_down]" value="False" class="form-radio">  <label class="option" for="edit-submitted-laying-down-2">False </label>

                </div>
            </div>
        </div>
        <div class="form-item webform-component webform-component-radios webform-component--longer-sessions">
            <label for="edit-submitted-longer-sessions">5.  I prefer longer study sessions to power through material <span class="form-required" title="This field is required.">*</span></label>
            <div class="form-radios"><div class="form-item form-type-radio form-item-submitted-longer-sessions">
                    <input type="radio" id="edit-submitted-longer-sessions-1" name="submitted[longer_sessions]" value="True" class="form-radio">  <label class="option" for="edit-submitted-longer-sessions-1">True </label>

                </div>
                <div class="form-item form-type-radio form-item-submitted-longer-sessions">
                    <input type="radio" id="edit-submitted-longer-sessions-2" name="submitted[longer_sessions]" value="False" class="form-radio">  <label class="option" for="edit-submitted-longer-sessions-2">False </label>

                </div>
            </div>
        </div>
        <div class="highlighted-link form-actions">
            <a href="#submit-quiz" class="webform-submit button-primary more form-submit">Submit</a>
            <a href="#" onclick="jQuery('#home').removeClass('study-quiz-only').scrollintoview(); return false;" class="fancy-close">&nbsp;</a>
            <p style="margin-bottom:0;line-height: 1px; clear:both;">&nbsp;</p>
        </div>
    </div>
</div>
<div id="parent_home" class="parents_only">
    <h2 class="full-size">Thank you for taking a strong interest in the study behavior of your student</h2>
    <h2 class="mobile-size">Getting started is easy</h2>
    <div class="grid_6 big-arrow">
        <h3>What we do</h3>
        <p>Study Sauce was born from the realization that no one ever teaches students how to study.  This is crazy considering how much time is spent studying.</p><p>We have integrated the best scientific findings into our service to teach students the most effective methods.  We teach students how to study and change behavior along the way to create a lasting, positive change.</p><p>Our software automatically detects many of the most important study behaviors and alerts the students to both good and bad habits.  We make it easy to become a great studier.</p>
    </div>
    <div class="grid_6 highlighted-link">
        <h3>What you can do as a parent</h3>
        <ol>
            <li><h4><span>1</span> Invite student <a href="#invite" class="more">Invite</a></h4><span>Send an invitation to your student to join and learn the best study methods.</span></li>
            <li><h4><span>2</span> Set up incentives <a href="#goals" class="more">Set goals</a></h4><span>Give your student a little extra motivation. Incentive psychology works, try it!</span></li>
            <li><h4><span>3</span> Upgrade <a href="#plan" class="more">Study plan</a></h4><span>Purchase a personalized study plan for your student. We guarantee a higher GPA or your money back.</span></li>
        </ol>
    </div>
</div>
<div id="student_home" class="students_only">
    <h2>Study Sauce focuses on the 3 principles of studying</h2>
    <img src="/<?php print drupal_get_path('theme', 'successinc'); ?>/images/home-wheel.png" />
    <div id="getting-started">
        <h3>Getting started is easy</h3>
        <p>
            <input id="home-tasks-quiz" name="home-tasks-quiz" type="checkbox" readonly="readonly"
                <?php print (count($quizSubmissions) ? 'checked="checked"' : ''); ?> />
            <label for="home-tasks-quiz"><a href="#study-quiz"><span>Take the study quiz</span></a></label><br />
            <input id="home-tasks-schedule" name="home-tasks-schedule" type="checkbox" readonly="readonly"
                <?php print (count($classes) ? 'checked="checked"' : ''); ?> />
            <label for="home-tasks-schedule"><a href="#schedule"><span>Enter your class schedule</span></a></label><br />
            <input id="home-tasks-deadlines" name="home-tasks-deadlines" type="checkbox" readonly="readonly"
                <?php print (studysauce_any_dates() ? 'checked="checked"' : ''); ?> />
            <label for="home-tasks-deadlines"><a href="#deadlines"><span>Set up deadline reminders</span></a></label><br />
            <input id="home-tasks-checklist" name="home-tasks-checklist" type="checkbox" readonly="readonly"
                <?php print (count($classes) &&
                studysauce_any_dates() &&
                !empty($lastOrder) &&
                count($checkins) > 1 &&
                isset($node->field_touched_music['und'][0]['value']) &&
                count($quizSubmissions) &&
                isset($node->field_grades['und'][0]['value']) &&
                isset($b->field_hours['und'][0]['value']) &&
                isset($partner->field_first_name['und'][0]['value']) &&
                isset($strategies->field_spaced_strategies['und'][0]['value']) &&
                isset($strategies->field_active_strategies['und'][0]['value']) &&
                isset($strategies->field_teach_strategies['und'][0]['value']) ? 'checked="checked"' : ''); ?> />
            <label for="home-tasks-checklist"><span>Complete checklists below</span></label>
        </p>
    </div>
    <p style="clear:both;">&nbsp;<br />&nbsp;</p>
    <ol>
        <li><h4>Time Management</h4>
            <h3>Stop procrastinating!</h3>
            <p>Planning ahead reduces stress and helps you get better grades.  Our study tools help you plan.</p>
            <p>
                <input id="home-schedule" name="home-schedule" type="checkbox" readonly="readonly"
                    <?php print (count($classes) ? 'checked="checked"' : ''); ?> />
                <label for="home-schedule"><a href="#schedule"><span>Enter class schedule</span></a></label><br />
                <input id="home-reminders" name="home-reminders" type="checkbox" readonly="readonly"
                    <?php print (studysauce_any_dates() ? 'checked="checked"' : ''); ?> />
                <label for="home-reminders"><a href="#deadlines"><span>Set up deadline reminders</span></a></label><br />
                <input id="home-plan" name="home-plan" type="checkbox" readonly="readonly"
                    <?php print (!empty($lastOrder) ? 'checked="checked"' : ''); ?> />
                <label for="home-plan"><a href="/cart/add/e-p13_q1_a4o13_s?destination=cart/checkout"><span>Get a personalized study plan</span><?php if(!isset($groups['node']) && !$lastOrder): ?> <sup class="premium">Premium</sup><?php endif; ?></a></label><br />
                <input id="home-checkin" name="home-checkin" type="checkbox" readonly="readonly"
                    <?php print (count($checkins) > 0 ? 'checked="checked"' : ''); ?> />
                <label for="home-checkin"><a href="#checkin"><span>Check in when you study</span></a></label>
            </p>
        </li>
        <li>
            <h4>Study Environment</h4>
            <h3>Put down your phone and step away from the bed.</h3>
            <p>Learn how to set up an effective study environment.  Check in when you study and we will guide you through the best scientific research.</p>
            <p>
                <input id="home-quiz" name="home-quiz" type="checkbox" readonly="readonly"
                    <?php print (count($quizSubmissions) ? 'checked="checked"' : ''); ?> />
                <label for="home-quiz"><a href="#study-quiz"><span>Take the study quiz to test your knowledge</span></a></label><br />
                <input id="home-tips" name="home-tips" type="checkbox" readonly="readonly"
                    <?php print (count($checkins) > 1 ? 'checked="checked"' : ''); ?> />
                <label for="home-tips"><a href="#checkin"><span>Get personalized study tips when you check in</span></a></label><br />
                <input id="home-music" name="home-music" type="checkbox" readonly="readonly"
                    <?php print (isset($node->field_touched_music['und'][0]['value']) ? 'checked="checked"' : ''); ?> />
                <label for="home-music"><a href="#checkin"><span>Experiment with classical music to see if it helps you focus</span></a></label>
            </p>
        </li>
        <li>
            <h4>Study Strategies</h4>
            <h3>Discover your individual learning profile.</h3>
            <p>Understand how you best learn.  Use the best study methods for the specific task.</p>
            <p>
                <input id="home-profile" name="home-profile" type="checkbox" readonly="readonly"
                    <?php print (isset($node->field_grades['und'][0]['value']) ? 'checked="checked"' : ''); ?> />
                <label for="home-profile"><a href="#profile"><span>Take the learning diagnostic</span><?php if(!isset($groups['node']) && !$lastOrder): ?> <sup class="premium">Premium</sup><?php endif; ?></a></label><br />
                <input id="home-goals" name="home-goals" type="checkbox" readonly="readonly"
                    <?php print (isset($b->field_hours['und'][0]['value']) ? 'checked="checked"' : ''); ?> />
                <label for="home-goals"><a href="#goals"><span>Create your study goals</span></a></label><br />
                <input id="home-partner" name="home-partner" type="checkbox" readonly="readonly"
                    <?php print (isset($partner->field_first_name['und'][0]['value']) ? 'checked="checked"' : ''); ?> />
                <label for="home-partner"><a href="#partner"><span>Invite an accountability partner</span></a></label><br />
                <input id="home-spaced" name="home-spaced" type="checkbox" readonly="readonly"
                    <?php print (isset($strategies->field_spaced_strategies['und'][0]['value']) ? 'checked="checked"' : ''); ?> />
                <label for="home-spaced"><a href="#plan"><span>Complete a spaced repetition study session</span><?php if(!isset($groups['node']) && !$lastOrder): ?> <sup class="premium">Premium</sup><?php endif; ?></a></label><br />
                <input id="home-active" name="home-active" type="checkbox" readonly="readonly"
                    <?php print (isset($strategies->field_active_strategies['und'][0]['value']) ? 'checked="checked"' : ''); ?> />
                <label for="home-active"><span>Complete an active recall exercise</span><?php if(!isset($groups['node']) && !$lastOrder): ?> <sup class="premium">Premium</sup><?php endif; ?></label><br />
                <input id="home-teach" name="home-teach" type="checkbox" readonly="readonly"
                    <?php print (isset($strategies->field_teach_strategies['und'][0]['value']) ? 'checked="checked"' : ''); ?> />
                <label for="home-teach"><span>Complete a teaching video</span><?php if(!isset($groups['node']) && !$lastOrder): ?> <sup class="premium">Premium</sup><?php endif; ?></label><br />
                <?php /* <input id="home-friend" name="home-friend" type="checkbox" readonly="readonly"
                    />
                <label for="home-friend">Ask a friend a question<?php if(!isset($groups['node']) && !$lastOrder): ?> <sup class="premium">Premium</sup><?php endif; ?></label> */ ?>
            </p>
        </li>
    </ol>
</div>
<p style="clear:both;margin:0;font-size:1px;height:0;">&nbsp;</p>