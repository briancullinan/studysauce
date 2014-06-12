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
if($lastOrder)
    list($events, $node, $classes, $entities) = studysauce_get_events($account, $lastOrder ? $lastOrder->created : null);
else
{
    $sample = theme('studysauce-plan-sample');
    $encoded = preg_replace('/<\/?script>/i', '', $sample);
    list($events, $classes, $entities) = json_decode($encoded);
    $entities = array_map(function ($x) {
        return (object) array('field_study_type' => array('und' => array(0 => array('value' => $x)))); }, (array)$entities);
    $classes = (array) $classes;
    foreach($classes as $i => $c)
        $classes[intval($i)] = $c;
    $events = array_map(function ($x) { return (array)$x; }, (array) $events);
}

$strategies = studysauce_get_strategies($account);


// on mobile only show event between this week, hide everything else unless the user clicks View historic
$startWeek = strtotime(date("Y-m-d", strtotime('this week', time()))) - 86400;
$endWeek = $startWeek + 604800 - 86400;
//$dotwStr = date('l', strtotime($event['start']));
?>

<h2>Study schedule</h2>
<div id="calendar" class="full-only"></div>
<script type="text/javascript">

    window.planEvents = <?php print json_encode($events); ?>; // convert events array to object to keep track of keys better
    window.strategies = <?php print json_encode($strategies); ?>;
</script>
<div class="sort-by">
    <label>Sort by: </label>
    <input type="radio" id="schedule-by-date" name="schedule-by" checked="checked"><label for="schedule-by-date">Date</label>&nbsp;
    <input type="radio" id="schedule-by-class" name="schedule-by"><label for="schedule-by-class">Class</label>
    <input type="checkbox" id="schedule-historic"><label for="schedule-historic">View historical</label>
</div>
<?php
print theme('studysauce-adviser-strategies');

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
    $session = (isset($entities[$cid]->field_study_type['und'][0]['value']) && strpos($event['className'], 'deadline-event') === false
        ? ($entities[$cid]->field_study_type['und'][0]['value'] == 'memorization'
            ? 'spaced'
            : ($entities[$cid]->field_study_type['und'][0]['value'] == 'reading'
                ? 'active'
                : ($entities[$cid]->field_study_type['und'][0]['value'] == 'conceptual'
                    ? 'teach'
                    : '')))
        : ($classI == '' || strpos($event['className'], 'deadline-event') !== false
            ? 'other'
            : ''));

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
    print ((strpos($event['className'], 'deadline-event') !== false && isset($event['percent']) && !empty($event['percent']) && intval($event['percent']) > 0) ? 'deadline' : ''); ?> <?php
    print ($time < strtotime(date('Y/m/d')) - 86400 ? 'hide' : ''); ?> <?php
    print (strtotime($event['start']) >= $startWeek && strtotime($event['start']) <= $endWeek ? 'mobile' : ''); ?> <?php
    print ('class' . $classI); ?> <?php
    print ('cid' . $cid); ?> <?php
    print ('default-' . $session); ?>" id="eid-<?php print $eid; ?>">
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
                <input type="checkbox" id="schedule-completed-<?php print $eid; ?>" />
                <label for="schedule-completed-<?php print $eid; ?>">&nbsp;</label></div>
        </div>
        <input type="hidden" name="plan-path" value="<?php print url('node/plup/plan', array('query' => array('plupload_token' => drupal_get_token('plupload-handle-uploads')))); ?>" />
        <input type="hidden" name="plan-title" value="<?php print $event['title']; ?>" />
    </div>
<?php
}
?>
<p style="clear: both; margin-bottom:0; line-height:1px;">&nbsp;</p>
