<?php

function _studysauce_get_metrics()
{
    global $checkins, $checkouts, $classes;
    $hours = _studysauce_get_incentives_hours();
    if(!isset($checkins) || !is_array($checkins) || !isset($checkouts) || !is_array($checkouts))
        return array(array(), '', '0 hours', $hours);
    $shortTimeIntervals = array(
        'years' => 'yr',
        'year' => 'yr',
        'months' => 'mo',
        'month' => 'mo',
        'days' => 'day',
        'day' => 'day',
        'hours' => 'hr',
        'hour' => 'hrs',
        'minutes' => 'min',
        'minute' => 'min',
        'seconds' => 'sec',
        'second' => 'sec'
    );
    reset($checkouts);
    $times = array();
    $timeGroups = array();
    $rows = array();
    $total = 0;
    foreach($checkins as $t => $c)
    {
        list($k, $v) = each($checkouts);

        $length = $k - $t;
        if($length <= 60)
            $length = 60;

        $lengthStr = _studysauce_time_elapsed_string($length);
        $rows[$t] = '
            <div class="row">
                <div class="class-name"><span class="class' . $c . '">&nbsp;</span>' . array_values($classes)[$c] . '</div>
                <div class="class-date"><span class="full-only">' . date('j F', $t) . '</span><span class="mobile-only">' . date('j M', $t) . '</span></div>
                <div class="class-time"><span class="full-only">' . $lengthStr . '</span><span class="mobile-only">' . str_replace(array_keys($shortTimeIntervals), array_values($shortTimeIntervals), $lengthStr) . '</span></div>
            </div>
            ';

        // since we are already in order by time, sum up the other lengths on this day
        $date = new DateTime();
        $date->setTimestamp($t);
        $date->setTime(0, 0, 0);
        $g = $date->format('W');
        if(!isset($timeGroups[$g][$c][0]))
        {
            $length0 = 0;
        }
        else
        {
            $length0 = array_sum($timeGroups[$g][$c]);
        }
        $timeGroups[$g][$c][] = $length;

        $times[] = array(
            'time' => $t,
            'length' => $length,
            'length0' => $length0,
            'class' => array_values($classes)[$c]);

        $total += $length;
    }
    krsort($rows);
    $rowsOutput = '';
    foreach($rows as $t => $row)
        $rowsOutput .= $row;

    $total = round($total / 3600, 1);
    if($total <> 1)
        $total .= ' hours';
    else
        $total .= ' hour';

    return array($times, $rowsOutput, $total, $hours);
}
