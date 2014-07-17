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
                    foreach($strategies['und'] as $j => $s)
                    {
                        $eid = $s['value'];
                        $entity = entity_load('field_collection_item', array($eid));
                        if(isset($entity[$eid]->field_class_name['und'][0]['value']) &&
                            $entity[$eid]->field_class_name['und'][0]['value'] == $strategy['name'])
                        {
                            $existing = $entity[$eid];
                            break;
                        }
                    }
                }

                // remove the strategy
                if(isset($strategy['remove']))
                {
                    if(isset($existing))
                    {
                        entity_delete('field_collection_item', array($existing->item_id));
                        unset($strategies[$j]);
                        node_save($node);
                    }
                }
                // create the strategy
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
                        //$existing->revision = 1;
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
                    if(isset($strategy['uploads'][0]['thumbnail']))
                        $existing->field_teaching['und'][0]['thumbnail'] = $strategy['uploads'][0]['thumbnail'];
                    if(isset($strategy['uploads'][0]['fid']))
                        $existing->field_teaching['und'][0]['fid'] = $strategy['uploads'][0]['fid'];
                    if(isset($strategy['review']))
                    {
                        if(!empty($strategy['review']))
                            $existing->field_review['und'] = array_map(function ($x) { return array('value'=> $x); },
                                explode(',', $strategy['review']));
                        else
                            $existing->field_review['und'] = array();
                    }
                    if(isset($strategy['prepared']))
                    {
                        if(!empty($strategy['prepared']))
                            $existing->field_prepared['und'] = array_map(function ($x) { return array('value'=> $x); },
                                explode(',', $strategy['prepared']));
                        else
                            $existing->field_prepared['und'] = array();
                    }


                    $existing->save(true);

                    if(isset($strategy['uploads'][0]['fid']))
                    {
                        $file = file_load($strategy['uploads'][0]['fid']);
                        $fileName = isset($strategy['uploads'][0]['rename']) ? $strategy['uploads'][0]['rename'] : $file->filename;
                        $location = 'public:///';
                        $filePath = $location . $fileName;
                        if ($file->uri !== $filePath) {
                            file_prepare_directory($location, FILE_CREATE_DIRECTORY);
                            file_move($file, $filePath);
                        }
                        $video = video_jobs::load($file->fid);
                        if (!$video) {
                            list($entity_id, $entity_vid, $bundle) = entity_extract_ids('field_collection_item', $existing);
                            // @todo get the default dimension when not available in $item
                            if (!video_jobs::create($file->fid, '320x180', $entity_id, 'field_collection_item', 'field_teaching', 'und', 0)) {
                                drupal_set_message(t('Something went wrong with your video job creation. Please check your recent log entries for further debugging.'), 'error');
                                return;
                            }
                            // Load a fresh copy of the job
                            $video = video_jobs::load($file->fid);
                        }
                        $video->video_status = VIDEO_RENDERING_PENDING;
                        $video->statusupdated = time();
                        $video->dimensions = '320x180';
                        video_jobs::update($video);
                    }

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
        foreach(array('active', 'other', 'spaced', 'teach', 'prework') as $i => $strategy)
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
                    $result[$name][$strategy]['default'] = isset($entity->field_default['und'][0]['value'])
                        ? $entity->field_default['und'][0]['value']
                        : false;
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
                    }
                    elseif($strategy == 'other')
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
                            $file = file_load($entity->field_teaching['und'][0]['fid']);
                            $outputs = db_select('video_output', 'vo')
                                ->fields('vo')
                                ->condition('vo.original_fid', array($entity->field_teaching['und'][0]['fid']), 'IN')
                                ->execute()->fetchAllAssoc('output_fid');
                            foreach ($outputs as $outputfid => $output) {
                                $trans = file_load($output->output_fid);
                                if($trans->filesize > 0)
                                    $result[$name][$strategy]['uploads'][0]['play'] = check_plain(file_create_url($trans->uri));
                            }
                            if(empty($outputs))
                            {
                                $result[$name][$strategy]['uploads'][0]['play'] = check_plain(file_create_url($file->uri));
                                $video = video_jobs::load($entity->field_teaching['und'][0]['fid']);
                                if (!$video) {
                                    list($entity_id, $entity_vid, $bundle) = entity_extract_ids('field_collection_item', $entity);
                                    // @todo get the default dimension when not available in $item
                                    if (!video_jobs::create($file->fid, '320x180', $entity_id, 'field_collection_item', 'field_teaching', 'und', 0)) {
                                        drupal_set_message(t('Something went wrong with your video job creation. Please check your recent log entries for further debugging.'), 'error');
                                        return;
                                    }
                                    // Load a fresh copy of the job
                                    $video = video_jobs::load($file->fid);
                                }
                                $video->video_status = VIDEO_RENDERING_PENDING;
                                $video->statusupdated = time();
                                $video->dimensions = '320x180';
                                video_jobs::update($video);
                            }
                        }
                    }
                    elseif($strategy == 'spaced')
                    {
                        $result[$name][$strategy]['notes'] = isset($entity->field_notes['und'][0]['value'])
                            ? $entity->field_notes['und'][0]['value']
                            : '';
                        $result[$name][$strategy]['review'] = isset($entity->field_review['und'][0]['value'])
                            ? implode(',', array_map(function ($x) { return $x['value']; }, $entity->field_review['und']))
                            : '';
                    }
                    elseif($strategy == 'prework')
                    {
                        $result[$name][$strategy]['notes'] = isset($entity->field_notes['und'][0]['value'])
                            ? $entity->field_notes['und'][0]['value']
                            : '';
                        $result[$name][$strategy]['prepared'] = isset($entity->field_prepared['und'][0]['value'])
                            ? implode(',', array_map(function ($x) { return $x['value']; }, $entity->field_prepared['und']))
                            : '';
                    }
                }
            }
        }
    }
    return $result;
}

function studysauce_save_completed()
{
    if(!isset($account))
    {
        global $user;
        $account = user_load($user->uid);
    }

    if($_POST['type'] == 'd')
    {
        // find deadline and mark completed

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
                foreach($dates->field_reminders[LANGUAGE_NONE] as $eid => $reminder)
                {
                    $eid = $reminder['value'];
                    $entity = entity_load('field_collection_item', array($eid));
                    if (!empty($entity) &&
                        $entity[$eid]->field_class_name['und'][0]['value'] . ': ' . $entity[$eid]->field_assignment['und'][0]['value'] == $_POST['className'])
                    {
                        $entity[$eid]->field_completed[LANGUAGE_NONE][0]['value'] = true && $_POST['completed'];
                        $entity[$eid]->save();
                    }

                }
            }
        }
    }
    else
    {
        // find matching event
        $events = studysauce_get_events();
        $events[$_POST['cid']]->field_completed[LANGUAGE_NONE][0]['value'] = true && $_POST['completed'];
        $events[$_POST['cid']]->save(true);
    }
}

function studysauce_remove_old_events(&$node, &$events, $cid, $removeCid = false)
{
    $field_collection_item_values = array();
    $keys = array();
    foreach($node->field_classes['und'] as $i => $x)
    {
        // skip new events created this round
        if(!isset($x['value']))
            continue;

        $keys[] = $x['value'];
    }
    $newEvents = array();

    // remove all past classes
    foreach($events as $eid => $event)
    {
        // remove empty and invalid events
        if(!isset($event->field_class_name['und'][0]['value']) || empty($event->field_class_name['und'][0]['value']) ||
            // remove events that match the old class name
            ($event->field_class_name['und'][0]['value'] == $events[$cid]->field_class_name['und'][0]['value'] &&
                // don't remove the current event unless instructed so
                ($eid != $cid || $removeCid)) ||
            // always remove free study events because they will be recalculated
            (isset($event->field_event_type['und'][0]['value']) && $event->field_event_type['und'][0]['value'] == 'f'))
        {
            $field_collection_item_values[] = $eid;
            $key = array_search($eid, $keys);
            // Unset them.
            unset($node->field_classes[LANGUAGE_NONE][$key]);
        }
        else
            $newEvents[$eid] = $event;
    }
    entity_delete_multiple('field_collection_item', $field_collection_item_values);

    // check if strategies have been saved
    $events = $newEvents;
}

function studysauce_add_reoccurring_events($node, &$events, $cid)
{
    $classStart = new DateTime($events[$cid]->field_time['und'][0]['value'], new DateTimeZone('UTC'));
    $classEnd = new DateTime($events[$cid]->field_time['und'][0]['value2'], new DateTimeZone('UTC'));

    $length = strtotime($classEnd->format('1/1/1970 H:i:s')) - strtotime($classStart->format('1/1/1970 H:i:s'));
    if($length <= 0)
        $length += 86400;

    // add repeating other events
    $startTerm = strtotime('this week', $classStart->getTimestamp());
    $endTerm = strtotime('this week', $classEnd->getTimestamp()) + 604800;
    for ($w = $startTerm; $w < $endTerm; $w += 604800)
    {
        foreach ($events[$cid]->field_day_of_the_week['und'] as $j => $dotw)
        {
            $d = 0;
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
                    continue;
            }

            $t = strtotime('this week', $w) + $d;
            if($t < $classStart->getTimestamp() || $t > $classEnd->getTimestamp())
                continue;

            $classT = new DateTime();
            $classT->setTimestamp($t);
            $classT->setTimezone(new DateTimeZone('UTC'));
            $classT->setTime($classStart->format('H'), $classStart->format('i'), $classStart->format('s'));

            $entity = entity_create('field_collection_item', array('field_name' => 'field_classes'));
            $entity->setHostEntity('node', $node);
            $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $events[$cid]->field_class_name['und'][0]['value'];
            $entity->field_day_of_the_week[LANGUAGE_NONE] = array();
            $entity->field_event_type[LANGUAGE_NONE][0]['value'] = $events[$cid]->field_event_type[LANGUAGE_NONE][0]['value'];
            $entity->field_time[LANGUAGE_NONE][0] = array(
                'value' => $classT->format('Y/m/d H:i:s'),
                'value2' => date_add($classT, new DateInterval('PT' . $length . 'S'))->format('Y/m/d H:i:s')
            );
            $events[-count($events)] = $entity;
        }
    }
    // do nothing more for singly occurring events

}

function studysauce_get_preferred_time($node, $events, $cid, $notBefore = false)
{

    // there are 16 usable hours, 4 * (5 - 1) = 16
    // we subtract 1 hour of break time from the following 5-hour time groups
    // 6am-11am
    // 11am-4pm
    // 4pm-9pm
    // 9pm-2am

    // fill time slots for every day based on time preferences
    uasort($events, function ($a, $b) {
        return strtotime($a->field_time['und'][0]['value']) - strtotime($b->field_time['und'][0]['value']);
    });


    // normalize each group value over 4, that leaves room for 15 minute breaks
    $timeslots = array(
        '16' => isset($node->field_4_pm_9_pm['und'][0]['value']) ? intval($node->field_4_pm_9_pm['und'][0]['value']) / 5.0 * 4 : 2,
        '21' => isset($node->field_11_am_4_pm['und'][0]['value']) ? intval($node->field_9_pm_2_am['und'][0]['value']) / 5.0 * 4 : 2,
        '11' => isset($node->field_9_pm_2_am['und'][0]['value']) ? intval($node->field_11_am_4_pm['und'][0]['value']) / 5.0 * 4 : 2,
        '06' => isset($node->field_6_am_11_am['und'][0]['value']) ? intval($node->field_6_am_11_am['und'][0]['value']) / 5.0 * 4 : 2
    );
    arsort($timeslots);

    $buckets = array();

    // MAGIC, DO NOT TOUCH!!!!!!
    foreach($events as $e => $event)
    {
        // we are only dealing with $events a day before and a day after $cid
        //if(strtotime($event->field_time['und'][0]['value2']) < strtotime($events[$cid]->field_time['und'][0]['value']) - 86400)
        //    continue;

        //if(strtotime($event->field_time['und'][0]['value']) > strtotime($events[$cid]->field_time['und'][0]['value2']) + 86400)
        //    break;

        // ignore events that can also move time as a result of this function
        // TODO: modify this for post-study and free-study
        if($e == $cid ||
            // ignore free study events when placing prework and sr events, but not when placing other free study
            ($event->field_event_type['und'][0]['value'] == 'f' && $events[$cid]->field_event_type['und'][0]['value'] != 'f') ||
            // sr events get placed after pre-work, ignore them on the first pass, but don't ignore them if we are placing SR events
            ($event->field_event_type['und'][0]['value'] == 'sr' && $events[$cid]->field_event_type['und'][0]['value'] != 'sr') || $event->field_event_type['und'][0]['value'] == 'h' ||
            isset($event->field_day_of_the_week['und'][0]['value']))
            continue;


        // TODO: if the event was moved manually don't move it


        // TODO: if our buckets are overflowed, even the buckets out

        $start = new DateTime($event->field_time['und'][0]['value'], new DateTimeZone('UTC'));
        $end = new DateTime($event->field_time['und'][0]['value2'], new DateTimeZone('UTC'));
        foreach($timeslots as $k => $count)
        {
            $bucketStart = new DateTime('now', new DateTimeZone($event->field_time['und'][0]['timezone']));
            $bucketStart->setTime($k, 0, 0);
            $bucketStart->setTimezone(new DateTimeZone('UTC'));
            $bucketStart->setDate($start->format('Y'), $start->format('m'), $start->format('d'));
            $bucketEnd = clone $bucketStart;
            $bucketEnd->add(new DateInterval('PT5H'));
            //if($end < $bucketStart || $start > $bucketEnd)
            //    continue;

            $length = 0;
            // if the event overlaps with a bucket, count it
            if($end >= $bucketStart && $start <= $bucketStart)
            {
                $length = date_timestamp_get($end) - date_timestamp_get($bucketStart);
            }
            elseif($start <= $bucketEnd && $end >= $bucketEnd)
            {
                $length = date_timestamp_get($bucketEnd) - date_timestamp_get($start);
            }
            $buckets[$bucketStart->format('Y/m/d')][$k] = isset($buckets[$bucketStart->format('Y/m/d')][$k]) ? ($buckets[$bucketStart->format('Y/m/d')][$k] + $length / 3600.0) : ($length / 3600.0);
        }

    }

    // find the best bucket to put this event into
    $start = new DateTime($events[$cid]->field_time['und'][0]['value'], new DateTimeZone('UTC'));
    foreach($timeslots as $k => $count)
    {
        $bucketStart = clone $start;
        $bucketStart->setTimezone(new DateTimeZone($events[$cid]->field_time['und'][0]['timezone']));
        $bucketStart->setTime($k, 0, 0);
        $bucketStart->setTimezone(new DateTimeZone('UTC'));
        // check if the timeslot is before the class, study sessions cannot be moved before classes
        if($notBefore)
        {
            $end = new DateTime($events[$cid]->field_time['und'][0]['value'], new DateTimeZone('UTC'));
            if($bucketStart->getTimestamp() < $end->getTimestamp())
                continue;
        }

        if(!isset($buckets[$bucketStart->format('Y/m/d')][$k]) || $buckets[$bucketStart->format('Y/m/d')][$k] < $count ||
            // we are over the amount for the day
            array_sum(array_values($timeslots)) <= array_sum(array_values($buckets[$bucketStart->format('Y/m/d')])))
        {

            // calculate how far the event has to move to be in the bucket accounting for change in timezone
            $diff = $bucketStart->getTimestamp() - $start->getTimestamp();

            $newTime = array(
                0 => date('Y/m/d H:i:s', strtotime($events[$cid]->field_time['und'][0]['value']) + $diff),
                1 => date('Y/m/d H:i:s', strtotime($events[$cid]->field_time['und'][0]['value2']) + $diff));
            return $newTime;
        }
    }

    return array(
        0 => $events[$cid]->field_time['und'][0]['value'],
        1 => $events[$cid]->field_time['und'][0]['value2']);

}

function studysauce_remove_overlaps($node, $events, $cid)
{
    // move overlapping study times
    // TODO: set this to > 0 to activate overlap-removal
    $safety = 20; // our safety is high because we are only moving at 1/6 * 60 increments
    while(--$safety >= 0)
    {

        uasort($events, function ($a, $b) use ($events) {
            // put all of the details at the beginning
            if(isset($a->field_day_of_the_week['und']) && !empty($a->field_day_of_the_week['und']))
                return -count($events);
            if(isset($b->field_day_of_the_week['und']) && !empty($b->field_day_of_the_week['und']))
                return count($events);

            return strtotime($a->field_time['und'][0]['value']) - strtotime($b->field_time['und'][0]['value']);
        });

        // move overlapping study times for events next to $cid
        $anyMoved = false;
        $cidIndex = array_search($cid, array_keys($events));
        if($cidIndex > 0)
        {
            $prev = array_keys($events)[$cidIndex - 1];
            // TODO: make sure the previous event isn't an all day
            if(strtotime($events[$prev]->field_time['und'][0]['value2']) >= strtotime($events[$cid]->field_time['und'][0]['value']) &&
                // ignore class entries, this is just for display
                (!isset($events[$prev]->field_day_of_the_week['und']) || empty($events[$prev]->field_day_of_the_week['und']))
                // TODO: ignore all day events
            )
            {
                //$diff = strtotime($events[$prev]->field_time['und'][0]['value2']) - strtotime($events[$cid]->field_time['und'][0]['value']) + 900; // tried to move to end times of other classes and it didn't work very well
                $diff = 60 * 15; // tried to move to end times of other classes and it didn't work very well

                $events[$cid]->field_time['und'][0]['value'] = date('Y/m/d H:i:s', strtotime($events[$cid]->field_time['und'][0]['value']) + $diff);
                $events[$cid]->field_time['und'][0]['value2'] = date('Y/m/d H:i:s', strtotime($events[$cid]->field_time['und'][0]['value2']) + $diff);
                $anyMoved = true;
            }
        }

        // TODO: if we are moving ahead, don't move it back in to an invalid location again
        if($cidIndex < count($events) - 1 && !$anyMoved)
        {
            $next = array_keys($events)[$cidIndex + 1];

            // TODO: make sure the next event isn't an all day
            if(strtotime($events[$next]->field_time['und'][0]['value']) <= strtotime($events[$cid]->field_time['und'][0]['value2']) &&
                // ignore class entries, this is just for display
                (!isset($events[$next]->field_day_of_the_week['und']) || empty($events[$next]->field_day_of_the_week['und']))
                // TODO: ignore all day events
            )
            {
                $diff = 60 * 15; // move 15 minutes
                $events[$cid]->field_time['und'][0]['value'] = date('Y/m/d H:i:s', strtotime($events[$cid]->field_time['und'][0]['value']) + $diff);
                $events[$cid]->field_time['und'][0]['value2'] = date('Y/m/d H:i:s', strtotime($events[$cid]->field_time['und'][0]['value2']) + $diff);
                $anyMoved = true;
            }
        }

        if(!$anyMoved)
            break;
    }

    return array(
        0 => $events[$cid]->field_time['und'][0]['value'],
        1 => $events[$cid]->field_time['und'][0]['value2']);
}

function studysauce_add_prework_events($node, &$events, $cid)
{
    $classStart = new DateTime($events[$cid]->field_time['und'][0]['value'], new DateTimeZone('UTC'));
    $classEnd = new DateTime($events[$cid]->field_time['und'][0]['value2'], new DateTimeZone('UTC'));

    $length = strtotime($classEnd->format('1/1/1970 H:i:s')) - strtotime($classStart->format('1/1/1970 H:i:s'));
    if($length <= 0)
        $length += 86400;
    if($length > 7200)
        $length = 7200;

    // add repeating other events
    $startTerm = strtotime('this week', $classStart->getTimestamp());
    $endTerm = strtotime('this week', $classEnd->getTimestamp()) + 604800;
    for ($w = $startTerm; $w < $endTerm; $w += 604800)
    {
        foreach ($events[$cid]->field_day_of_the_week['und'] as $j => $dotw)
        {
            $d = 0;
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
                    continue;
            }

            $t = strtotime('this week', $w) + $d;
            if($t < $classStart->getTimestamp() || $t > $classEnd->getTimestamp())
                continue;

            $classT = new DateTime();
            $classT->setTimestamp($t);
            $classT->setTimezone(new DateTimeZone('UTC'));
            $classT->setTime($classStart->format('H'), $classStart->format('i'), $classStart->format('s'));
            $classT->sub(new DateInterval('P1D'));

            $entity = entity_create('field_collection_item', array('field_name' => 'field_classes'));
            $entity->setHostEntity('node', $node);
            $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $events[$cid]->field_class_name['und'][0]['value'];
            $entity->field_day_of_the_week[LANGUAGE_NONE] = array();
            $entity->field_event_type[LANGUAGE_NONE][0]['value'] = 'p';
            $entity->field_time[LANGUAGE_NONE][0] = array(
                'value' => $classT->format('Y/m/d H:i:s'),
                'value2' => date_add($classT, new DateInterval('PT' . $length . 'S'))->format('Y/m/d H:i:s')
            );
            $key = -count($events);
            $events[$key] = $entity;

            // TODO: don't change times for events that have been moved

            // find the right time for the event a day before
            list($entity->field_time[LANGUAGE_NONE][0]['value'], $entity->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_get_preferred_time($node, $events, $key);

            // remove overlap from newly placed event
            list($entity->field_time[LANGUAGE_NONE][0]['value'], $entity->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_remove_overlaps($node, $events, $key);
        }
    }
    // do nothing more for singly occurring events

}

function studysauce_add_study_events($node, &$events, $cid)
{
    $classStart = new DateTime($events[$cid]->field_time['und'][0]['value'], new DateTimeZone('UTC'));
    $classEnd = new DateTime($events[$cid]->field_time['und'][0]['value2'], new DateTimeZone('UTC'));

    $length = strtotime($classEnd->format('1/1/1970 H:i:s')) - strtotime($classStart->format('1/1/1970 H:i:s'));
    if($length <= 0)
        $length += 86400;
    $fullLength = $length;
    if($length > 7200)
        $length = 7200;

    // add repeating other events
    $startTerm = strtotime('this week', $classStart->getTimestamp());
    $endTerm = strtotime('this week', $classEnd->getTimestamp()) + 604800;
    for ($w = $startTerm; $w < $endTerm; $w += 604800)
    {
        foreach ($events[$cid]->field_day_of_the_week['und'] as $j => $dotw)
        {
            $d = 0;
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
                    continue;
            }

            $t = strtotime('this week', $w) + $d;
            if($t < $classStart->getTimestamp() || $t > $classEnd->getTimestamp())
                continue;

            $classT = new DateTime();
            $classT->setTimestamp($t);
            $classT->setTimezone(new DateTimeZone('UTC'));
            $classT->setTime($classStart->format('H'), $classStart->format('i'), $classStart->format('s'));
            $classT->add(new DateInterval('PT' . $fullLength . 'S'));

            $entity = entity_create('field_collection_item', array('field_name' => 'field_classes'));
            $entity->setHostEntity('node', $node);
            $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $events[$cid]->field_class_name['und'][0]['value'];
            $entity->field_day_of_the_week[LANGUAGE_NONE] = array();
            $entity->field_event_type[LANGUAGE_NONE][0]['value'] = 'sr';
            $entity->field_time[LANGUAGE_NONE][0] = array(
                'value' => $classT->format('Y/m/d H:i:s'),
                'value2' => date_add($classT, new DateInterval('PT' . $length . 'S'))->format('Y/m/d H:i:s')
            );
            $key = -count($events);
            $events[$key] = $entity;

            // TODO: don't change times for events that have been moved

            // find the right time for the event a day before
            list($entity->field_time[LANGUAGE_NONE][0]['value'], $entity->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_get_preferred_time($node, $events, $key, true);

            // remove overlap from newly placed event
            list($entity->field_time[LANGUAGE_NONE][0]['value'], $entity->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_remove_overlaps($node, $events, $key);
        }
    }
    // do nothing more for singly occurring events

}

function studysauce_add_free_study_events($node, $events)
{
    // calculate how much study is already allocated
    $classTotals = array();
    $studyTotals = array();
    foreach($events as $cid => $event)
    {
        if(!isset($event->field_event_type['und'][0]['value']) ||  $event->field_event_type['und'][0]['value'] == 'c')
        {
            // skip the data entry events
            if(isset($event->field_day_of_the_week['und']) && !empty($event->field_day_of_the_week['und']))
                continue;

            $classStart = new DateTime($event->field_time['und'][0]['value']);
            $classEnd = new DateTime($event->field_time['und'][0]['value2']);

            // get class length
            $length = strtotime($classEnd->format('1/1/1970 H:i:s')) - strtotime($classStart->format('1/1/1970 H:i:s'));
            if($length <= 0)
                $length += 86400;
            if($length > 7200)
                $length = 7200;

            // add the length to the specific week
            $classStart->setTime(0,0,0);
            $startWeek = strtotime('last sunday', $classStart->getTimestamp());
            // if we get last Sunday, we actually wanted this sunday
            if($startWeek + 604800 == $classStart->getTimestamp())
                $startWeek += 604800;
            $classTotals[$startWeek] = (isset($classTotals[$startWeek]) ? $classTotals[$startWeek] : 0) + $length;
        }
        elseif($event->field_event_type['und'][0]['value'] == 'sr' || $event->field_event_type['und'][0]['value'] == 'p')
        {
            $classStart = new DateTime($event->field_time['und'][0]['value']);
            $classEnd = new DateTime($event->field_time['und'][0]['value2']);

            // get class length
            $length = strtotime($classEnd->format('1/1/1970 H:i:s')) - strtotime($classStart->format('1/1/1970 H:i:s'));
            if($length <= 0)
                $length += 86400;
            if($length > 7200)
                $length = 7200;

            // add the length to the specific week
            $classStart->setTime(0,0,0);
            $startWeek = strtotime('last sunday', $classStart->getTimestamp());
            // if we get last Sunday, we actually wanted this sunday
            if($startWeek + 604800 == $classStart->getTimestamp())
                $startWeek += 604800;
            $studyTotals[$startWeek] = (isset($studyTotals[$startWeek]) ? $studyTotals[$startWeek] : 0) + $length;
        }
    }

    // add free study to every
    foreach($classTotals as $w => $totalLength)
    {
        $studyLength = isset($studyTotals[$w]) ? $studyTotals[$w] : 0;
        $weekends = (isset($node->field_weekends['und'][0]['value']) && $node->field_weekends['und'][0]['value'] == 'hit_hard');

        // TODO: adjust study factor based on nothing but A's preference, only affect free time, can we do more?
        $studyFactor = 2.4;
        if(isset($node->field_grades['und'][0]['value']) && $node->field_grades['und'][0]['value'] == 'as_only')
            $studyFactor = 2.8;

        // there are 16 usable hours, 4 * (5 - 1) = 16 * 7 days = 112 usable hours in a week
        $shouldStudy = min($totalLength * $studyFactor, ($weekends ? 112 : 80) * 3600 - $totalLength); // a student should study 3 [2.4] times outside of class
        $remainingStudy = min(14 * 3600, // turns out filling up the entire week is too much
            max($shouldStudy - $studyLength, 3600 * 5)); // subtract the hours already accounted for by schedules study sessions
        $freeHours = floor($remainingStudy / 3600);
        for($j = 0; $j < $freeHours; $j++)
        {
            // use weekends setting to determine if the first free study should fall on a sunday
            // subtract a day because the week starts on Monday
            // TODO: find day with least classes
            if($weekends && $j < $freeHours / 2)
                $freeT = $w - 86400;
            elseif($weekends && $j > $freeHours / 2)
                $freeT = $w + 432000;
            else
                $freeT = $w + ($weekends ? 168 : 120) / $freeHours * $j * 3600 + ($weekends ? 0 : 86400);

            $classT = new DateTime();
            $classT->setTimestamp($freeT);

            $entity = entity_create('field_collection_item', array('field_name' => 'field_classes'));
            $entity->setHostEntity('node', $node);
            $entity->field_class_name[LANGUAGE_NONE][0]['value'] = 'Free study';
            $entity->field_day_of_the_week[LANGUAGE_NONE] = array();
            $entity->field_event_type[LANGUAGE_NONE][0]['value'] = 'f';
            $entity->field_time[LANGUAGE_NONE][0] = array(
                'value' => $classT->format('Y/m/d H:i:s'),
                'value2' => date_add($classT, new DateInterval('PT3600S'))->format('Y/m/d H:i:s')
            );
            $key = -count($events);
            $events[$key] = $entity;

            // find the right time for the event a day before
            list($entity->field_time[LANGUAGE_NONE][0]['value'], $entity->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_get_preferred_time($node, $events, $key);

            // remove overlap from newly placed event
            list($entity->field_time[LANGUAGE_NONE][0]['value'], $entity->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_remove_overlaps($node, $events, $key);
        }
    }
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
        $node->revision = false;
    }
    else
    {
        $node = new stdClass();
        $node->title = isset($user->mail) ? $user->mail : '';
        $node->type = 'schedule';
        $node->language = LANGUAGE_NONE; // Or e.g. 'en' if locale is enabled
        $node->uid = $user->uid;
        $node->status = 1; //(1 or 0): published or not
        node_object_prepare($node); // Sets some defaults. Invokes hook_prepare() and hook_node_prepare().
        $node = node_submit($node); // Prepare node for saving
        node_save($node);
    }

    // save profile
    if(isset($_POST['university']) || isset($_POST['grades']) || isset($_POST['weekends']) ||
        isset($_POST['6-am-11-am']) || isset($_POST['11-am-4-pm']) || isset($_POST['4-pm-9-pm']) ||
        isset($_POST['9-pm-2-am']))
    {
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
    }

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
        $events = studysauce_get_events();
        $removed = array();

        foreach($_POST['classes'] as $j => $class)
        {
            // check if class entity already exists
            if(!isset($events[$class['cid']]))
            {
                $entity = entity_create('field_collection_item', array('field_name' => 'field_classes'));
                $entity->setHostEntity('node', $node);
            }
            else
            {
                $entity = $events[$class['cid']];
                $entity->revision = false;

                // figure out what changed
                if(isset($entity->field_class_name[LANGUAGE_NONE][0]['value']) &&
                    $entity->field_class_name[LANGUAGE_NONE][0]['value'] != $class['className'])
                {
                    // remove old reoccurring events
                    studysauce_remove_old_events($node, $events, $class['cid']);
                    $removed[] = $class['cid'];
                }
            }
            $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $class['className'];
            $dotw = explode(',', $class['dotw']);
            $entity->field_day_of_the_week[LANGUAGE_NONE] = array_map(function ($x) { return array('value' => $x); }, $dotw);
            $entity->field_event_type[LANGUAGE_NONE][0]['value'] = $class['type'];
            $entity->field_time[LANGUAGE_NONE][0] = array(
                'value' => gmdate('Y-m-d H:i:s', strtotime($class['start'])),
                'value2' => gmdate('Y-m-d H:i:s', strtotime($class['end']))
            );
            $entity->save(true);
            if(!isset($events[$class['cid']]))
            {
                $events[$entity->item_id] = $entity;
            }
        }

        // TODO: make this more efficient, don't remove class items that haven't changed
        $classes = _studysauce_get_schedule_classes();
        foreach($classes as $cid => $class)
        {
            // check if we already removed all elements
            if(in_array($cid, $removed))
                continue;

            // rebuild entire schedule
            studysauce_remove_old_events($node, $events, $cid);

            studysauce_add_reoccurring_events($node, $events, $cid);
        }

        foreach($classes as $cid => $class)
        {
            // set up pre-work
            if($events[$cid]->field_event_type[LANGUAGE_NONE][0]['value'] == 'c')
            {
                studysauce_add_prework_events($node, $events, $cid);
            }
        }

        foreach($classes as $cid => $class)
        {
            // set up sr-sessions
            if($events[$cid]->field_event_type[LANGUAGE_NONE][0]['value'] == 'c')
            {
                studysauce_add_study_events($node, $events, $cid);
            }
        }

        // if there are any changes render the free study
        studysauce_add_free_study_events($node, $events);
        node_save($node);
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
                $events = studysauce_get_events();
                studysauce_remove_old_events($node, $events, $_POST['remove'], true);
            }
        }
    }


    // get event schedule
    $newSDS = studysauce_get_sds_messages();
    $lastSDS = end($newSDS);
    list($awards) = studysauce_get_awards();
    list($times, $rows, $total, $hours) = _studysauce_get_metrics();
    global $studysauceExportClasses, $studysauceExportEvents;
    print json_encode(array(
        'schedule' => '<div>' . theme('studysauce-schedule') . '</div>',
        'plan' => '<div>' . theme('studysauce-plans') . '</div>',
        'events' => $studysauceExportEvents,
        'classes' => $studysauceExportClasses,
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

        $entities = array();
        if (!isset($node->field_classes[LANGUAGE_NONE]) || !is_array($node->field_classes[LANGUAGE_NONE]))
            return $entities;

        foreach ($node->field_classes[LANGUAGE_NONE] as $i => $class)
        {
            $eid = $class['value'];
            $entity = entity_load('field_collection_item', array($eid));
            if (!empty($entity))
            {
                if (isset($entity[$eid]->field_class_name['und'][0]['value']) && !empty($entity[$eid]->field_class_name['und'][0]['value'])) {

                    // this is use by schedule only, if everything is filled in display the event
                    if (isset($entity[$eid]->field_time['und'][0]['value']) &&
                        isset($entity[$eid]->field_time['und'][0]['value2'])
                    ) {
                        $entities[$eid] = $entity[$eid];
                    }
                }
            }
        }

        uasort($entities, function ($a, $b) {
            return strtotime($a->field_time['und'][0]['value']) - strtotime($b->field_time['und'][0]['value']);
        });

        return $entities;
    }

    return array();
}


