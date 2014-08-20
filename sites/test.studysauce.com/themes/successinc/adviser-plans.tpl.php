<?php
drupal_add_css(drupal_get_path('theme', 'successinc') . '/plans.css');
drupal_add_js(drupal_get_path('theme', 'successinc') . '/js/plans.js');
drupal_add_css(drupal_get_path('theme', 'successinc') . '/js/fullcalendar/fullcalendar.css');
drupal_add_js(drupal_get_path('theme', 'successinc') . '/js/fullcalendar/lib/moment.min.js');
drupal_add_js(drupal_get_path('theme', 'successinc') . '/js/fullcalendar/fullcalendar.js');
// check if user has purchased a plan
if (!isset($account)) {
    global $user;
    $account = user_load($user->uid);
}

list($events, $classes) = studysauce_get_events($account);
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
    {
        $label = 'REMINDER';
        continue;
    }
    elseif($x->field_event_type['und'][0]['value'] == 'm')
    {
        $label = 'MEAL';
        continue;
    }
    elseif($x->field_event_type['und'][0]['value'] == 'z')
    {
        $label = 'SLEEP';
        continue;
    }

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
        'editable' => false,
        'dates' => isset($dates) ? $dates : null
    );
}
$studysauceExportEvents = $jsEvents;


$strategies = studysauce_get_strategies($account);

// on mobile only show event between this week, hide everything else unless the user clicks View historic
$startWeek = strtotime(date("Y-m-d", strtotime('this week', time()))) - 86400;
$endWeek = $startWeek + 604800 - 86400;
//$dotwStr = date('l', strtotime($event['start']));
?>

<h2>Study schedule</h2>
<?php if (count($classes)): ?>
    <div id="calendar" class="full-only"></div>
    <script type="text/javascript">

        window.planEvents = <?php print json_encode($jsEvents); ?>; // convert events array to object to keep track of keys better
        window.strategies = <?php print json_encode($strategies); ?>;
    </script>
    <div class="sort-by">
        <label>Sort by: </label>
        <input type="radio" id="schedule-by-date" name="schedule-by" value="date" checked="checked"><label
            for="schedule-by-date">Date</label>&nbsp;
        <input type="radio" id="schedule-by-class" name="schedule-by" value="class"><label
            for="schedule-by-class">Class</label>
        <input type="checkbox" id="schedule-historic"><label for="schedule-historic">View historical</label>
    </div>
    <?php
    print theme('studysauce-adviser-strategies');

    $first = true;
    $headStr = '';
    $nowTimestamp = date_time_set(new DateTime('now', new DateTimeZone(date_default_timezone_get())), 0, 0, 0)->getTimestamp();
    foreach ($events as $eid => $event)
    {
        if($eid == '') continue;

        // TODO: should we allow notes for class events?
        if(isset($event->field_event_type['und'][0]['value']) &&
            ($event->field_event_type['und'][0]['value'] == 'c' ||
                $event->field_event_type['und'][0]['value'] == 'h' ||
                $event->field_event_type['und'][0]['value'] == 'r' ||
                $event->field_event_type['und'][0]['value'] == 'm' ||
                $event->field_event_type['und'][0]['value'] == 'z'
            ))
            continue;

        $time = new DateTime($event->field_time['und'][0]['value'], new DateTimeZone('UTC'));
        $time->setTimezone(new DateTimeZone(date_default_timezone_get()));
        $newHead = $time->format('j F');
        if($headStr != $newHead)
        {
            $headStr = $newHead;
            ?><div class="head <?php print ($time->getTimestamp() < $nowTimestamp - 86400 ? 'hide' : ''); ?> <?php
                print (strtotime($event->field_time['und'][0]['value']) >= $startWeek && strtotime($event->field_time['und'][0]['value']) <= $endWeek ? 'mobile' : ''); ?>"><?php print $headStr; ?></div><?
        }
        $cid = array_search($event->field_class_name['und'][0]['value'], array_map(function ($x) { return $x->field_class_name['und'][0]['value']; }, $classes));
        if($cid === false)
            $cid = '';
        $classI = array_search($cid, array_keys($classes));
        if($classI === false)
            $classI = '';

        $session = isset($strategies[$event->field_class_name['und'][0]['value']]) &&
        array_search(true, array_map(function ($x) {
            return $x['default'];
        }, $strategies[$event->field_class_name['und'][0]['value']])) !== false
            // check if the strategy default is saved
            ? array_search(true, array_map(function ($x) {
                return $x['default'];
            }, $strategies[$event->field_class_name['und'][0]['value']]))
            // if this is a deadline or not a class event, show notes box
            : ($event->field_event_type['und'][0]['value'] == 'd' || $classI == ''
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
                                : '')))));

        if($event->field_event_type['und'][0]['value'] == 'd' && $cid != '')
            $title = 'Deadline' . preg_replace(array('/' . preg_quote($classes[$cid]->field_class_name['und'][0]['value']) . '\s*/'), array(''), $event->field_class_name['und'][0]['value']);
        elseif($event->field_event_type['und'][0]['value'] == 'd')
            $title = 'Deadline' . str_replace('Nonacademic', '', $event->field_class_name['und'][0]['value']);
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
        print (isset($event->field_completed['und'][0]['value']) && $event->field_completed['und'][0]['value'] ? 'done' : ''); ?>"
             id="eid-<?php print $eid; ?>">
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
                    <span class="<?php print (isset($event->field_completed['und'][0]['value']) && $event->field_completed['und'][0]['value'] ? 'checked' : ''); ?>">&nbsp;</span></div>
            </div>
            <input type="hidden" name="plan-path"
                   value="<?php print url('node/plup/plan', array('query' => array('plupload_token' => drupal_get_token('plupload-handle-uploads')))); ?>"/>
        </div>
    <?php
    }
    ?>

<?php endif;
if (!count($classes)): ?>
    <h3>Your student has not completed this section yet.</h3>
<?php endif; ?>
<p style="clear: both; margin-bottom:0; line-height:1px;">&nbsp;</p>
