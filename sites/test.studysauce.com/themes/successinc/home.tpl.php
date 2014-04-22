<?php
global $user;
$user = user_load($user->uid);
$classes = _studysauce_get_schedule_classes();
global $user;
if(!isset($orders))
{
    $orders = _studysauce_orders_by_uid($user->uid);
    $conn = studysauce_get_connections();
    foreach ($conn as $i => $c)
        $orders = array_merge($orders, _studysauce_orders_by_uid($c->uid));
}
// get event schedule
$lastOrder = end($orders);

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

// check if goals is filled out
list($b) = _studysauce_unsponsored_goals();

?>
<div id="study-quiz">
    <div>
        <?php
        $node = node_load(17);
        webform_node_view($node, 'full');
        print theme_webform_view($node->content); ?>
        <a href="#" onclick="jQuery('#home').removeClass('study-quiz-only').scrollintoview(); return false;" class="fancy-close">&nbsp;</a>
        <p style="margin-bottom:0;line-height: 1px; clear:both;">&nbsp;</p>
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
            <li><h4><span>1</span> Invite student <a href="#invite2" onclick="jQuery.fancybox({href: jQuery(this).is('.connected') ? '#connections' : '#webform-ajax-wrapper-250', hideOnContentClick: false, centerOnScroll: true, padding:0, type: 'inline'}); return false;" class="more">Invite</a></h4><span>Send an invitation to your student to join and learn the best study methods.</span></li>
            <li><h4><span>2</span> Set up incentives <a href="#goals" class="more">Set goals</a></h4><span>Give your student a little extra motivation. Incentive psychology works, try it!</span></li>
            <li><h4><span>3</span> Upgrade <a href="#plan" class="more">Study plan</a></h4><span>Purchase a personalized study plan for your student. We guarantee a higher GPA or your money back.</span></li>
        </ol>
    </div>
</div>
<div id="student_home" class="students_only">
    <h2>Study Sauce focuses on the 3 principles of studying</h2>
    <img src="/<?php print drupal_get_path('theme', 'successinc'); ?>/images/home-wheel.png" style="float:left;" />
    <div id="getting-started">
        <h3>Getting started is easy</h3>
        <p>
            <input id="home-tasks-quiz" name="home-schedule" type="checkbox" readonly="readonly"
                <?php print (count($quizSubmissions) ? 'checked="checked"' : ''); ?> />
            <label for="home-tasks-quiz"><a href="#study-quiz">Take the study quiz</a></label><br />
            <input id="home-tasks-schedule" name="home-schedule" type="checkbox" readonly="readonly"
                <?php print (count($classes) ? 'checked="checked"' : ''); ?> />
            <label for="home-tasks-schedule"><a href="#schedule">Enter your class schedule</a></label><br />
            <input id="home-tasks-deadlines" name="home-schedule" type="checkbox" readonly="readonly"
                <?php print (studysauce_any_dates() ? 'checked="checked"' : ''); ?> />
            <label for="home-tasks-deadlines"><a href="#deadlines">Set up deadline reminders</a></label><br />
            <input id="home-tasks-checklist" name="home-schedule" type="checkbox" readonly="readonly" />
            <label for="home-tasks-checklist">Complete checklists below</label>
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
                <label for="home-schedule"><a href="#schedule">Enter class schedule</a></label><br />
                <input id="home-reminders" name="home-reminders" type="checkbox" readonly="readonly"
                    <?php print (studysauce_any_dates() ? 'checked="checked"' : ''); ?> />
                <label for="home-reminders"><a href="#deadlines">Set up reminders for key dates</a></label><br />
                <input id="home-plan" name="home-plan" type="checkbox" readonly="readonly"
                    <?php print (!empty($lastOrder) ? 'checked="checked"' : ''); ?> />
                <label for="home-plan"><a href="#premium">Get a personalized study plan (Premium)</a></label><br />
                <input id="home-checkin" name="home-checkin" type="checkbox" readonly="readonly"
                    <?php print (count($checkins) > 0 ? 'checked="checked"' : ''); ?> />
                <label for="home-checkin"><a href="#checkin">Check in when you study</a></label>
            </p>
        </li>
        <li>
            <h4>Study Environment</h4>
            <h3>Put down your phone and step away from the bed.</h3>
            <p>Learn how to set up an effective study environment.  Check in when you study and we will guide you through the best scientific research.</p>
            <p>
                <input id="home-quiz" name="home-quiz" type="checkbox" readonly="readonly"
                    <?php print (count($quizSubmissions) ? 'checked="checked"' : ''); ?> />
                <label for="home-quiz"><a href="#study-quiz">Take the study quiz to test your knowledge</a></label><br />
                <input id="home-tips" name="home-tips" type="checkbox" readonly="readonly"
                    <?php print (count($checkins) > 1 ? 'checked="checked"' : ''); ?> />
                <label for="home-tips"><a href="#checkin">Get personalized study tips when you check in</a></label><br />
                <input id="home-music" name="home-music" type="checkbox" readonly="readonly"
                    />
                <label for="home-music"><a href="#checkin">Experiment with classical music to see if it helps you focus</a></label>
            </p>
        </li>
        <li>
            <h4>Study Strategies</h4>
            <h3>Discover your individual learning profile.</h3>
            <p>Understand how you best learn.  Use the best study methods for the specific task.</p>
            <p>
                <input id="home-profile" name="home-profile" type="checkbox" readonly="readonly"
                    <?php print (isset($node->field_university['und'][0]['value']) ? 'checked="checked"' : ''); ?> />
                <label for="home-profile"><a href="#profile">Take the learning diagnostic <sup class="premium">Premium</sup></a></label><br />
                <input id="home-goals" name="home-goals" type="checkbox" readonly="readonly"
                    <?php print (isset($b->field_hours['und'][0]['value']) ? 'checked="checked"' : ''); ?> />
                <label for="home-goals"><a href="#goals">Create your study goals</a></label><br />
                <input id="home-partner" name="home-partner" type="checkbox" readonly="readonly"
                    />
                <label for="home-partner"><a href="#partner">Invite an accountability partner</a></label><br />
                <input id="home-spaced" name="home-spaced" type="checkbox" readonly="readonly"
                    />
                <label for="home-spaced"><a href="#plan">Complete a spaced repetition study session <sup class="premium">Premium</sup></a></label><br />
                <input id="home-active" name="home-active" type="checkbox" readonly="readonly"
                    />
                <label for="home-active">Complete an active recall exercise <sup class="premium">Premium</sup></label><br />
                <input id="home-teach" name="home-teach" type="checkbox" readonly="readonly"
                    />
                <label for="home-teach">Complete a teaching video <sup class="premium">Premium</sup></label><br />
                <input id="home-friend" name="home-friend" type="checkbox" readonly="readonly"
                    />
                <label for="home-friend">Ask a friend a question <sup class="premium">Premium</sup></label>
            </p>
        </li>
    </ol>
</div>
<p style="clear:both;margin:0;font-size:1px;height:0;">&nbsp;</p>