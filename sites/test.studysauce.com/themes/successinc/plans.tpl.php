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
{
    list($events, $classes, $others) = studysauce_get_events($account, $lastOrder ? $lastOrder->created : null);

    /*
    $simplify = function ($x) { return array(
        'field_class_name' => $x->field_class_name,
        'field_time' => $x->field_time,
        'field_study_type' => $x->field_study_type
    ); };
    $json = json_encode(array($events, array_map($simplify, $classes), array_map($simplify, $others)));
    */
}
else
{
    // adjust times for demo accounts
    if(!$lastOrder && empty($groups['node']))
    {
        $sample = theme('studysauce-plan-sample');
        $encoded = preg_replace('/<\/?script>/i', '', $sample);
        list($events, $classes, $others) = json_decode($encoded);
    }

    // convert classes to numeric array
    $classes = array_map(function ($x) {
        $x->field_time = (array)$x->field_time;
        $x->field_time['und'][0] = (array)$x->field_time['und'][0];
        $x->field_study_type = (array)$x->field_study_type;
        $x->field_study_type['und'][0] = (array)$x->field_study_type['und'][0];
        $x->field_class_name = (array)$x->field_class_name;
        $x->field_class_name['und'][0] = (array)$x->field_class_name['und'][0];

        return $x;
    }, (array) $classes);

    $startWeek = strtotime('this week 00:00:00', time()) - 86400;
    $events = array_map(function ($x) use ($startWeek) {
        $x->field_time = (array)$x->field_time;
        $x->field_time['und'][0] = (array)$x->field_time['und'][0];
        $x->field_event_type = (array)$x->field_event_type;
        $x->field_event_type['und'][0] = (array)$x->field_event_type['und'][0];
        $x->field_class_name = (array)$x->field_class_name;
        $x->field_class_name['und'][0] = (array)$x->field_class_name['und'][0];
        $x->field_deleted = (array)$x->field_deleted;
        $x->field_deleted['und'][0] = (array)$x->field_deleted['und'][0];

        // update times so there is always something showing
        $classT = new DateTime($x->field_time['und'][0]['value']);
        $startT = strtotime('this week 00:00:00', $classT->getTimestamp()) - 86400;
        $diff = $startWeek - $startT;
        $classT->setTimestamp($classT->getTimestamp() + $diff);
        $classE = new DateTime($x->field_time['und'][0]['value2']);
        $classE->setTimestamp($classE->getTimestamp() + $diff);
        $x->field_time['und'][0]['value'] = $classT->format('Y/m/d H:i:s') . ' UTC';
        $x->field_time['und'][0]['value2'] = $classE->format('Y/m/d H:i:s') . ' UTC';

        return $x;
    }, (array) $events);
}

// this is used when the plan has to be rendered but the javascript is removed by jquery like in a ajax callback, the jsEvents is then loaded manually as a separate property
global $studysauceExportEvents, $studysauceExportClasses;
$jsEvents = array();
foreach($events as $i => $x)
{
    // skip data entry events
    if($x->field_deleted['und'][0]['value'])
    {
        continue;
    }

    $cid = array_search($x->field_class_name['und'][0]['value'], array_map(function ($x) { return $x->field_class_name['und'][0]['value']; }, $classes));
    if($cid === false)
        $cid = '';
    $classI = array_search($cid, array_keys($classes));
    if($classI === false)
        $classI = '';

    $label = 'CLASS';
    if($x->field_event_type['und'][0]['value'] == 'sr' || $x->field_event_type['und'][0]['value'] == 'f')
        $label = 'STUDY';
    elseif($x->field_event_type['und'][0]['value'] == 'p')
        $label = 'PRE-WORK';
    elseif($x->field_event_type['und'][0]['value'] == 'o')
        $label = 'OTHER';
    elseif($x->field_event_type['und'][0]['value'] == 'd')
        $label = 'DEADLINE';
    elseif($x->field_event_type['und'][0]['value'] == 'h')
        $label = 'HOLIDAY';
    elseif($x->field_event_type['und'][0]['value'] == 'r')
        $label = 'REMINDER';

    // set up dates recurrence
    if($x->field_event_type['und'][0]['value'] == 'sr')
    {
        $startDay = strtotime($classes[$cid]->field_time['und'][0]['value']);
        $endDay = strtotime($classes[$cid]->field_time['und'][0]['value2']);
        $t = strtotime($x->field_time['und'][0]['value']);
        $dates = array();
        if($t <= $endDay)
            $dates[] = date('n/d', $t);
        if($t - 86400 * 7 >= $startDay && $t - 86400 * 7 <= $endDay)
            $dates[] = date('n/d', $t - 86400 * 7);
        if($t - 86400 * 14 >= $startDay && $t - 86400 * 14 <= $endDay)
            $dates[] = date('n/d', $t - 86400 * 14);
        if($t - 86400 * 28 >= $startDay && $t - 86400 * 28 <= $endDay)
            $dates[] = date('n/d', $t - 86400 * 28);
    }

    $jsEvents[$i] = array(
        'cid' => $i,
        'title' => '<h4>' . $label . '</h4>' . $x->field_class_name['und'][0]['value'],
        'start' => (new DateTime($x->field_time['und'][0]['value'], new DateTimeZone('UTC')))->format('r'),
        'end' => (new DateTime($x->field_time['und'][0]['value2'], new DateTimeZone('UTC')))->format('r'),
        'className' => 'event-type-' . $x->field_event_type['und'][0]['value'] . ' ' . ($classI !== '' ? ('class' . $classI) : ''),
        // all day for deadlines, reminders, and holidays
        'allDay' => $x->field_event_type['und'][0]['value'] == 'd' || $x->field_event_type['und'][0]['value'] == 'h' ||
            $x->field_event_type['und'][0]['value'] == 'r',
        'editable' => ($x->field_event_type['und'][0]['value'] == 'sr' || $x->field_event_type['und'][0]['value'] == 'f' || $x->field_event_type['und'][0]['value'] == 'p'),
        'dates' => isset($dates) ? $dates : null
    );
}
$studysauceExportEvents = $jsEvents;
$studysauceExportClasses = array_map(function ($x) { return $x->field_class_name['und'][0]['value']; }, $classes);

if(!$lastOrder && empty($groups['node'])): ?><div class="buy-plan"><?php endif;

    $strategies = studysauce_get_strategies();

    // on mobile only show event between this week, hide everything else unless the user clicks View historic
    $startWeek = strtotime(date("Y-m-d", strtotime('this week', time()))) - 86400;
    $endWeek = $startWeek + 604800 - 86400;

    ?>

    <h2><?php print (isset($account->field_first_name['und'][0]['value']) ? ('Personalized study plan for ' . $account->field_first_name['und'][0]['value']) : 'Your personalized study plan'); ?></h2>
    <div id="calendar" class="full-only"></div>
    <script type="text/javascript">

        window.planEvents = <?php print json_encode($jsEvents); ?>; // convert events array to object to keep track of keys better
        window.strategies = <?php print json_encode($strategies); ?>;
    </script>
    <div class="sort-by">
        <label>Sort by: </label>
        <input type="radio" id="schedule-by-date" name="schedule-by" value="date" checked="checked"><label for="schedule-by-date">Date</label>&nbsp;
        <input type="radio" id="schedule-by-class" name="schedule-by" value="class"><label for="schedule-by-class">Class</label>
        <a href="#expand">Expand</a>
        <input type="checkbox" id="schedule-historic"><label for="schedule-historic">View historical</label>
    </div>
    <?php
    print theme('studysauce-strategies');

    $first = true;
    $headStr = '';
    $nowTimestamp = date_time_set(new DateTime('now', new DateTimeZone(date_default_timezone_get())), 0, 0, 0)->getTimestamp();
    foreach ($events as $eid => $event)
    {
        if($eid == '') continue;

        // TODO: should we allow notes for class events?
        if(isset($event->field_event_type['und'][0]['value']) &&
            ($event->field_event_type['und'][0]['value'] == 'c' || $event->field_event_type['und'][0]['value'] == 'h' || $event->field_event_type['und'][0]['value'] == 'r'))
            continue;

        $time = new DateTime($event->field_time['und'][0]['value'], new DateTimeZone('UTC'));
        $time->setTimezone(new DateTimeZone(date_default_timezone_get()));
        $newHead = $time->format('j F');
        if($headStr != $newHead)
        {
            $headStr = $newHead;
            ?><div class="head <?php print ($time->getTimestamp() < $nowTimestamp - 86400 ? 'hide' : ''); ?>"><?php print $headStr; ?></div><?
        }
        $cid = array_search($event->field_class_name['und'][0]['value'], array_map(function ($x) { return $x->field_class_name['und'][0]['value']; }, $classes));
        if($cid === false)
            $cid = '';
        $classI = array_search($cid, array_keys($classes));
        if($classI === false)
            $classI = '';

        $session = $event->field_event_type['und'][0]['value'] == 'd' || $classI === ''
                ? 'other'
                : ($event->field_event_type['und'][0]['value'] == 'p'
                    ? 'prework'
                    // if no strategy default to sr
                    // convert memorization answer to spaced
                    : (!isset($classes[$cid]->field_study_type['und'][0]['value']) || $classes[$cid]->field_study_type['und'][0]['value'] == 'memorization'
                        ? 'spaced'
                        // convert reading answer to active
                        : ($classes[$cid]->field_study_type['und'][0]['value'] == 'reading'
                            ? 'active'
                            // convert conceptual answer to teach
                            : ($classes[$cid]->field_study_type['und'][0]['value'] == 'conceptual'
                                ? 'teach'
                                // if nothing is selected nothing shows up
                                : ''))));

        if($event->field_event_type['und'][0]['value'] == 'd')
            $title = 'Deadline' . preg_replace(array('/' . preg_quote($classes[$cid]->field_class_name['und'][0]['value']) . '\s*/'), array(''), $event->field_class_name['und'][0]['value']);
        elseif($event->field_event_type['und'][0]['value'] == 'f')
        {
            $cid = 'f';
            $title = 'Any class needed';
        }
        elseif($event->field_event_type['und'][0]['value'] == 'sr')
            $title = $session == 'active' ? 'Active reading' : ($session == 'teach' ? 'Teach' : 'Spaced repetition');
        elseif($event->field_event_type['und'][0]['value'] == 'p')
            $title = 'Pre-work';
        else
            $title = $event->field_class_name['und'][0]['value'];

        ?>
        <div class="row <?php
        print ($first && !($first = false) ? 'first' : ''); ?> <?php
        print 'event-type-' . $event->field_event_type['und'][0]['value']; ?> <?php
        print ($time->getTimestamp() < $nowTimestamp - 86400 || $event->field_deleted[LANGUAGE_NONE][0]['value'] ? 'hide' : ''); ?> <?php
        print (strtotime($event->field_time['und'][0]['value']) >= $startWeek && strtotime($event->field_time['und'][0]['value']) <= $endWeek ? 'mobile' : ''); ?> <?php
        print ('class' . $classI); ?> <?php
        print ('cid' . $cid); ?> <?php
        print ('default-' . $session); ?> <?php
        print (isset($event->field_completed['und'][0]['value']) && $event->field_completed['und'][0]['value'] ? 'done' : ''); ?>" id="eid-<?php print $eid; ?>">
            <div class="field-type-text field-name-field-class-name field-widget-text-textfield form-wrapper">
                <span class="class">&nbsp;</span>
                <div class="read-only"><?php print htmlspecialchars($event->field_class_name['und'][0]['value'], ENT_QUOTES); ?></div>
            </div>
            <div class="field-type-text field-name-field-assignment field-widget-text-textfield form-wrapper">
                <div class="read-only"><?php print htmlspecialchars($title, ENT_QUOTES); ?></div>
            </div>
            <div class="field-type-number-integer field-name-field-percent field-widget-number form-wrapper">
                <div class="read-only"></div>
            </div>
            <div class="field-type-list-boolean field-name-field-completed field-widget-options-onoff form-wrapper">
                <div class="read-only">
                    <input type="checkbox" id="schedule-completed-<?php print $eid; ?>" <?php
                    print (isset($event->field_completed['und'][0]['value']) && $event->field_completed['und'][0]['value'] ? 'checked="checked"' : ''); ?> />
                    <label for="schedule-completed-<?php print $eid; ?>">&nbsp;</label></div>
            </div>
            <input type="hidden" name="plan-path" value="<?php print url('node/plup/plan', array('query' => array('plupload_token' => drupal_get_token('plupload-handle-uploads')))); ?>" />
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

