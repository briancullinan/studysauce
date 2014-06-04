<?php

function studysauce_save_strategies()
{
    global $user;

    $query = new EntityFieldQuery();
    $nodes = $query->entityCondition('entity_type', 'node')
        ->propertyCondition('type', 'strategies')
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
        $node->type = 'strategies';
        $node->language = LANGUAGE_NONE; // Or e.g. 'en' if locale is enabled
        $node->uid = $user->uid;
        $node->status = 1; //(1 or 0): published or not
        $node->revision = 1;
        node_object_prepare($node); // Sets some defaults. Invokes hook_prepare() and hook_node_prepare().
        $node = node_submit($node); // Prepare node for saving
        node_save($node);
    }

    if(isset($_POST['strategies']) && is_array($_POST['strategies']))
    {
        // first, we need a list of all the events, this is stored in the schedule node
        list($events, $schedule, $classes, $entities) = studysauce_get_events();
        $names = array_map(function ($x) {
            return $x['title'];
        }, $events);

        foreach($_POST['strategies'] as $i => $strategy)
        {
            // check if the event name actually exists ins schedule
            if(($eid = array_search($strategy['name'], $names)) != false)
            {
                // check if the strategy is already saved
                $field = 'field_' . $strategy['type'] . '_strategies';
                $strategies = &$node->$field;
                if(isset($strategies['und'][0]['value']))
                {
                    unset($existing);
                    foreach($strategies['und'] as $i => $s)
                    {
                        $eid = $s['value'];
                        $entity = entity_load('field_collection_item', array($eid));
                        $entity = $entity[$eid];
                        if(isset($entity->field_class_name['und'][0]['value']) &&
                            $entity->field_class_name['und'][0]['value'] == $strategy['name'])
                        {
                            $existing = $entity;
                            break;
                        }
                    }
                }
                if(isset($strategy['remove']))
                {
                    if(isset($existing))
                    {
                        entity_delete('field_collection_item', array($existing->item_id));
                        unset($strategies[$i]);
                        node_save($node);
                    }
                }
                else
                {
                    if(!isset($existing))
                    {
                        $existing = entity_create('field_collection_item', array('field_name' => $field));
                        $existing->setHostEntity('node', $node);
                        $existing->field_class_name['und'][0]['value'] = $strategy['name'];
                    }
                    else
                    {
                        $existing->revision = 1;
                    }

                    if(isset($_POST['default']) && $_POST['default'] == $strategy['type'])
                        $existing->field_default['und'][0]['value'] = 1;
                    else
                        $existing->field_default['und'][0]['value'] = 0;

                    if(isset($strategy['skim']))
                        $existing->field_skim['und'][0]['value'] = $strategy['skim'];
                    if(isset($strategy['why']))
                        $existing->field_why['und'][0]['value'] = $strategy['why'];
                    if(isset($strategy['questions']))
                        $existing->field_questions['und'][0]['value'] = $strategy['questions'];
                    if(isset($strategy['summarize']))
                        $existing->field_summarize['und'][0]['value'] = $strategy['summarize'];
                    if(isset($strategy['exam']))
                        $existing->field_exam['und'][0]['value'] = $strategy['exam'];

                    if(isset($strategy['title']))
                        $existing->field_title['und'][0]['value'] = $strategy['title'];
                    if(isset($strategy['notes']))
                        $existing->field_notes['und'][0]['value'] = $strategy['notes'];
                    if(isset($strategy['uploads'][0]['fid']))
                    {
                        $existing->field_teaching['und'][0]['fid'] = $strategy['uploads'][0]['fid'];
                        $file = file_load($strategy['uploads'][0]['fid']);
                        $fileName = isset($strategy['uploads'][0]['rename']) ? $strategy['uploads'][0]['rename'] : $file->filename;
                        $location = 'public:///';
                        $filePath = $location . $fileName;
                        if ($file->uri !== $filePath) {
                            file_prepare_directory($location, FILE_CREATE_DIRECTORY);
                            file_move($file, $filePath);
                        }
                    }
                    if(isset($strategy['uploads'][0]['thumbnail']))
                        $existing->field_teaching['und'][0]['thumbnail'] = $strategy['uploads'][0]['thumbnail'];
                    if(isset($strategy['review']))
                        $existing->field_review['und'] = array_map(function ($x) {
                            return array('value'=> $x);
                        }, explode(',', $strategy['review']));


                    $existing->save();

                    node_save($node);
                }
            }
        }
    }
}

function studysauce_get_strategies($account = null)
{
    if(!isset($account))
    {
        global $user;
        $account = user_load($user->uid);
    }
    $result = array();

    $query = new EntityFieldQuery();
    $nodes = $query->entityCondition('entity_type', 'node')
        ->propertyCondition('type', 'strategies')
        ->propertyCondition('title', isset($account->mail) ? $account->mail : '')
        ->propertyCondition('status', 1)
        ->range(0, 1)
        ->execute();
    if (!empty($nodes['node']))
    {
        $nodes = array_keys($nodes['node']);
        $nid = array_shift($nodes);
        $node = node_load($nid);
        foreach(array('active', 'other', 'spaced', 'teach') as $i => $strategy)
        {
            $field = 'field_' . $strategy . '_strategies';
            $strategies = $node->$field;
            if(isset($strategies['und'][0]['value']))
            {
                foreach($strategies['und'] as $i => $s)
                {
                    $eid = $s['value'];
                    $entity = entity_load('field_collection_item', array($eid));
                    if(!isset($entity[$eid]))
                        continue;
                    $entity = $entity[$eid];
                    if(!isset($entity->field_class_name['und'][0]['value']))
                        continue;

                    $name = $entity->field_class_name['und'][0]['value'];
                    if($strategy == 'active')
                    {
                        $result[$name][$strategy]['skim'] = isset($entity->field_skim['und'][0]['value'])
                            ? $entity->field_skim['und'][0]['value']
                            : '';
                        $result[$name][$strategy]['why'] = isset($entity->field_why['und'][0]['value'])
                            ? $entity->field_why['und'][0]['value']
                            : '';
                        $result[$name][$strategy]['questions'] = isset($entity->field_questions['und'][0]['value'])
                            ? $entity->field_questions['und'][0]['value']
                            : '';
                        $result[$name][$strategy]['summarize'] = isset($entity->field_summarize['und'][0]['value'])
                            ? $entity->field_summarize['und'][0]['value']
                            : '';
                        $result[$name][$strategy]['exam'] = isset($entity->field_exam['und'][0]['value'])
                            ? $entity->field_exam['und'][0]['value']
                            : '';
                        $result[$name][$strategy]['default'] = isset($entity->field_default['und'][0]['value'])
                            ? $entity->field_default['und'][0]['value']
                            : false;
                    }
                    if($strategy == 'other')
                    {
                        $result[$name][$strategy]['notes'] = isset($entity->field_notes['und'][0]['value'])
                            ? $entity->field_notes['und'][0]['value']
                            : '';
                    }
                    elseif($strategy == 'teach')
                    {
                        $result[$name][$strategy]['title'] = isset($entity->field_title['und'][0]['value'])
                            ? $entity->field_title['und'][0]['value']
                            : '';
                        $result[$name][$strategy]['notes'] = isset($entity->field_notes['und'][0]['value'])
                            ? $entity->field_notes['und'][0]['value']
                            : '';
                        if(isset($entity->field_teaching['und'][0]['fid']))
                        {
                            $result[$name][$strategy]['uploads'][0]['fid'] = $entity->field_teaching['und'][0]['fid'];
                            $result[$name][$strategy]['uploads'][0]['thumbnail'] = $entity->field_teaching['und'][0]['thumbnail'];
                            $result[$name][$strategy]['uploads'][0]['uri'] = image_style_url('achievement', $entity->field_teaching['und'][0]['thumbnailfile']->uri);

                            // Load the derived files
                            $outputs = db_select('video_output', 'vo')
                                ->fields('vo')
                                ->condition('vo.original_fid', array($entity->field_teaching['und'][0]['fid']), 'IN')
                                ->execute()->fetchAllAssoc('output_fid');
                            foreach ($outputs as $outputfid => $output) {
                                if ($output->original_fid == $entity->field_teaching['und'][0]['fid']) {
                                    $file = file_load($output->output_fid);
                                    if($file->filesize > 0)
                                        $result[$name][$strategy]['uploads'][0]['play'] = check_plain(file_create_url($file->uri));
                                }
                            }
                        }
                    }
                    elseif($strategy == 'spaced')
                    {
                        $result[$name][$strategy]['notes'] = isset($entity->field_notes['und'][0]['value'])
                            ? $entity->field_notes['und'][0]['value']
                            : '';
                        $result[$name][$strategy]['review'] = isset($entity->field_review['und'][0]['value'])
                            ? implode(',', array_map(function ($x) {
                                return $x['value'];
                            }, $entity->field_review['und']))
                            : '';
                    }
                }
            }
        }
    }
    return $result;
}

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

    // save profile
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

    node_save($node);

    // save class
    if(!isset($_POST['classes']) || !is_array($_POST['classes']))
        $_POST['classes'] = array();

    // move single values in to an array so we can reuse the code from the plan page and the schedule page
    if(isset($_POST['className']) && isset($_POST['type']) && isset($_POST['dotw']) &&
        isset($_POST['start']) && isset($_POST['end']))
        $_POST['classes'][] = array(
            'className' => $_POST['className'],
            'type' => $_POST['type'],
            'dotw' => $_POST['dotw'],
            'start' => $_POST['start'],
            'end' => $_POST['end']
        );
    if(count($_POST['classes']) > 0)
    {
        list($events, $node, $classes, $entities) = studysauce_get_events();

        foreach($_POST['classes'] as $j => $class)
        {
            // check if class entity already exists by name
            $saved = array();
            // TODO: make this rely on entity id instead?
            foreach($entities as $i => $entity)
            {
                if(isset($entity->field_class_name[LANGUAGE_NONE][0]['value']))
                {
                    $saved[$entity->item_id] = $entity->field_class_name[LANGUAGE_NONE][0]['value'];
                    if($entity->field_class_name[LANGUAGE_NONE][0]['value'] == $class['className'] ||
                        isset($class['cid']) && $entity->item_id == $class['cid'])
                        $thisEid = $entity->item_id;
                }
            }
            if(!isset($thisEid))
            {
                $entity = entity_create('field_collection_item', array('field_name' => 'field_classes'));
                $entity->setHostEntity('node', $node);
            }
            else
            {
                $entity = entity_load('field_collection_item', array($thisEid));
                $entity = $entity[$thisEid];
                $entity->revision = 1;
            }
            $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $class['className'];
            $entity->field_day_of_the_week[LANGUAGE_NONE] = array_map(function ($x) { return array('value' => $x); }, explode(',', $class['dotw']));
            $entity->field_event_type[LANGUAGE_NONE][0]['value'] = $class['type'];
            $entity->field_time[LANGUAGE_NONE][0] = array(
                'value' => gmdate('Y-m-d H:i:s', strtotime($class['start'])),
                'value2' => gmdate('Y-m-d H:i:s', strtotime($class['end']))
            );
            $entity->save();

            // modify later events of the same type
            $anyAdded = false;
            if(isset($class['className']) && isset($class['type']) &&
                ($class['type'] == 'p' || $class['type'] == 'sr' || $class['type'] == 'f'))
            {
                $className = preg_replace('/^<h4>.*?<\/h4>|\(P\)\s*|^\(SR\)\s*|^\(F\)\s*|\s*\([0-9\/,\s-]+\)$|\s*<small>[0-9\/,\s-]+<\/small>$/i', '', $entity->field_class_name['und'][0]['value']);
                $i = array_search($className, array_values($classes));
                $diff = 0;
                foreach($events as $e => $evt)
                {
                    if($evt['title'] == $class['className'])
                    {
                        $diff = strtotime($class['start']) - strtotime($evt['start']);
                        continue;
                    }

                    // if the event is after the one that was moved
                    if(strtotime($evt['start']) < strtotime($class['start']))
                        continue;

                    // find diff
                    $test1 = date('1/1/1970 H:i:s', strtotime($evt['start']));
                    $test2 = date('1/1/1970 H:i:s', strtotime($class['start']) - $diff);
                    $test3 = strcmp($test1, $test2);

                    if( // make sure the event has the same start time
                        strcmp(date('1/1/1970 H:i:s', strtotime($evt['start'])), date('1/1/1970 H:i:s', strtotime($class['start']) - $diff)) == 0 &&
                        // if the type of event is the same
                        strpos($evt['className'], 'class' . $i) !== false)
                    {
                        // TODO: check if we are crossing over any event of the same class
                        if($class['type'] == 'p' && strpos($evt['className'], 'p-event') !== false ||
                            $class['type'] == 'sr' && strpos($evt['className'], 'sr-event') !== false ||
                            $class['type'] == 'f' && strpos($evt['className'], 'free-event') !== false)
                        {
                            if(($eid = array_search($evt['title'], $saved)) !== false)
                            {
                                $entity = entity_load('field_collection_item', array($eid));
                                $entity = $entity[$eid];
                                $entity->revision = 1;
                            }
                            else
                            {
                                $entity = entity_create('field_collection_item', array('field_name' => 'field_classes'));
                                $entity->setHostEntity('node', $node);
                                $anyAdded = true;
                                $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $evt['title'];
                            }
                            $entity->field_event_type[LANGUAGE_NONE][0]['value'] = $class['type'];
                            $entity->field_time[LANGUAGE_NONE][0] = array(
                                'value' => gmdate('Y-m-d H:i:s', strtotime($evt['start']) + $diff),
                                'value2' => gmdate('Y-m-d H:i:s', strtotime($evt['end']) + $diff)
                            );
                            $entity->save();
                        }
                    }
                }
            }

            if(!isset($thisEid) || $anyAdded)
                node_save($node);
        }
    }
    else
    {

        // save class profile
        $eids = array();
        foreach ($node->field_classes[LANGUAGE_NONE] as $i => $class)
            $eids[] = $class['value'];
        foreach($_POST as $eid => $settings)
        {
            if(is_array($settings) && isset($settings['type']) && isset($settings['difficulty']) &&
                in_array($eid, $eids))
            {
                $entity = entity_load('field_collection_item', array($eid));
                if (!empty($entity))
                {
                    $entity[$eid]->field_study_type['und'][0]['value'] = $settings['type'];
                    $entity[$eid]->field_study_difficulty['und'][0]['value'] = $settings['difficulty'];
                    $entity[$eid]->save(true);

                    node_save($node);
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
                node_save($node);
            }
        }
    }


    // get event schedule
    list($events, $node, $classes, $entities) = studysauce_get_events();

    $newSDS = studysauce_get_sds_messages();
    $lastSDS = end($newSDS);
    list($awards) = studysauce_get_awards();
    list($times, $rows, $total, $hours) = _studysauce_get_metrics();

    print json_encode(array(
        'events' => $events,
        'classes' => $classes,
        'schedule' => '<div>' . theme('studysauce-schedule') . '</div>',
        'plan' => '<div>' . theme('studysauce-plans') . '</div>',
        'profile' => '<div>' . theme('studysauce-profile') . '</div>',
        'lastSDS' => $lastSDS,
        'awards' => $awards,
        'rows' => $rows,
        'times' => $times,
        'total' => $total,
        'hours' => $hours,
    ));
    drupal_exit();
}

function studysauce_get_events($account = null, $lastOrderTime = null)
{
    if(!isset($account))
    {
        global $user;
        $account = user_load($user->uid);
    }
    if(!isset($lastOrderTime) || empty($lastOrderTime))
    {
        $lastOrder = _studysauce_orders_by_uid($account->uid);
        if(isset($lastOrder) && isset($lastOrder->created))
            $lastOrderTime = $lastOrder->created;
    }

    // get the schedule with university filled in
    $query = new EntityFieldQuery();
    $nodes = $query->entityCondition('entity_type', 'node')
        ->propertyCondition('type', 'schedule')
        ->propertyCondition('title', isset($account->mail) ? $account->mail : '')
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
                        if (isset($entity[$eid]->field_time['und'][0]['value']) &&
                            isset($entity[$eid]->field_time['und'][0]['value2'])
                        ) {
                            $entities[$eid] = $entity[$eid];
                            if ($startTerm == 0 || strtotime($entity[$eid]->field_time['und'][0]['value'] . ' UTC') < $startTerm)
                                $startTerm = strtotime($entity[$eid]->field_time['und'][0]['value'] . ' UTC');
                            if ($endTerm == 0 || strtotime($entity[$eid]->field_time['und'][0]['value2'] . ' UTC') > $endTerm)
                                $endTerm = strtotime($entity[$eid]->field_time['und'][0]['value2'] . ' UTC');
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


        $startTerm = strtotime('this week', $startTerm);
        $endTerm = strtotime('this week', $endTerm) + 604800;
        $startWeek = strtotime('this week', time()) - 86400;
        for ($w = $startTerm; $w < $endTerm; $w += 604800)
        {
            $totalLength = 0;
            $studyLength = 0;
            foreach ($entities as $eid => $entity)
            {
                if(!isset($entity->field_event_type['und'][0]['value']) || $entity->field_event_type['und'][0]['value'] == 'c')
                {
                    $i = array_search($eid, array_keys($classes));
                    $classStart = strtotime($entity->field_time['und'][0]['value'] . ' UTC');
                    $classEnd = strtotime($entity->field_time['und'][0]['value2'] . ' UTC');
                    $startDay = strtotime(date("Y-m-d", $classStart));
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

                        $t = strtotime('this week', $w) + $d;
                        if($t < $classStart || $t > $classEnd)
                            continue;

                        // calculate hours spent in class and remaining free time
                        $length = strtotime(date('1/1/1970 H:i:s', $classEnd)) - strtotime(date('1/1/1970 H:i:s', $classStart));
                        if($length < 0)
                            $length += 86400;
                        $totalLength += max(min(14400, $length), 3600);

                        $classT = new DateTime(date('Y/m/d', $t) . ' ' . date('H:i:s', $classStart));
                        $classT->setTimezone(new DateTimeZone('UTC'));
                        $events[] = array(
                            'editable' => false,
                            'title' => '<h4>CLASS</h4>' . $entity->field_class_name['und'][0]['value'],
                            'start' => $classT->format('Y/m/d H:i:s') . ' UTC',
                            'end' => date_add($classT, new DateInterval('PT' . $length . 'S'))->format('Y/m/d H:i:s') . ' UTC',
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

                        // add spaced repetition 7 days, 14 days, 28 days
                        $dates = array(date('n/d', $t));
                        if($t - 86400 * 7 >= $startDay)
                            $dates[] = date('n/d', $t - 86400 * 7);
                        if($t - 86400 * 14 >= $startDay)
                            $dates[] = date('n/d', $t - 86400 * 14);
                        if($t - 86400 * 28 >= $startDay)
                            $dates[] = date('n/d', $t - 86400 * 28);

                        $srTitle = '<h4>STUDY</h4>' . $entity->field_class_name['und'][0]['value'] . ' <small>' . implode(', ', $dates) . '</small>';
                        if(!in_array($srTitle, $saved))
                        {
                            $spT = new DateTime(date('Y/m/d', $t) . ' ' . date('H:i:s', $classEnd));
                            $spT->setTimezone(new DateTimeZone('UTC'));
                            $spT->add(new DateInterval('PT2H'));
                            $sessionLength = 3600;
                            if(isset($entity->field_study_difficulty['und'][0]['value']))
                            {
                                if($entity->field_study_difficulty['und'][0]['value'] == 'easy')
                                    $sessionLength = 2700;
                                elseif($entity->field_study_difficulty['und'][0]['value'] == 'average')
                                    $sessionLength = 3060;
                            }
                            $studyLength += $sessionLength; // study hours automatically taken up by this week
                            $events[] = array(
                                'title' => $srTitle,
                                'start' => $spT->format('Y/m/d H:i:s') . '  UTC',
                                'end' => date_add($spT, new DateInterval('PT' . $sessionLength . 'S'))->format('Y/m/d H:i:s') . '  UTC',
                                'className' => 'class' . $i . ' sr-event',
                                'allDay' => false,
                                'dates' => $dates,
                                'rendered' => '
                                            <div class="row">
                                                <div class="class-name"><span class="class' . $i . '">&nbsp;</span>' . $srTitle . '</div>
                                                <div class="class-date">&nbsp;</div>
                                            </div>
                                            ');
                        }

                        // add prework
                        $pTitle = '<h4>PRE-WORK</h4>' . $entity->field_class_name['und'][0]['value'] . ' <small>' . date('n/d', $t) . '</small>';
                        if(!in_array($pTitle, $saved))
                        {
                            $preT = new DateTime(date('Y/m/d', $t) . ' ' . date('H:i:s', $classStart));
                            $preT->setTimezone(new DateTimeZone('UTC'));
                            $preT->sub(new DateInterval('P1D'));

                            // TODO: adjust length of session based on class difficulty, can we do more here?
                            $sessionLength = 3600;
                            if(isset($entity->field_study_difficulty['und'][0]['value']))
                            {
                                if($entity->field_study_difficulty['und'][0]['value'] == 'easy')
                                    $sessionLength = 2700;
                                elseif($entity->field_study_difficulty['und'][0]['value'] == 'average')
                                    $sessionLength = 3060;
                            }
                            $studyLength += $sessionLength; // study hours automatically taken up by this week
                            //if((date('l', $preT) == 'Sunday' || date('l', $preT) == 'Saturday') &&
                            //    isset($node->field_weekends['und'][0]['value']) && $node->field_weekends['und'][0]['value'] == 'hit_hard')
                            //    $preT += date('l', $preT) == 'Sunday' ? (86400 * 5) : (86400 * 6); // prepare for next week's class
                            $events[] = array(
                                'title' => $pTitle,
                                'start' => $preT->format('Y/m/d H:i:s') . '  UTC',
                                'end' => date_add($preT, new DateInterval('PT' . $sessionLength . 'S'))->format('Y/m/d H:i:s') . '  UTC',
                                'className' => 'class' . $i . ' p-event',
                                'allDay' => false,
                                'rendered' => '
                                            <div class="row">
                                                <div class="class-name"><span class="class' . $i . '">&nbsp;</span>' . $pTitle . '</div>
                                                <div class="class-date">&nbsp;</div>
                                            </div>
                                            ');
                        }

                    }
                }
                elseif($entity->field_event_type['und'][0]['value'] == 'o')
                {
                    $classStart = strtotime($entity->field_time['und'][0]['value'] . ' UTC');
                    $classEnd = strtotime($entity->field_time['und'][0]['value2'] . ' UTC');

                    // add repeating other events
                    if(isset($entity->field_day_of_the_week['und'][0]['value']))
                    {
                        foreach ($entity->field_day_of_the_week['und'] as $j => $dotw)
                        {
                            if($dotw['value'] == 'Monthly' && $classStart >= $w && $classStart <= $w + 604800)
                            {
                                // adjust the entered date for the current month
                                $length = strtotime(date('1/1/1970 H:i:s', $classEnd)) - strtotime(date('1/1/1970 H:i:s', $classStart));
                                if($length <= 0)
                                    $length += 86400;
                                $startStr = date('m', $w + 604800) . date('/d/', $classStart) . date('Y', $w + 604800) . ' ' . date('H:i:s', $classStart);
                                $classStart = strtotime($startStr);
                                $classEnd = $classStart + $length;
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
                            elseif($dotw['value'] == 'Yearly' && $classStart >= $w && $classStart <= $w + 604800)
                            {
                                // adjust the entered date for the current month
                                $length = strtotime(date('1/1/1970 H:i:s', $classEnd)) - strtotime(date('1/1/1970 H:i:s', $classStart));
                                if($length <= 0)
                                    $length += 86400;
                                // this line is all that is different
                                $startStr = date('m', $classStart) . date('/d/', $classStart) . date('Y', $w + 604800) . ' ' . date('H:i:s', $classStart);
                                $classStart = strtotime($startStr);
                                $classEnd = $classStart + $length;
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
                            else
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

                                $t = strtotime('this week', $w) + $d;
                                if($t < $classStart || $t > $classEnd)
                                    continue;

                                $length = strtotime(date('1/1/1970 H:i:s', $classEnd)) - strtotime(date('1/1/1970 H:i:s', $classStart));
                                if($length < 0)
                                    $length += 86400;

                                $classT = new DateTime(date('Y/m/d', $t) . ' ' . date('H:i:s', $classStart));
                                $classT->setTimezone(new DateTimeZone('UTC'));
                                $events[] = array(
                                    'editable' => false,
                                    'title' => $entity->field_class_name['und'][0]['value'],
                                    'start' => $classT->format('Y/m/d H:i:s') . ' UTC',
                                    'end' => date_add($classT, new DateInterval('PT' . $length . 'S'))->format('Y/m/d H:i:s') . ' UTC',
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
                    }
                    // only add single occurring event when we are rendering the right week
                    elseif($classStart >= $w && $classStart <= $w + 604800)
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
                        $length = strtotime(date('1/1/1970 H:i:s', $classEnd)) - strtotime(date('1/1/1970 H:i:s', $classStart));
                        if($length <= 0)
                            $length += 86400;
                        $studyLength += $length; // add to study length if the user has moved their study event
                        $i = array_search(
                            // extract class name from title by replacing everything else
                            preg_replace('/^<h4>.*?<\/h4>|\(P\)\s*|^\(SR\)\s*|^\(F\)\s*|\s*\([0-9\/,\s-]+\)$|\s*<small>[0-9\/,\s-]+<\/small>$/i', '', $entity->field_class_name['und'][0]['value']), array_values($classes));
                        unset($dates);
                        if($entity->field_event_type['und'][0]['value'] == 'sr' &&
                            preg_match('/\s*<small>([0-9\/,\s-]+)<\/small>$/i', $entity->field_class_name['und'][0]['value'], $matches))
                        {
                            $dates = explode(', ', $matches[1]);
                        }
                        $events[] = array(
                            'title' => $entity->field_class_name['und'][0]['value'],
                            'start' => date('Y/m/d H:i:s', $classStart) . ' UTC',
                            'end' => date('Y/m/d H:i:s', $classEnd) . ' UTC',
                            'className' => $entity->field_event_type['und'][0]['value'] == 'f' ? 'free-event' : 'class' . $i . ' ' . $entity->field_event_type['und'][0]['value'] . '-event',
                            'allDay' => false,
                            'dates' => isset($dates) ? $dates : null,
                            'rendered' => '
                                            <div class="row">
                                                <div class="class-name"><span class="class' . $i . '">&nbsp;</span>' . $entity->field_class_name['und'][0]['value'] . '</div>
                                                <div class="class-date">&nbsp;</div>
                                            </div>
                                            ');
                    }
                }
            }

            // add free study events based on preference
            if($w < $startWeek + 2419200)
            {
                // TODO: adjust study factor based on nothing but A's preference, only affect free time, can we do more?
                $studyFactor = 2.4;
                if(isset($node->field_grades['und'][0]['value']) && $node->field_grades['und'][0]['value'] == 'as_only')
                    $studyFactor = 2.8;
                // there are 16 usable hours, 4 * (5 - 1) = 16 * 7 days = 112 usable hours in a week
                $shouldStudy = max($totalLength * $studyFactor, 112*3600); // a student should study 3 [2.6] times outside of class
                $remainingStudy = min($shouldStudy - $studyLength, 3600); // subtract the hours already accounted for by schedules study sessions
                $freeHours = floor($remainingStudy / 3600);
                for($j = 0; $j < $freeHours; $j++)
                {
                    $fTitle = '<h4>STUDY</h4>Any class needed <small>' . date('n/d', strtotime('this week', $w)) . '-' . ($j + 1) . '</small>';
                    if(in_array($fTitle, $saved))
                        continue;

                    // use weekends setting to determine if the first free study should fall on a sunday
                    $freeT = $w + 112 / $freeHours * $j * 3600 + 3600 * (isset($node->field_weekends['und'][0]['value']) && $node->field_weekends['und'][0]['value'] == 'hit_hard' ? -1 : 0) * 24;
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
            ->propertyCondition('title', $account->mail)
            ->propertyCondition('status', 1)
            ->range(0, 1)
            ->execute();

        if (!empty($dates['node'])) {
            $key_dates = array_keys($dates['node']);
            $kid = array_shift($key_dates);
            $dates = node_load($kid);

            if (isset($dates->field_reminders[LANGUAGE_NONE][0]['value']))
            {
                $reminders = array();
                foreach($dates->field_reminders[LANGUAGE_NONE] as $eid => $reminder)
                {
                    $eid = $reminder['value'];
                    $entity = entity_load('field_collection_item', array($eid));
                    if (!empty($entity))
                        $reminders[$eid] = $entity[$eid];
                }
                uasort($reminders, function ($a, $b) {
                    if(isset($a->field_due_date['und'][0]['value']) && isset($b->field_due_date['und'][0]['value']))
                        return strtotime($a->field_due_date['und'][0]['value']) - strtotime($b->field_due_date['und'][0]['value']);
                });
                foreach ($reminders as $eid => $reminder)
                {
                    if(!isset($reminder->field_due_date['und'][0]['value']))
                        continue;
                    $classI = array_search($reminder->field_class_name['und'][0]['value'], array_values($classes));
                    $dT = strtotime($reminder->field_due_date['und'][0]['value']);
                    $events[] = array(
                        'percent' => $reminder->field_class_name['und'][0]['value'] != 'Nonacademic' ? $reminder->field_percent['und'][0]['value'] : '',
                        'title' => $reminder->field_class_name['und'][0]['value'] . ': ' . $reminder->field_assignment['und'][0]['value'],
                        'start' => date('Y/m/d H:i:s', $dT) . ' UTC',
                        'end' => date('Y/m/d H:i:s', $dT + 86400 - 1) . ' UTC',
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

        // there are 16 usable hours, 4 * (5 - 1) = 16
        // we subtract 1 hour of break time from the following 5-hour time groups
        // 6am-11am
        // 11am-4pm
        // 4pm-9pm
        // 9pm-2am

        // fill time slots for every day based on time preferences
        $keys = array_map(function ($x) {
            return strtotime($x['start']);
        }, $events);
        array_multisort($keys, SORT_ASC, SORT_NUMERIC, $events);


        // normalize each group value over 4, that leaves room for 15 minute breaks
        $times = array(
            '16' => isset($node->field_4_pm_9_pm['und'][0]['value']) ? intval($node->field_4_pm_9_pm['und'][0]['value']) / 5.0 * 4 : 2,
            '21' => isset($node->field_11_am_4_pm['und'][0]['value']) ? intval($node->field_9_pm_2_am['und'][0]['value']) / 5.0 * 4 : 2,
            '11' => isset($node->field_9_pm_2_am['und'][0]['value']) ? intval($node->field_11_am_4_pm['und'][0]['value']) / 5.0 * 4 : 2,
            '06' => isset($node->field_6_am_11_am['und'][0]['value']) ? intval($node->field_6_am_11_am['und'][0]['value']) / 5.0 * 4 : 2
        );
        $rtimes = array(
            '06' => isset($node->field_6_am_11_am['und'][0]['value']) ? intval($node->field_6_am_11_am['und'][0]['value']) / 5.0 * 4 : 2,
            '11' => isset($node->field_11_am_4_pm['und'][0]['value']) ? intval($node->field_11_am_4_pm['und'][0]['value']) / 5.0 * 4 : 2,
            '16' => isset($node->field_4_pm_9_pm['und'][0]['value']) ? intval($node->field_4_pm_9_pm['und'][0]['value']) / 5.0 * 4 : 2,
            '21' => isset($node->field_9_pm_2_am['und'][0]['value']) ? intval($node->field_9_pm_2_am['und'][0]['value']) / 5.0 * 4 : 2
        );
        $artimes = array();
        foreach($rtimes as $k => $t)
            $artimes[$k] = $t;
        arsort($artimes);
        $buckets = array();
        $i = 0;

        // MAGIC, DO NOT TOUCH!!!!!!
        while (++$i < count($events)) {
            if(in_array($events[$i]['title'], $saved))
            {
                continue;
            }
            $d = date_timezone_set(new DateTime($events[$i]['start']), new DateTimeZone('UTC'))->format('Y/m/d');
            //$d = (new DateTime($events[$i]['start']))->format('Y/m/d');

            // determine the order we will fill each bucket
            // SR events should start in the afternoon
            if(strpos($events[$i]['className'], 'sr-event') !== false)
            {
                $otimes = $times;
            }
            elseif(strpos($events[$i]['className'], 'p-event') !== false)
            {
                $otimes = $rtimes;
            }
            // if our buckets are overflowed, even the buckets out
            elseif(isset($buckets[$d]) && array_sum(array_values($times)) <= array_sum(array_values($buckets[$d])))
            {
                $otimes = $rtimes;
            }
            else
                $otimes = $artimes;

            foreach($otimes as $k => $count)
            {
                if(!isset($buckets[$d][$k]) || $buckets[$d][$k] < $count - 1 ||
                    // we are over the amount for the day
                    array_sum(array_values($times)) <= array_sum(array_values($buckets[$d])))
                {
                    $length = strtotime(date('1/1/1970 H:i:s', strtotime($events[$i]['end']))) - strtotime(date('1/1/1970 H:i:s', strtotime($events[$i]['start'])));
                    if($length <= 0)
                        $length += 86400;

                    $buckets[$d][$k] = isset($buckets[$d][$k]) ? ($buckets[$d][$k] + $length / 3600.0) : ($length / 3600.0);
                    if(strpos($events[$i]['className'], 'p-event') !== false || strpos($events[$i]['className'], 'sr-event') !== false ||
                        strpos($events[$i]['className'], 'free-event') !== false)
                    {
                        // calculate how far the event has to move to be in the bucket accounting for change in timezone
                        $diff = strtotime($d . ' ' . $k . ':00:00') - strtotime($events[$i]['start']);
                        // TODO: uncomment this to put in to buckets before spacing out
                        $events[$i]['start'] = gmdate('Y/m/d H:i:s', strtotime($events[$i]['start']) + $diff) . ' UTC';
                        $events[$i]['end'] = gmdate('Y/m/d H:i:s', strtotime($events[$i]['end']) + $diff) . ' UTC';
                    }
                    break;
                }
            }
        }

        // move overlapping study times
        // TODO: set this to > 0 to activate overlap-removal
        $safety = 20; // our safety is high because we are only moving at 1/4 * 60 increments
        while(--$safety >= 0)
        {

            $keys = array_map(function ($x) {
                return strtotime($x['start']);
            }, $events);
            array_multisort($keys, SORT_ASC, SORT_NUMERIC, $events);

            // move overlapping study times
            $i = 0;
            $anyMoved = false;
            while (++$i < count($events)) {
                if(in_array($events[$i]['title'], $saved))
                {
                    continue;
                }

                if($i == 8)
                {
                    usleep(1);
                }
                if ($i > 0 && strtotime($events[$i - 1]['end']) >= strtotime($events[$i]['start']) &&
                    (strpos($events[$i]['className'], 'p-event') !== false || strpos($events[$i]['className'], 'sr-event') !== false ||
                        strpos($events[$i]['className'], 'free-event') !== false)
                ) {
                    $diff = strtotime($events[$i - 1]['end']) - strtotime($events[$i]['start']) + 900; // tried to move to end times of other classes and it didn't work very well
                    //$diff = 60*15; // move 15 minutes back
                    $events[$i]['start'] = gmdate('Y/m/d H:i:s', strtotime($events[$i]['start']) + $diff) . ' UTC';
                    $events[$i]['end'] = gmdate('Y/m/d H:i:s', strtotime($events[$i]['end']) + $diff) . ' UTC';
                    $anyMoved = true;
                }
                elseif ($i + 1 < count($events) && strtotime($events[$i + 1]['start']) <= strtotime($events[$i]['end']) &&
                    (strpos($events[$i]['className'], 'p-event') !== false || strpos($events[$i]['className'], 'sr-event') !== false ||
                        strpos($events[$i]['className'], 'free-event') !== false)
                ) {
                    //$diff = strtotime($events[$i - 1]['end']) - strtotime($events[$i]['start']) + 1200; // tried to move to end times of other classes and it didn't work very well
                    $diff = 60*15; // move 15 minutes back
                    $events[$i]['start'] = gmdate('Y/m/d H:i:s', strtotime($events[$i]['start']) + $diff) . ' UTC';
                    $events[$i]['end'] = gmdate('Y/m/d H:i:s', strtotime($events[$i]['end']) + $diff) . ' UTC';
                    $anyMoved = true;
                }
            }

            if(!$anyMoved)
                break;
        }
        // fill time slots for every day based on time preferences
        $keys = array_map(function ($x) {
            return strtotime($x['start']);
        }, $events);
        array_multisort($keys, SORT_ASC, SORT_NUMERIC, $events);


        return array($events, $node, $classes, $entities);
    }



    return array(array(), null, array(), array());
}


