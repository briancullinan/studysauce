<?php

function studysauce_clean_checkins($classes)
{
    $checkins = array();
    $checkouts = array();
    $countI = 0;
    foreach($classes as $i => $class)
    {
        if(isset($class->field_class_name[LANGUAGE_NONE][0]['value']) &&
            !empty($class->field_class_name[LANGUAGE_NONE][0]['value']) &&
            (!isset($class->field_event_type['und'][0]['value']) || $class->field_event_type['und'][0]['value'] == 'c'))
        {

            if(isset($class->field_checkin[LANGUAGE_NONE]) &&
                is_array($class->field_checkin[LANGUAGE_NONE]))
            {
                foreach($class->field_checkin[LANGUAGE_NONE] as $j => $checkin)
                {

                    // Create a list of valid checkin times for all classes
                    if(empty($checkin['value']))
                        continue;

                    $time = strtotime(trim($checkin['value']));
                    if($time == 0)
                        continue;
                    $checkins[$time] = $countI;

                    // add checkout times
                    if(isset($checkin['value2']) && !empty($checkin['value2']) &&
                        strtotime(trim($checkin['value2'])) > 0)
                    {
                        $checkouts[strtotime(trim($checkin['value2']))] = $countI;
                    }
                    $checkouts[min(time(), $time + 3600)] = $countI;
                }
            }

            $countI++;
        }
    }

    ksort($checkins);

    // if the checkin time is before the last checkout time then change the checkout time to match checkin time,
    //    they switched classes in the middle of the session
    $resultCheckouts = array();
    foreach($checkins as $i => $class)
    {
        $diffs = array();
        foreach(($checkins + $checkouts) as $k => $c)
            $diffs[$k] = $k - $i;

        asort($diffs);

        foreach($diffs as $t => $length)
            if($length > 0)
            {
                if($length < 60)
                    $length = 60;

                $resultCheckouts[$t] = $length;
                break;
            }
    }

    // Clean the list of checkins:
    //    1) exclude checkins too close to each other < 60 seconds
    //    2) create list of checkouts based on checkin times or assume 3600 seconds passed
    /*$lastClass = -1;
    $lastTime = 0;
    $clean = array();
    foreach($checkins as $t => $c)
    {
        if($lastTime > 0 && $t - $lastTime < 60 && $lastClass == $c)
            continue;
        else
            $clean[$t] = $c;

        $lastTime = $t;
        $lastClass = $c;
    }
    $checkins = $clean;
*/
    reset($checkouts);

    return array(0 => $checkins, 1 => $resultCheckouts, 2 => $classes, 'checkins' => $checkins, 'checkouts' => $resultCheckouts);
}

function _studysauce_get_metrics($account = null)
{
    if($account == null)
    {
        global $user;
        $account = user_load($user->uid);
    }
    $classesNames = _studysauce_get_schedule_classes($account);
    $classes = array();
    foreach($classesNames as $eid => $c)
    {
        $entity = entity_load('field_collection_item', array($eid));
        $classes[$eid] = $entity[$eid];
    }

    list($checkins, $checkouts) = studysauce_clean_checkins($classes);

    $hours = _studysauce_get_incentives_hours($account);
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
    $times = array();
    $timeGroups = array();
    $rows = array();
    $total = 0;
    foreach($checkins as $t => $c)
    {
        list($k) = each($checkouts);

        $length = $k - $t;
        if($length <= 60)
            $length = 60;

        $lengthStr = _studysauce_time_elapsed_string($length);
        $rows[$t] = '
            <div class="row">
                <div class="class-name"><span class="class' . $c . '">&nbsp;</span>' . array_values($classesNames)[$c] . '</div>
                <div class="class-date"><span class="full-only">' . date('j F', $t) . '</span><span class="mobile-only">' . date('j M', $t) . '</span></div>
                <div class="class-time"><span class="full-only">' . $lengthStr . '</span><span class="mobile-only">' . str_replace(array_keys($shortTimeIntervals), array_values($shortTimeIntervals), $lengthStr) . '</span></div>
            </div>
            ';

        // since we are already in order by time, sum up the other lengths on this day
        $date = new DateTime();
        $date->setTimestamp($t);
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
        $timeGroups[$g][$c][] = $length;

        $times[] = array(
            'time' => $t,
            'length' => $length,
            'length0' => $length0,
            'class' => array_values($classesNames)[$c]);

        $total += $length;
    }
    krsort($rows);
    $rowsOutput = '';
    foreach($rows as $row)
        $rowsOutput .= $row;

    $total = round($total / 3600, 1);
    if($total <> 1)
        $total .= ' hours';
    else
        $total .= ' hour';

    return array($times, $rowsOutput, $total, $hours);
}
