<?php
drupal_add_css(drupal_get_path('theme', 'successinc') .'/metrics.css');
drupal_add_css(drupal_get_path('theme', 'successinc') .'/adviser-metrics.css');
drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/metrics.js');

if(!isset($account))
{
    global $user;
    $account = user_load($user->uid);
}

$classesNames = _studysauce_get_schedule_classes($account);
list($times, $rows, $total, $hours) = _studysauce_get_metrics($account);

// move dates automatically in empty account and demo accounts
if(in_array('demo', $account->roles)):
    $classesNames = array();

    // load times from fake data
    $sample = theme('studysauce-metrics-sample');
    $encoded = preg_replace('/<\/?script>/i', '', $sample);
    list($times, $rows, $total, $hours) = json_decode($encoded);

    $timeGroups = array();
    $times = (array)$times;

    // recalculate times to be current week
    $last = (array)end($times);
    $last = $last['time'];
    $first = (array)reset($times);
    $first = $first['time'];
    $range = max(1, $last - $first);
    $recent = time() - 60*60*24*7*4;

    // sort by class then time
    $classes = array_map(function ($x) use (&$classesNames) {
        $x = (array)$x;
        if(($c = array_search($x['class'], array_values($classesNames))) === false)
        {
            $c = count($classesNames);
            $classesNames[] = $x['class'];
        }
        return $c;
    }, (array)$times);
    $ts = array_map(function ($x) {
        $x = (array)$x;
        return $x['time'];
    }, (array)$times);
    array_multisort($classes, SORT_NUMERIC, SORT_ASC, $ts, SORT_NUMERIC, SORT_ASC, $times);

    $times = array_map(function ($x) use ($recent, $first, $range, $classesNames, &$timeGroups) {
        $x = (array)$x;
        $x['time'] = ($x['time'] - $first) / $range * 60*60*24*7*5 + $recent;
        $c = intval(array_search($x['class'], $classesNames));

        // recalculate metrics positions
        $date = new DateTime();
        $date->setTimestamp($x['time']);
        $date->setTime(0, 0, 0);
        $date = $date->add( new DateInterval('P1D') );
        $g = $date->format('W');
        if(!isset($timeGroups[$g][$c][0]))
        {
            $length0 = 0;
        }
        else
        {
            $length0 = array_sum($timeGroups[$g][$c]);
        }
        $timeGroups[$g][$c][] = $x['length'];
        $x['length0'] = $length0;
        return $x;
    }, $times);

    if(empty($times))
    {
        ?>
        <script type="text/javascript">
            jQuery(document).ready(function () {
                jQuery('#metrics').addClass('empty');
            });
        </script>
    <? }
endif;

?><h2>Study metrics</h2><?php

if(empty($times)):
    ?>
    <script type="text/javascript">
        jQuery(document).ready(function () {
            jQuery('#metrics').addClass('empty');
        });
    </script>
    <h3>Your student has not completed this section yet.</h3>
<? endif;
if(!empty($times)):
?>
    <div class="centrify">
        <div id="legend">
            <ol>
                <?php
                foreach($classesNames as $cid => $c)
                {
                    ?><li><span class="class<?php print array_search($cid, array_keys($classesNames)); ?>">&nbsp;</span><?php print $c; ?></li><?php
                }
                ?>
            </ol>
        </div>
        <div id="timeline">
            <h3>Study hours by week</h3>
            <h4 style="margin:5px 0; color:#555;"><?php print ($hours > 0 ? ('Goal: ' . $hours . ' hours') : '&nbsp;'); ?></h4>
        </div>
        <div id="pie-chart">
            <h3>Study hours by class</h3>
            <h4 style="margin:5px 0; color:#555;">Total study hours: <strong id="study-total"><?php print $total; ?></strong></h4>
        </div>
    </div>
    <hr/>
    <script type="text/javascript">
        window.initialHistory = <?php print json_encode($times); ?>;
        window.classNames = <?php print json_encode(array_values($classesNames)); ?>;
    </script>
    <div id="checkins-list">
        <div class="heading row">
            <label class="class-name">Class</label>
            <label class="class-date"><span class="full-only">Check in date</span><span class="mobile-only">Date</span></label>
            <label class="class-time">Duration</label>
        </div>
        <?php print $rows; ?>
    </div>
<?php endif; ?>

<hr/>
