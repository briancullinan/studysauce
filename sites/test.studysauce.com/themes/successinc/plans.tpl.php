<?php
drupal_add_css(drupal_get_path('theme', 'successinc') .'/plans.css');
drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/plans.js');
// check if user has purchased a plan
if(!isset($account))
{
    global $user;
    $account = user_load($user->uid);
}
$lastOrder = _studysauce_orders_by_uid($account->uid);
$groups = og_get_groups_by_user();

// only load events if the user is paid
if($lastOrder || !empty($groups['node']))
    list($events, $node, $classes, $entities) = studysauce_get_events($account, $lastOrder ? $lastOrder->created : null);

// adjust times for demo accounts
if(!$lastOrder && empty($groups['node']))
{
    if(!$lastOrder && empty($groups['node']))
    {
        $sample = theme('studysauce-plan-sample');
        $encoded = preg_replace('/<\/?script>/i', '', $sample);
        list($events, $classes, $entities) = json_decode($encoded);
    }

    // load study type into a property so it can be used to determine the default session type below
    $entities = array_map(function ($x) {
        return (object) array('field_study_type' => array('und' => array(0 => array('value' => $x)))); }, (array)$entities);

    // convert classes to numeric array
    $classes = (array) $classes;
    foreach($classes as $i => $c)
        $classes[intval($i)] = $c;

    $startWeek = strtotime('this week 00:00:00', time()) - 86400;
    $events = array_map(function ($x) use ($startWeek) {
        $x = (array)$x;

        // update times so there is always something showing
        $classT = new DateTime($x['start']);
        $startT = strtotime('this week 00:00:00', $classT->getTimestamp()) - 86400;
        $diff = $startWeek - $startT;
        $classT->setTimestamp($classT->getTimestamp() + $diff);
        $classE = new DateTime($x['end']);
        $classE->setTimestamp($classE->getTimestamp() + $diff);
        $x['start'] = $classT->format('Y/m/d H:i:s') . ' UTC';
        $x['end'] = $classE->format('Y/m/d H:i:s') . ' UTC';

        return $x;
    }, (array) $events);
}

global $studysauceExportEvents, $studysauceExportClasses;
$studysauceExportEvents = $events;
$studysauceExportClasses = $classes;

if(!$lastOrder && empty($groups['node'])): ?><div class="buy-plan"><?php endif;

    $strategies = studysauce_get_strategies();

    // on mobile only show event between this week, hide everything else unless the user clicks View historic
    $startWeek = strtotime(date("Y-m-d", strtotime('this week', time()))) - 86400;
    $endWeek = $startWeek + 604800 - 86400;
    //$dotwStr = date('l', strtotime($event['start']));
    ?>

    <h2><?php print (isset($account->field_first_name['und'][0]['value']) ? ('Personalized study plan for ' . $account->field_first_name['und'][0]['value']) : 'Your personalized study plan'); ?></h2>
    <div id="calendar" class="full-only"></div>
    <script type="text/javascript">

        window.planEvents = <?php print json_encode($events); ?>; // convert events array to object to keep track of keys better
        window.strategies = <?php print json_encode($strategies); ?>;
    </script>
    <div class="sort-by">
        <label>Sort by: </label>
        <input type="radio" id="schedule-by-date" name="schedule-by" value="date" checked="checked"><label for="schedule-by-date">Date</label>&nbsp;
        <input type="radio" id="schedule-by-class" name="schedule-by" value="class"><label for="schedule-by-class">Class</label>
        <input type="checkbox" id="schedule-historic"><label for="schedule-historic">View historical</label>
    </div>
    <?php
    print theme('studysauce-strategies');

    $first = true;
    $headStr = '';
    $classes[''] = 'Nonacademic';
    $classes['f'] = 'Free study';
    foreach ($events as $eid => $event)
    {
        if($eid == '') continue;

        // TODO: should we allow notes for class events?
        if(strpos($event['className'], 'class-event') !== false ||
            strpos($event['className'], 'holiday-event') !== false)
            continue;

        $time = strtotime($event['start']);
        if($headStr != date('j F', $time))
        {
            $headStr = date('j F', $time);
            ?><div class="head <?php print ($time < strtotime(date('Y/m/d')) - 86400 ? 'hide' : ''); ?>"><?php print $headStr; ?></div><?
        }
        $classI = '';
        $cid = '';
        if(preg_match('/class([0-9]+)(\s|$)/i', $event['className'], $matches))
        {
            $classI = $matches[1];
            $cid = array_keys($classes)[$classI];
        }
        $session = isset($strategies[$event['title']]) &&
            array_search(true, array_map(function ($x) { return $x['default']; }, $strategies[$event['title']])) !== false
            // check if the strategy default is saved
            ? array_search(true, array_map(function ($x) { return $x['default']; }, $strategies[$event['title']]))
            // if this is a deadline or not a class event, show notes box
            : (strpos($event['className'], 'deadline-event') !== false || $classI == ''
                ? 'other'
                : (strpos($event['className'], 'p-event') !== false
                    ? 'prework'
                    // if no strategy default to sr
                    // convert memorization answer to spaced
                    : (!isset($entities[$cid]->field_study_type['und'][0]['value']) || $entities[$cid]->field_study_type['und'][0]['value'] == 'memorization'
                        ? 'spaced'
                        // convert reading answer to active
                        : ($entities[$cid]->field_study_type['und'][0]['value'] == 'reading'
                            ? 'active'
                            // convert conceptual answer to teach
                            : ($entities[$cid]->field_study_type['und'][0]['value'] == 'conceptual'
                                ? 'teach'
                                // if nothing is selected nothing shows up
                                : '')))));

        if(strpos($event['className'], 'deadline-event') !== false)
            $title = 'Deadline' . preg_replace(array('/' . preg_quote($classes[$cid]) . '\s*/'), array(''), $event['title']);
        elseif(strpos($event['className'], 'free-event') !== false)
        {
            $cid = 'f';
            $title = 'Any class needed';
        }
        elseif(strpos($event['className'], 'sr-event') !== false)
            $title = $session == 'active' ? 'Active reading' : ($session == 'teach' ? 'Teach' : 'Spaced repetition');
        elseif(strpos($event['className'], 'p-event') !== false)
            $title = 'Pre-work';
        else
            $title = $event['title'];

        ?>
        <div class="row <?php
        print ($first && !($first = false) ? 'first' : ''); ?> <?php
        print ((strpos($event['className'], 'deadline-event') !== false) ? 'deadline' : ''); ?> <?php
        print ($time < strtotime(date('Y/m/d')) - 86400 ? 'hide' : ''); ?> <?php
        print (strtotime($event['start']) >= $startWeek && strtotime($event['start']) <= $endWeek ? 'mobile' : ''); ?> <?php
        print ('class' . $classI); ?> <?php
        print ('cid' . $cid); ?> <?php
        print ('default-' . $session); ?> <?php
        print (isset($event['completed']) && $event['completed'] ? 'done' : ''); ?>" id="eid-<?php print $eid; ?>">
            <div class="field-type-text field-name-field-class-name field-widget-text-textfield form-wrapper">
                <span class="class">&nbsp;</span>
                <div class="read-only"><?php print htmlspecialchars($classes[$cid], ENT_QUOTES); ?></div>
            </div>
            <div class="field-type-text field-name-field-assignment field-widget-text-textfield form-wrapper">
                <div class="read-only"><?php print htmlspecialchars($title, ENT_QUOTES); ?></div>
            </div>
            <div class="field-type-number-integer field-name-field-percent field-widget-number form-wrapper">
                <div class="read-only"><?php if(isset($event['percent']) && !empty($event['percent'])): print intval($event['percent']); ?>% of grade<?php else: ?>&nbsp;<?php endif; ?></div>
            </div>
            <div class="field-type-list-boolean field-name-field-completed field-widget-options-onoff form-wrapper">
                <div class="read-only">
                    <input type="checkbox" id="schedule-completed-<?php print $eid; ?>" <?php
                    print (isset($event['completed']) && $event['completed'] ? 'checked="checked"' : ''); ?> />
                    <label for="schedule-completed-<?php print $eid; ?>">&nbsp;</label></div>
            </div>
            <input type="hidden" name="plan-path" value="<?php print url('node/plup/plan', array('query' => array('plupload_token' => drupal_get_token('plupload-handle-uploads')))); ?>" />
            <input type="hidden" name="plan-title" value="<?php print $event['title']; ?>" />
        </div>
    <?php
    }
    ?>
    <p style="clear: both; margin-bottom:0; line-height:1px;">&nbsp;</p>
    <a class="return-to-top" href="#return-to-top">Top</a>

<?php if(!$lastOrder && empty($groups['node'])): ?>
        <div class="middle-wrapper">
            <div class="highlighted-link">
                <a href="#premium"><h2>Upgrade to premium and we will build your personalized study plan.</h2></a>
                <a class="more-parents" href="#parents" onclick="jQuery('#plan').addClass('bill-my-parents-only'); return false;">Bill my parents</a>
                <div class="bill-my-parents">
                    <h3>Send an email to have someone prepay for Study Sauce.  We will then alert you when your account has been activated.</h3>
                    <div class="form-item webform-component webform-component-textfield webform-component--student-first-name">
                        <label>First name</label>
                        <input type="text" name="invite-first" size="60" maxlength="128" class="form-text required"
                               value="">
                    </div>
                    <div class="form-item webform-component webform-component-textfield webform-component--student-last-name">
                        <label>Last name</label>
                        <input type="text" name="invite-last" size="60" maxlength="128" class="form-text required"
                               value="">
                    </div>
                    <div class="form-item webform-component webform-component-email">
                        <label>Friend's email</label>
                        <input class="email form-text form-email required" type="email" name="invite-email" size="60"
                               value="">
                    </div>
                    <div style="text-align: right;">
                        <a href="#bill-send" class="more">Send</a></div>
                    <a href="#" onclick="jQuery('#plan').removeClass('bill-my-parents-only bill_step_2_only').scrollintoview(); return false;"
                       class="fancy-close">&nbsp;</a>
                </div>
                <div class="bill_step_2">
                    <h2>Thanks!</h2>
                    <h3>We will let you know when your account has been activated.</h3>
                    <div style="text-align: right;">
                        <a href="#" onclick="jQuery('#plan').removeClass('bill-my-parents-only bill_step_2_only').scrollintoview(); return false;" class="more">Close</a></div>
                    <a href="#" onclick="jQuery('#plan').removeClass('bill-my-parents-only bill_step_2_only').scrollintoview(); return false;"
                       class="fancy-close">&nbsp;</a>
                </div>
            </div>
        </div>
    </div>
<?php endif; ?>

