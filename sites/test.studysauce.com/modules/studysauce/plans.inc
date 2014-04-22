<?php

function studysauce_save_calendar()
{
    global $user;

    $query = new EntityFieldQuery();
    $nodes = $query->entityCondition('entity_type', 'node')
        ->propertyCondition('type', 'schedule')
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
    else
    {
        $node = new stdClass();
        $node->title = isset($user->mail) ? $user->mail : '';
        $node->type = 'schedule';
        $node->language = LANGUAGE_NONE; // Or e.g. 'en' if locale is enabled
        $node->uid = $user->uid;
        $node->status = 1; //(1 or 0): published or not
        $node->revision = 1;
        node_object_prepare($node); // Sets some defaults. Invokes hook_prepare() and hook_node_prepare().
        $node = node_submit($node); // Prepare node for saving
        node_save($node);
    }

    if(isset($_POST['university']))
        $node->field_university['und'][0]['value'] = $_POST['university'];
    if(isset($_POST['grades']))
        $node->field_grades['und'][0]['value'] = $_POST['grades'];
    if(isset($_POST['weekends']))
        $node->field_weekends['und'][0]['value'] = $_POST['weekends'];
    if(isset($_POST['6-am-11-am']))
        $node->field_6_am_11_am['und'][0]['value'] = $_POST['6-am-11-am'];
    if(isset($_POST['11-am-4-pm']))
        $node->field_11_am_4_pm['und'][0]['value'] = $_POST['11-am-4-pm'];
    if(isset($_POST['4-pm-9-pm']))
        $node->field_4_pm_9_pm['und'][0]['value'] = $_POST['4-pm-9-pm'];
    if(isset($_POST['9-pm-2-am']))
        $node->field_9_pm_2_am['und'][0]['value'] = $_POST['9-pm-2-am'];

    if(isset($_POST['className']) && isset($_POST['type']) && isset($_POST['dotw']) &&
        isset($_POST['start']) && isset($_POST['end']))
    {
        list($events, $node, $classes, $entities) = studysauce_get_events();

        // check if class entity already exists by name
        $saved = array();
        // TODO: make this rely on entity id instead?
        foreach($entities as $i => $entity)
        {
            if(isset($entity->field_class_name[LANGUAGE_NONE][0]['value']))
            {
                $saved[$entity->item_id] = $entity->field_class_name[LANGUAGE_NONE][0]['value'];
                if($entity->field_class_name[LANGUAGE_NONE][0]['value'] == $_POST['className'])
                    $thisEid = $entity->item_id;
            }
        }
        if(!isset($thisEid))
        {
            $entity = entity_create('field_collection_item', array('field_name' => 'field_classes'));
            $entity->setHostEntity('node', $node);
            $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $_POST['className'];
        }
        else
            $entity = $entities[$thisEid];
        $entity->revision = 1;
        $entity->field_day_of_the_week[LANGUAGE_NONE] = array_map(function ($x) { return array('value' => $x); }, explode(',', $_POST['dotw']));
        $entity->field_event_type[LANGUAGE_NONE][0]['value'] = $_POST['type'];
        $entity->field_time[LANGUAGE_NONE][0] = array(
            'value' => gmdate('Y-m-d H:i:s', strtotime($_POST['start'])),
            'value2' => gmdate('Y-m-d H:i:s', strtotime($_POST['end']))
        );
        $entity->save();

        // modify later events of the same type
        if(isset($_POST['className']) && isset($_POST['type']) &&
            ($_POST['type'] == 'p' || $_POST['type'] == 'sr' || $_POST['type'] == 'f'))
        {
            $i = array_search(preg_replace('/^\(P\)\s*|^\(SR\)\s*|^\(F\)\s*|\s*\([0-9\/,\s-]+\)$|\s*<small>[0-9\/,\s-]+<\/small>$/i', '', $entity->field_class_name['und'][0]['value']), array_values($classes));
            foreach($events as $e => $evt)
            {
                // if the event is after the one that was moved
                if(strtotime($evt['start']) > $_POST['end'] &&
                // if the type of event is the same
                    strpos($evt['className'], 'class' . $i) !== false)
                    // TODO: check if we are crossing over any event of the same class
                    if($_POST['type'] == 'p' && substr($evt['title'], 0, 3) == '(P)' ||
                        $_POST['type'] == 'sr' && substr($evt['title'], 0, 4) == '(SR)' ||
                        $_POST['type'] == 'f' && substr($evt['title'], 0, 3) == '(F)')
                    {
                        if(($eid = array_search($evt['title'], $saved)) !== false)
                        {
                        //    $entities[$eid]->revision = 1;
                        //    $entities[$eid]->field_event_type[LANGUAGE_NONE][0]['value'] = $_POST['type'];
                        //    $entities[$eid]->field_time[LANGUAGE_NONE][0] = array(
                        //        'value' => gmdate('Y-m-d H:i:s', strtotime($_POST['start'])),
                        //        'value2' => gmdate('Y-m-d H:i:s', strtotime($_POST['end']))
                        //    );
                        }
                        else
                        {
                        //    $entity = entity_create('field_collection_item', array('field_name' => 'field_classes'));
                        //    $entity->setHostEntity('node', $node);
                        //    $entity->revision = 1;
                        //    $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $evt['title'];
                        //    $entity->field_event_type[LANGUAGE_NONE][0]['value'] = $_POST['type'];
                        //    $entity->field_time[LANGUAGE_NONE][0] = array(
                        //        'value' => gmdate('Y-m-d H:i:s', strtotime($_POST['start'])),
                        //        'value2' => gmdate('Y-m-d H:i:s', strtotime($_POST['end']))
                        //    );
                        }
                    }
            }
        }
    }

    if(isset($_POST['remove']))
    {
        foreach($node->field_classes['und'] as $i => $c)
        {
            if($c['value'] == $_POST['remove'])
            {
                entity_delete('field_collection_item', array($c['value']));
            }
        }
    }

    $node = node_submit($node); // Prepare node for saving
    node_save($node);

    // get event schedule
    list($events, $node, $classes, $entities) = studysauce_get_events($o ? $o->created : null);

    $newSDS = studysauce_get_sds_messages();
    $lastSDS = end($newSDS);
    list($awards) = studysauce_get_awards();
    list($times, $rows, $total, $hours) = _studysauce_get_metrics();

    print json_encode(array(
        'events' => $events,
        'classes' => $classes,
        'schedule' => '<div>' . theme('studysauce-schedule') . '</div>',
        'plan' => '<div>' . theme('studysauce-plans') . '</div>',
        'lastSDS' => $lastSDS,
        'awards' => $awards,
        'rows' => $rows,
        'times' => $times,
        'total' => $total,
        'hours' => $hours,
    ));
    drupal_exit();
}

function studysauce_get_events($lastOrderTime = null)
{
    global $user;

    if(!isset($lastOrderTime) || empty($lastOrderTime))
    {
        if(!isset($orders))
        {
            $orders = _studysauce_orders_by_uid($user->uid);
            $conn = studysauce_get_connections();
            foreach ($conn as $i => $c)
                $orders = array_merge($orders, _studysauce_orders_by_uid($c->uid));
        }
        $o = end($orders);
        if(isset($o) && isset($o->created))
            $lastOrderTime = $o->created;
    }

    // get the schedule with university filled in
    $query = new EntityFieldQuery();
    $nodes = $query->entityCondition('entity_type', 'node')
        ->propertyCondition('type', 'schedule')
        ->propertyCondition('title', isset($user->mail) ? $user->mail : '')
        ->propertyCondition('status', 1)
        ->range(0, 1)
        ->execute();
    if (!empty($nodes['node'])) {
        $nodes = array_keys($nodes['node']);
        $nid = array_shift($nodes);
        $node = node_load($nid);

        $classes = array();
        $saved = array();
        $events = array();
        $entities = array();
        $startTerm = 0;
        $endTerm = 0;
        if (isset($node->field_classes[LANGUAGE_NONE][0]))
        {
            foreach ($node->field_classes[LANGUAGE_NONE] as $i => $class)
            {
                $eid = $class['value'];
                $entity = entity_load('field_collection_item', array($eid));
                if (!empty($entity))
                {
                    if (isset($entity[$eid]->field_class_name['und'][0]['value']) && !empty($entity[$eid]->field_class_name['und'][0]['value'])) {

                        // list classes the same way the checkin and key dates tab does
                        if(!isset($entity[$eid]->field_event_type['und'][0]['value']) || $entity[$eid]->field_event_type['und'][0]['value'] == 'c')
                            $classes[$eid] = $entity[$eid]->field_class_name['und'][0]['value'];

                        // this is use by schedule only, if everything is filled in display the event
                        if (isset($entity[$eid]->field_day_of_the_week['und'][0]['value']) && isset($entity[$eid]->field_time['und'][0]['value']) &&
                            isset($entity[$eid]->field_time['und'][0]['value2'])
                        ) {
                            $entities[$eid] = $entity[$eid];
                            if ($startTerm == 0 || strtotime($entity[$eid]->field_time['und'][0]['value']) < $startTerm)
                                $startTerm = strtotime($entity[$eid]->field_time['und'][0]['value']);
                            if ($endTerm == 0 || strtotime($entity[$eid]->field_time['und'][0]['value2']) > $endTerm)
                                $endTerm = strtotime($entity[$eid]->field_time['und'][0]['value2']);
                            if(isset($entity[$eid]->field_event_type['und'][0]['value']) && (
                                    $entity[$eid]->field_event_type['und'][0]['value'] == 'p' ||
                                    $entity[$eid]->field_event_type['und'][0]['value'] == 'sr' ||
                                    $entity[$eid]->field_event_type['und'][0]['value'] == 'f'))
                                $saved[$eid] = $entity[$eid]->field_class_name['und'][0]['value'];
                        }
                    }
                }
            }
        }


        $startTerm = strtotime(date("Y-m-d", strtotime('this week', $startTerm)));
        $endTerm = strtotime(date("Y-m-d", strtotime('this week', $endTerm))) + 604800;
        $startWeek = strtotime(date("Y-m-d", strtotime('this week', time()))) - 86400;
        for ($w = $startTerm; $w < $endTerm; $w += 604800)
        {
            $totalLength = 0;
            $studyLength = 0;
            foreach ($entities as $eid => $entity)
            {
                if(!isset($entity->field_event_type['und'][0]['value']) || $entity->field_event_type['und'][0]['value'] == 'c')
                {
                    $i = array_search($eid, array_keys($classes));
                    $classStart = strtotime($entity->field_time['und'][0]['value']);
                    $classEnd = strtotime($entity->field_time['und'][0]['value2']);
                    foreach ($entity->field_day_of_the_week['und'] as $j => $dotw)
                    {
                        $d = 0;
                        $skip = false;
                        switch ($dotw['value'])
                        {
                            case 'M':
                                break;
                            case 'Tu':
                                $d = 86400;
                                break;
                            case 'W':
                                $d = 86400 * 2;
                                break;
                            case 'Th':
                                $d = 86400 * 3;
                                break;
                            case 'F':
                                $d = 86400 * 4;
                                break;
                            case 'Sa':
                                $d = 86400 * 5;
                                break;
                            case 'Su':
                                $d = 86400 * 6;
                                break;
                            default:
                                $skip = true;
                        }
                        if($skip)
                            continue;

                        $startDay = strtotime(date("Y-m-d", $classStart));
                        $t = strtotime(date("Y-m-d", strtotime('this week', $w))) + $d;
                        if($t < $classStart || $t > $classEnd)
                            continue;

                        // calculate hours spent in class and remaining free time
                        $length = strtotime(date('1/1/1970 H:i:s', $classEnd)) - strtotime(date('1/1/1970 H:i:s', $classStart));
                        if($length < 0)
                            $length += 86400;
                        $totalLength += max(min(14400, $length), 3600);

                        $classT = strtotime(date('Y/m/d', $t) . ' ' . date('H:i:s', $classStart));
                        $events[] = array(
                            'editable' => false,
                            'title' => '<h4>CLASS</h4>' . $entity->field_class_name['und'][0]['value'],
                            'start' => date('Y/m/d H:i:s', $classT) . ' UTC',
                            'end' => date('Y/m/d H:i:s', $classT + $length) . ' UTC',
                            'className' => 'class-event class' . $i,
                            'allDay' => false,
                            'rendered' => '
                                        <div class="row">
                                            <div class="class-name"><span class="class' . $i . '">&nbsp;</span>' . $entity->field_class_name['und'][0]['value'] . '</div>
                                            <div class="class-date"><span class="full-only">' . date('h:i A', $classStart) . ' - ' . date('h:i A', $classEnd) . '</span><span class="mobile-only">' . date('h:i A', $classStart) . ' - ' . date('h:i A', $classEnd) . '</span></div>
                                        </div>
                                        ');

                        // only show calculated events if it comes before 4 weeks after the last purchase date
                        // TODO: uncomment this feature when new reoccurring product is done
                        //if ($t > $lastOrderTime + 2419200)
                        if ($t > $startWeek + 2419200)
                            continue;

                        $pTitle = '(P) ' . $entity->field_class_name['und'][0]['value'] . ' <small>' . date('n/d', $t) . '</small>';
                        if(!in_array($pTitle, $saved))
                        {
                            // add prework
                            $preT = strtotime(date('Y/m/d', $t) . ' ' . date('H:i:s', $classStart)) - 86400;
                            $studyLength += 3600; // study hours automatically taken up by this week
                            //if((date('l', $preT) == 'Sunday' || date('l', $preT) == 'Saturday') &&
                            //    isset($node->field_weekends['und'][0]['value']) && $node->field_weekends['und'][0]['value'] == 'hit_hard')
                            //    $preT += date('l', $preT) == 'Sunday' ? (86400 * 5) : (86400 * 6); // prepare for next week's class
                            $events[] = array(
                                'title' => $pTitle,
                                'start' => date('Y/m/d H:i:s', $preT) . '  UTC',
                                'end' => date('Y/m/d H:i:s', $preT + 3600) . '  UTC',
                                'className' => 'class' . $i,
                                'allDay' => false,
                                'rendered' => '
                                            <div class="row">
                                                <div class="class-name"><span class="class' . $i . '">&nbsp;</span>' . $pTitle . '</div>
                                                <div class="class-date">&nbsp;</div>
                                            </div>
                                            ');
                        }

                        // add spaced repetition 7 days, 14 days, 28 days
                        $srTitle = '(SR) ' . $entity->field_class_name['und'][0]['value'] . ' <small>' .
                            date('n/d', $t) .
                            ($t - 86400 * 7 >= $startDay ? (', ' . date('n/d', $t - 86400 * 7)) : '') .
                            ($t - 86400 * 14 >= $startDay ? (', ' . date('n/d', $t - 86400 * 14)) : '') .
                            ($t - 86400 * 28 >= $startDay ? (', ' . date('n/d', $t - 86400 * 28)) : '') . '</small>';
                        if(!in_array($srTitle, $saved))
                        {
                            $spT = strtotime(date('Y/m/d', $t) . ' ' . date('H:i:s', $classStart)) + 3600 * 2;
                            $studyLength += 3600; // study hours automatically taken up by this week
                            $events[] = array(
                                'title' => $srTitle,
                                'start' => date('Y/m/d H:i:s', $spT) . '  UTC',
                                'end' => date('Y/m/d H:i:s', $spT + 3600) . '  UTC',
                                'className' => 'class' . $i,
                                'allDay' => false,
                                'rendered' => '
                                            <div class="row">
                                                <div class="class-name"><span class="class' . $i . '">&nbsp;</span>' . $srTitle . '</div>
                                                <div class="class-date">&nbsp;</div>
                                            </div>
                                            ');
                        }
                    }
                }
                elseif($entity->field_event_type['und'][0]['value'] == 'o')
                {
                    $classStart = strtotime($entity->field_time['und'][0]['value']);
                    $classEnd = strtotime($entity->field_time['und'][0]['value2']);

                    foreach ($entity->field_day_of_the_week['und'] as $j => $dotw)
                        if($dotw['value'] == 'Monthly')
                        {
                            // adjust the entered date for the current month
                            $length = strtotime(date('1/1/1970 H:i:s', $classEnd)) - strtotime(date('1/1/1970 H:i:s', $classStart));
                            if($length <= 0)
                                $length += 86400;
                            $startStr = date('m', $w + 604800) . date('/d/', $classStart) . date('Y', $w + 604800) . ' ' . date('H:i:s', $classStart);
                            $classStart = strtotime($startStr);
                            $classEnd = $classStart + $length;
                        }

                    if($classStart >= $w && $classStart <= $w + 604800)
                    {
                        $events[] = array(
                            'editable' => false,
                            'title' => $entity->field_class_name['und'][0]['value'],
                            'start' => date('Y/m/d H:i:s', $classStart) . ' UTC',
                            'end' => date('Y/m/d H:i:s', $classEnd) . ' UTC',
                            'className' => 'other-event',
                            'allDay' => false,
                            'rendered' => '
                                            <div class="row">
                                                <div class="class-name"><span class="">&nbsp;</span>' . $entity->field_class_name['und'][0]['value'] . '</div>
                                                <div class="class-date"><span class="full-only">' . date('h:i A', $classStart) . ' - ' . date('h:i A', $classEnd) . '</span><span class="mobile-only">' . date('h:i A', $classStart) . ' - ' . date('h:i A', $classEnd) . '</span></div>
                                            </div>
                                            ');
                    }
                }
                elseif($entity->field_event_type['und'][0]['value'] == 'p' ||
                    $entity->field_event_type['und'][0]['value'] == 'sr' ||
                    $entity->field_event_type['und'][0]['value'] == 'f')
                {
                    $classStart = strtotime($entity->field_time['und'][0]['value']);
                    $classEnd = strtotime($entity->field_time['und'][0]['value2']);
                    if($classStart >= $w && $classStart <= $w + 604800)
                    {
                        $studyLength += 3600; // add to study length if the user has moved their study event
                        $i = array_search(
                            // extract class name from title by replacing everything else
                            preg_replace('/^\(P\)\s*|^\(SR\)\s*|^\(F\)\s*|\s*\([0-9\/,\s-]+\)$|\s*<small>[0-9\/,\s-]+<\/small>$/i', '', $entity->field_class_name['und'][0]['value']), array_values($classes));
                        $events[] = array(
                            'title' => $entity->field_class_name['und'][0]['value'],
                            'start' => date('Y/m/d H:i:s', $classStart) . ' UTC',
                            'end' => date('Y/m/d H:i:s', $classEnd) . ' UTC',
                            'className' => $entity->field_event_type['und'][0]['value'] == 'f' ? 'free-event' : 'class' . $i,
                            'allDay' => false,
                            'rendered' => '
                                            <div class="row">
                                                <div class="class-name"><span class="class' . $i . '">&nbsp;</span>' . $entity->field_class_name['und'][0]['value'] . '</div>
                                                <div class="class-date">&nbsp;</div>
                                            </div>
                                            ');
                    }
                }
            }

            /*
            // add free study events based on preference
            if($w < $startWeek + 2419200)
            {
                $freeHours = floor(($totalLength * 2.6 - $studyLength) / 3600);
                for($j = 0; $j < $freeHours; $j++)
                {
                    $fTitle = '(F) Free study <small>' . date('n/d', strtotime(date("Y-m-d", strtotime('this week', $w)))) . '-' . ($j + 1) . '</small>';
                    if(in_array($fTitle, $saved))
                        continue;
                    $freeT = $w + 120 / $freeHours * $j * 3600 + 3600 * 5;
                    $events[] = array(
                        'title' => $fTitle,
                        'start' => date('Y/m/d H:i:s', $freeT) . '  UTC',
                        'end' => date('Y/m/d H:i:s', $freeT + 3600) . '  UTC',
                        'className' => 'free-event',
                        'allDay' => false,
                        'rendered' => '
                                        <div class="row">
                                            <div class="class-name"><span class="free-event">&nbsp;</span>' . $fTitle . '</div>
                                            <div class="class-date">&nbsp;</div>
                                        </div>
                                        ');
                }
            }
            */

        }

        if(isset($node->field_holidays['und'][0]['value']))
            foreach($node->field_holidays['und'] as $i => $h)
            {
                $hT = strtotime($h['value']);
                $events[] = array(
                    'title' => 'Holiday',
                    'start' => date('Y/m/d H:i:s', $hT) . ' UTC',
                    'end' => date('Y/m/d H:i:s', $hT + 86400) . ' UTC',
                    'className' => 'holiday-event',
                    'allDay' => true,
                    'rendered' => '
                                        <div class="row">
                                            <div class="class-name"><span class="holiday-event">&nbsp;</span>Holiday</div>
                                            <div class="class-date">&nbsp;</div>
                                        </div>
                                        ');
            }

        $query = new EntityFieldQuery();
        $dates = $query->entityCondition('entity_type', 'node')
            ->propertyCondition('type', 'key_dates')
            ->propertyCondition('title', $user->mail)
            ->propertyCondition('status', 1)
            ->range(0, 1)
            ->execute();

        if (!empty($dates['node'])) {
            $nodes = array_keys($dates['node']);
            $nid = array_shift($nodes);
            $node = node_load($nid);

            if (isset($node->field_reminders[LANGUAGE_NONE][0]['value']))
            {
                $reminders = array();
                foreach($node->field_reminders[LANGUAGE_NONE] as $eid => $reminder)
                {
                    $eid = $reminder['value'];
                    $entity = entity_load('field_collection_item', array($eid));
                    if (!empty($entity))
                        $reminders[$eid] = $entity[$eid];
                }
                uasort($reminders, function ($a, $b) { return strtotime($a->field_due_date['und'][0]['value']) - strtotime($b->field_due_date['und'][0]['value']); });
                foreach ($reminders as $eid => $reminder)
                {
                    $classI = array_search($reminder->field_class_name['und'][0]['value'], array_values($classes));
                    $dT = strtotime($reminder->field_due_date['und'][0]['value']);
                    $events[] = array(
                        'percent' => $reminder->field_percent['und'][0]['value'],
                        'title' => $reminder->field_class_name['und'][0]['value'] . ': ' . $reminder->field_assignment['und'][0]['value'],
                        'start' => date('Y/m/d H:i:s', $dT) . ' UTC',
                        'end' => date('Y/m/d H:i:s', $dT + 86400) . ' UTC',
                        'className' => 'class' . $classI . ' deadline-event',
                        'allDay' => true,
                        'rendered' => '
                                        <div class="row">
                                            <div class="class-name"><span class="class' . $classI . ' deadline-event">&nbsp;</span>Holiday</div>
                                            <div class="class-date">&nbsp;</div>
                                        </div>
                                        ');

                }
            }
        }

        $safety = 10;
        while(--$safety > 0)
        {

            $keys = array_map(function ($x) {
                return strtotime($x['start']);
            }, $events);
            array_multisort($keys, SORT_ASC, SORT_NUMERIC, $events);

            // move overlapping study times
            $i = 0;
            $anyMoved = false;
            while ($i < count($events)) {
                if(in_array($events[$i]['title'], $saved) ||
                    substr($events[$i]['title'], 0, 3) == '(F)')
                {
                    $i++;
                    continue;
                }

                if ($i > 0 && strtotime($events[$i]['start']) <= strtotime($events[$i - 1]['end']) &&
                    (substr($events[$i]['title'], 0, 3) == '(P)' || substr($events[$i]['title'], 0, 4) == '(SR)' ||
                        substr($events[$i]['title'], 0, 3) == '(F)')
                ) {
                    $diff = strtotime($events[$i - 1]['end']) - strtotime($events[$i]['start']) + 1200;
                    $events[$i]['start'] = date('c', strtotime($events[$i]['start']) + $diff);
                    $events[$i]['end'] = date('c', strtotime($events[$i]['end']) + $diff);
                    $anyMoved = true;
                } elseif ($i < count($events) - 1 && strtotime($events[$i]['end']) >= strtotime($events[$i + 1]['start']) &&
                    (substr($events[$i]['title'], 0, 3) == '(P)' || substr($events[$i]['title'], 0, 4) == '(SR)' ||
                        substr($events[$i]['title'], 0, 3) == '(F)')
                ) {
                    $diff = strtotime($events[$i + 1]['end']) - strtotime($events[$i]['start']) + 1200;
                    $events[$i]['start'] = date('c', strtotime($events[$i]['start']) + $diff);
                    $events[$i]['end'] = date('c', strtotime($events[$i]['end']) + $diff);
                    $i++;
                    $anyMoved = true;
                } else
                    $i++;
            }

            if(!$anyMoved)
                break;
        }

        return array($events, $node, $classes, $entities);
    }



    return array(array(), null, array(), array());
}


