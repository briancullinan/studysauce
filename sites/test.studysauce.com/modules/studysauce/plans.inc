<?php

global $holidays;
$holidays = array(
    date('Y') . '/09/01' => 'Memorial day',
    date('Y') . '/11/27' => 'Thanksgiving',
    date('Y') . '/11/28' => 'Thanksgiving',
    date('Y') . '/12/25' => 'Christmas',
    date('Y') . '/12/26' => 'Christmas',
    date('Y') . '/01/19' => 'Martin Luther King Jr.',
    date('Y') + 1 . '/09/01' => 'Memorial day',
    date('Y') + 1 . '/11/27' => 'Thanksgiving',
    date('Y') + 1 . '/11/28' => 'Thanksgiving',
    date('Y') + 1 . '/12/25' => 'Christmas',
    date('Y') + 1 . '/12/26' => 'Christmas',
    date('Y') + 1 . '/01/19' => 'Martin Luther King Jr.'

);

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
        list($events) = studysauce_get_events();

        foreach($_POST['strategies'] as $i => $strategy)
        {
            // check if the event name actually exists ins schedule
            if(isset($events[$strategy['eid']]))
            {
                // check if the strategy is already saved
                $field = 'field_' . $strategy['type'] . '_strategies';
                $strategies = $node->$field;
                if(isset($strategies['und']) && is_array($strategies['und']))
                {
                    $saved = db_select('field_data_field_class_name', 'ti')
                        ->fields('ti', array('entity_id'))
                        ->condition('bundle', $field, '=')
                        ->condition('field_class_name_value', '% (' . $strategy['eid'] . ')', 'LIKE')
                        ->condition('entity_id', array_map(function ($x) {return $x['value'];}, $strategies['und']), 'IN')
                        ->execute()
                        ->fetchAssoc();
                    if($saved)
                    {
                        $entity = entity_load('field_collection_item', array($saved['entity_id']));
                        if(isset($entity[$saved['entity_id']]))
                            $existing = $entity[$saved['entity_id']];
                    }
                }

                // remove the strategy
                if(isset($strategy['remove']))
                {
                    if(isset($existing))
                    {
                        entity_delete('field_collection_item', array($existing->item_id));
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
                        $existing->field_class_name['und'][0]['value'] = $events[$strategy['eid']]->field_class_name['und'][0]['value'] . ' (' . $strategy['eid'] . ')';
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


                    $existing->save();

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
            $entities = $node->$field;
            if(!isset($entities['und']) || empty($entities['und']))
                continue;

            $strategies = entity_load('field_collection_item', array_map(function ($x) {return $x['value'];}, $entities['und']));
            foreach($strategies as $i => $entity)
            {
                if(!isset($entity->field_class_name['und'][0]['value']) || empty($entity->field_class_name['und'][0]['value']) ||
                    !preg_match('/\s\(([0-9]+)\)$/i', $entity->field_class_name['und'][0]['value'], $matches))
                    continue;

                $eid = $matches[1];
                $result[$eid][$strategy]['default'] = isset($entity->field_default['und'][0]['value'])
                    ? $entity->field_default['und'][0]['value']
                    : false;
                if($strategy == 'active')
                {
                    $result[$eid][$strategy]['skim'] = isset($entity->field_skim['und'][0]['value'])
                        ? $entity->field_skim['und'][0]['value']
                        : '';
                    $result[$eid][$strategy]['why'] = isset($entity->field_why['und'][0]['value'])
                        ? $entity->field_why['und'][0]['value']
                        : '';
                    $result[$eid][$strategy]['questions'] = isset($entity->field_questions['und'][0]['value'])
                        ? $entity->field_questions['und'][0]['value']
                        : '';
                    $result[$eid][$strategy]['summarize'] = isset($entity->field_summarize['und'][0]['value'])
                        ? $entity->field_summarize['und'][0]['value']
                        : '';
                    $result[$eid][$strategy]['exam'] = isset($entity->field_exam['und'][0]['value'])
                        ? $entity->field_exam['und'][0]['value']
                        : '';
                }
                elseif($strategy == 'other')
                {
                    $result[$eid][$strategy]['notes'] = isset($entity->field_notes['und'][0]['value'])
                        ? $entity->field_notes['und'][0]['value']
                        : '';
                }
                elseif($strategy == 'teach')
                {
                    $result[$eid][$strategy]['title'] = isset($entity->field_title['und'][0]['value'])
                        ? $entity->field_title['und'][0]['value']
                        : '';
                    $result[$eid][$strategy]['notes'] = isset($entity->field_notes['und'][0]['value'])
                        ? $entity->field_notes['und'][0]['value']
                        : '';
                    if(isset($entity->field_teaching['und'][0]['fid']))
                    {
                        $result[$eid][$strategy]['uploads'][0]['fid'] = $entity->field_teaching['und'][0]['fid'];
                        $result[$eid][$strategy]['uploads'][0]['thumbnail'] = $entity->field_teaching['und'][0]['thumbnail'];
                        $result[$eid][$strategy]['uploads'][0]['uri'] = image_style_url('achievement', $entity->field_teaching['und'][0]['thumbnailfile']->uri);

                        // Load the derived files
                        $file = file_load($entity->field_teaching['und'][0]['fid']);
                        $outputs = db_select('video_output', 'vo')
                            ->fields('vo')
                            ->condition('vo.original_fid', array($entity->field_teaching['und'][0]['fid']), 'IN')
                            ->execute()->fetchAllAssoc('output_fid');
                        foreach ($outputs as $outputfid => $output) {
                            $trans = file_load($output->output_fid);
                            if($trans->filesize > 0)
                                $result[$eid][$strategy]['uploads'][0]['play'] = check_plain(file_create_url($trans->uri));
                        }
                        if(empty($outputs))
                        {
                            $result[$eid][$strategy]['uploads'][0]['play'] = check_plain(file_create_url($file->uri));
                            $video = video_jobs::load($entity->field_teaching['und'][0]['fid']);
                            if (!$video) {
                                list($entity_id, $entity_vid, $bundle) = entity_extract_ids('field_collection_item', $entity);
                                // @todo get the default dimension when not available in $item
                                if (!video_jobs::create($file->fid, '320x180', $entity_id, 'field_collection_item', 'field_teaching', 'und', 0)) {
                                    drupal_set_message(t('Something went wrong with your video job creation. Please check your recent log entries for further debugging.'), 'error');

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
                    $result[$eid][$strategy]['notes'] = isset($entity->field_notes['und'][0]['value'])
                        ? $entity->field_notes['und'][0]['value']
                        : '';
                    $result[$eid][$strategy]['review'] = isset($entity->field_review['und'][0]['value'])
                        ? implode(',', array_map(function ($x) { return $x['value']; }, $entity->field_review['und']))
                        : '';
                }
                elseif($strategy == 'prework')
                {
                    $result[$eid][$strategy]['notes'] = isset($entity->field_notes['und'][0]['value'])
                        ? $entity->field_notes['und'][0]['value']
                        : '';
                    $result[$eid][$strategy]['prepared'] = isset($entity->field_prepared['und'][0]['value'])
                        ? implode(',', array_map(function ($x) { return $x['value']; }, $entity->field_prepared['und']))
                        : '';
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
        list($events) = studysauce_get_events();
        if(isset($events[$_POST['cid']]))
        {
            db_update('studysauce_events')
                ->fields(array('completed' => 1))
                ->condition('eid', $_POST['cid'], '=')
                ->execute();
        }
    }
}

function studysauce_get_strategies_titles()
{
    global $user;
    $titles = array();

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
        foreach(array('active', 'other', 'spaced', 'teach', 'prework') as $i => $strategy)
        {
            $field = 'field_' . $strategy . '_strategies';
            $strategies = $node->$field;
            if(!isset($strategies['und']) || empty($strategies['und']))
                continue;

            $saved = db_select('field_data_field_class_name', 'ti')
                ->fields('ti', array('entity_id', 'field_class_name_value'))
                ->condition('bundle', $field, '=')
                ->condition('entity_id', array_map(function ($x) {return $x['value'];}, $strategies['und']), 'IN')
                ->execute()
                ->fetchAllAssoc('entity_id');

            $titles = array_merge($titles, array_map(function ($x) { return $x->field_class_name_value;}, $saved));
        }
    }

    return $titles;
}

function studysauce_remove_old_events(&$node, &$events, $renamed)
{
    $saved = array();

    // get a list of strategy titles that have been saved
    $titles = studysauce_get_strategies_titles();
    $strategyEvents = array();
    foreach($titles as $sid => $title)
    {
        if(!isset($title) || empty($title) ||
            !preg_match('/\s\(([0-9]+)\)$/i', $title, $matches))
            continue;

        $eid = $matches[1];
        $strategyEvents[$eid] = $sid;
    }

    // only save events that have been moved intentionally and have strategies associated with them
    foreach($events as $eid => $event)
    {
        // TODO: uncomment this to only save events with data attached
        if(isset($strategyEvents[$eid]) || $event->field_user_moved['und'][0]['value'])
            $saved[$eid] = $event;
    }

    studysauce_sort_events($saved);

    return $saved;
}

function studysauce_add_reoccurring_events($node, &$events, $class, $saved)
{
    global $holidays;

    $classStart = new DateTime($class->field_time['und'][0]['value'], new DateTimeZone('UTC'));
    $classEnd = new DateTime($class->field_time['und'][0]['value2'], new DateTimeZone('UTC'));

    $length = strtotime($classEnd->format('1/1/1970 H:i:s')) - strtotime($classStart->format('1/1/1970 H:i:s'));
    if($length <= 0)
        $length += 86400;

    // add repeating other events
    $startTerm = strtotime('this week', $classStart->getTimestamp());
    $endTerm = strtotime('this week', $classEnd->getTimestamp()) + 604800;
    for ($w = $startTerm; $w < $endTerm; $w += 604800)
    {
        foreach ($class->field_day_of_the_week['und'] as $j => $dotw)
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
            if(isset($holidays[$classT->format('Y/m/d')]))
                continue;
            $classT->setTimezone(new DateTimeZone('UTC'));
            $classT->setTime($classStart->format('H'), $classStart->format('i'), $classStart->format('s'));

            // check if we have a saved event around the same time
            $lastEid = 0;
            $lastDistance = 172800;
            // TODO: use each so we save our location
            foreach($saved as $eid => $event)
            {
                // if saved is 24 hours before current event, skip and never go back because we are ordered by time
                $distance = abs(strtotime($event->field_time['und'][0]['value']) - $classT->getTimestamp());
                if($distance < $lastDistance)
                {
                    $lastEid = $eid;
                    $lastDistance = $distance;
                }
                elseif($lastEid > 0)
                    break;
            }

            if($lastEid)
            {
                $events[$lastEid] = $saved[$lastEid];
                unset($saved[$lastEid]);
                $events[$lastEid]->field_time[LANGUAGE_NONE][0] = array(
                    'value' => $classT->format('Y/m/d H:i:s'),
                    'value2' => date_add($classT, new DateInterval('PT' . $length . 'S'))->format('Y/m/d H:i:s')
                );

                studysauce_bucket_event($node, $events, $lastEid);
            }
            else
            {

                $entity = new stdClass();
                $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $class->field_class_name['und'][0]['value'];
                $entity->field_event_type[LANGUAGE_NONE][0]['value'] = $class->field_event_type[LANGUAGE_NONE][0]['value'];
                $entity->field_time[LANGUAGE_NONE][0] = array(
                    'value' => $classT->format('Y/m/d H:i:s'),
                    'value2' => date_add($classT, new DateInterval('PT' . $length . 'S'))->format('Y/m/d H:i:s')
                );
                $key = 'new' . count($events);
                $events[$key] = $entity;

                studysauce_bucket_event($node, $events, $key);
            }
        }
    }
    // do nothing more for singly occurring events

}

global $buckets;
$buckets = array();
function studysauce_bucket_event($node, $events, $cid)
{
    global $buckets, $classTotals, $studyTotals;

    // don't count all day events
    if($events[$cid]->field_event_type['und'][0]['value'] == 'h' || $events[$cid]->field_event_type['und'][0]['value'] == 'd' ||
        $events[$cid]->field_event_type['und'][0]['value'] == 'r')
        return;

    $start = new DateTime($events[$cid]->field_time['und'][0]['value'], new DateTimeZone('UTC'));
    $start->setTimezone(new DateTimeZone(date_default_timezone_get()));
    $end = new DateTime($events[$cid]->field_time['und'][0]['value2'], new DateTimeZone('UTC'));
    $end->setTimezone(new DateTimeZone(date_default_timezone_get()));

    // if the event is before 4 am put in the bucket from the previous day
    $bucket = clone $start;
    $bucket->setTime(0,0,0);
    $b = $bucket->format('Y/m/d');
    $h = 0;
    if(intval($start->format('H')) < 6)
    {
        $bucket->sub(new DateInterval('P1D'));
        $b = $bucket->format('Y/m/d');
        $h = 21;
    }
    elseif(intval($start->format('H')) < 11)
        $h = 6;
    elseif(intval($start->format('H')) < 16)
        $h = 11;
    elseif(intval($start->format('H')) < 21)
        $h = 16;
    elseif(intval($start->format('H')) < 24)
        $h = 21;

    $s = $start->getTimestamp();
    $e = $end->getTimestamp();
    $length = $e - $s + 900;
    $bucketStart = $bucket->getTimestamp();
    // iterate over buckets until length is used up
    while($bucketStart + $h * 3600 < $e)
    {
        // the night time bucket is actually 9 hours because we don't want to over-pack before 6 AM
        $time = min($bucketStart + $h * 3600 + ($h == 21 ? (3600 * 9) : (3600 * 5)), $e) - max($bucketStart + $h * 3600, $s);
        $buckets[$b][$h] = (isset($buckets[$b][$h]) ? $buckets[$b][$h] : 0) + $time / 3600.0;

        // skip sleepy time
        if($h == 21)
        {
            $h = 6;
            $bucket->add(new DateInterval('P1D'));
            $b = $bucket->format('Y/m/d');
            $bucketStart += 86400;
        }
        else
            $h += 5;
    }

    // add the length to the specific week
    $start->setTime(0,0,0);
    $startWeek = strtotime('last sunday', $start->getTimestamp());

    // if we get last Sunday, we actually wanted this sunday
    if($startWeek + 604800 == $start->getTimestamp())
        $startWeek += 604800;

    if($events[$cid]->field_event_type['und'][0]['value'] == 'c')
    {
        $classTotals[$startWeek] = (isset($classTotals[$startWeek]) ? $classTotals[$startWeek] : 0) + $length;
    }
    elseif($events[$cid]->field_event_type['und'][0]['value'] == 'sr' || $events[$cid]->field_event_type['und'][0]['value'] == 'p')
    {
        $studyTotals[$startWeek] = (isset($studyTotals[$startWeek]) ? $studyTotals[$startWeek] : 0) + $length;
    }
}


// there are 16 usable hours, 4 * (5 - 1) = 16
// we subtract 1 hour of break time from the following 5-hour time groups
// 6am-11am
// 11am-4pm
// 4pm-9pm
// 9pm-2am

// normalize each group value over 4, that leaves room for 15 minute breaks
global $timeslots;
$timeslots = array();
function studysauce_get_preferred_time($node, $events, $cid, $notBefore = false)
{
    global $timeslots, $buckets;
    if(empty($timeslots))
    {
        $timeslots = array(
            16 => isset($node->field_4_pm_9_pm['und'][0]['value']) ? intval($node->field_4_pm_9_pm['und'][0]['value']) / 5.0 * 4 : 2,
            21 => isset($node->field_11_am_4_pm['und'][0]['value']) ? intval($node->field_9_pm_2_am['und'][0]['value']) / 5.0 * 4 : 2,
            11 => isset($node->field_9_pm_2_am['und'][0]['value']) ? intval($node->field_11_am_4_pm['und'][0]['value']) / 5.0 * 4 : 2,
            6 => isset($node->field_6_am_11_am['und'][0]['value']) ? intval($node->field_6_am_11_am['und'][0]['value']) / 5.0 * 4 : 2
        );
        arsort($timeslots);
    }

    // find the best bucket to put this event into
    $start = new DateTime($events[$cid]->field_time['und'][0]['value'], new DateTimeZone('UTC'));
    $start->setTimezone(new DateTimeZone(date_default_timezone_get()));

    $bucket = clone $start;
    $bucket->setTime(0,0,0);
    $b = $bucket->format('Y/m/d');
    // if it starts before 6 AM put it in the previous nights bucket
    if(intval($start->format('H')) < 6)
    {
        $bucket->sub(new DateInterval('P1D'));
        $b = $bucket->format('Y/m/d');
    }

    $s = $start->getTimestamp();
    $bucketStart = $bucket->getTimestamp();

    if($b == '2014/07/21' && $events[$cid]->field_event_type['und'][0]['value'] == 'sr' &&
        $events[$cid]->field_class_name['und'][0]['value'] == 'Phys 101')
    {
        $hit = 'wtf?';
    }

    // we are over the amount for the day
    $overfilled = false;
    if(isset($buckets[$b]) && array_sum(array_values($buckets[$b])) >= array_sum(array_values($timeslots)))
    {
        // add to the bucket with the least?
        $overfilled = true;
        $tmpTimeslots = array();

        // get timeslots that are not filled yet
        foreach($timeslots as $t => $c)
            if(!isset($buckets[$b][$t]) || $buckets[$b][$t] < 4)
                $tmpTimeslots[$t] = $c;

        // get timeslots in order of least filled
        if(empty($tmpTimeslots))
        {
            foreach($timeslots as $t => $c)
                $tmpTimeslots[$t] = isset($buckets[$b][$t]) ? $buckets[$b][$t] : 0;
            asort($tmpTimeslots);
        }
    }
    else
        $tmpTimeslots = $timeslots;

    foreach($tmpTimeslots as $h => $count)
    {
        // check if the timeslot is before the class, study sessions cannot be moved before classes
        // TODO: check for prework and notAfter the next class event
        if($notBefore)
        {
            if($bucketStart + $h * 3600 < $s)
                continue;
        }

        if(!isset($buckets[$b][$h]) || $buckets[$b][$h] < $count || $overfilled)
        {
            // TODO: check if the next day is emptier
            /*
            $next = clone $bucket;
            $next->add(new DateInterval('P1D'));
            $n = $next->format('Y/m/d');
            if($overfilled && isset($buckets[$n][6]) && $buckets[$n][6] > 3 &&
                array_sum($buckets[$n]) < $buckets[$b] / 2)
            {
                if($timeslots[11] > $timeslots[16])
                    $diff = $bucketStart + 86400 + 11 * 3600 - $s;
                else
                    $diff = $bucketStart + 86400 + 16 * 3600 - $s;
            }
            else
            */
                // calculate how far the event has to move to be in the bucket accounting for change in timezone
                $diff = $bucketStart + $h * 3600 - $s;

            $events[$cid]->field_time['und'][0]['value'] = date('Y/m/d H:i:s', strtotime($events[$cid]->field_time['und'][0]['value']) + $diff);
            $events[$cid]->field_time['und'][0]['value2'] = date('Y/m/d H:i:s', strtotime($events[$cid]->field_time['und'][0]['value2']) + $diff);

            // bucket events before overlaps are removed so we know if they got pushed out of their bucket and we don't repeat
            // TODO: fix this to trigger on the first time it happens by looking for empty spaces in plan
            //studysauce_bucket_event($node, $events, $cid);
            break;
        }
    }

    return array(
        0 => $events[$cid]->field_time['und'][0]['value'],
        1 => $events[$cid]->field_time['und'][0]['value2']);

}

function studysauce_sort_events(&$events)
{
    uasort($events, function ($a, $b) use ($events) {
        // put all of the details at the beginning
        if(isset($a->field_day_of_the_week['und']) && !empty($a->field_day_of_the_week['und']))
            return -count($events);
        if(isset($b->field_day_of_the_week['und']) && !empty($b->field_day_of_the_week['und']))
            return count($events);

        return strtotime($a->field_time['und'][0]['value']) - strtotime($b->field_time['und'][0]['value']);
    });

}

function studysauce_remove_overlaps($node, $events, $cid, $notBefore = false)
{
    $s = strtotime($events[$cid]->field_time['und'][0]['value']);
    $e = strtotime($events[$cid]->field_time['und'][0]['value2']);

    // get a list of events within a 24 hour time range to work with, we should never move more than that
    $workingEvents = array(
        'back' => clone $events[$cid],
        'forw' => clone $events[$cid]
    );
    foreach($events as $eid => $event)
        if(strtotime($event->field_time['und'][0]['value']) > $s - 86400 && strtotime($event->field_time['und'][0]['value2']) < $e + 86400 &&
            // skip cid because it's already added
            $eid != $cid &&
            // ignore class entries, this is just for display
            (!isset($event->field_day_of_the_week['und']) || empty($event->field_day_of_the_week['und'])) &&
            // ignore all day events
            $event->field_event_type['und'][0]['value'] != 'd' && $event->field_event_type['und'][0]['value'] != 'h' &&
            $event->field_event_type['und'][0]['value'] != 'r'
        )
            $workingEvents[$eid] = $event;


    if($events[$cid]->field_event_type['und'][0]['value'] == 'p' &&
        $events[$cid]->field_class_name['und'][0]['value'] == 'Phys 401' &&
        strtotime($events[$cid]->field_time['und'][0]['value']) > strtotime('2014/07/06 00:00:00'))
    {
        $hit = 'should not be overlapping';
    }

    // move overlapping study times
    $safety = 96; // our safety is high because we are only moving at 1/6 * 60 increments, don't move more than 24 hours
    while(--$safety > 0)
    {

        studysauce_sort_events($workingEvents);
        $keys = array_keys($workingEvents);

        // move overlapping study times for events next to $cid
        $backIndex = array_search('back', $keys); // every iteration move a copy forward and a copy backward
        $forwIndex = array_search('forw', $keys);
        $diff = 0;
        if(($forwIndex > 0 && ($prev = $keys[$forwIndex - 1]) != 'back' ||
            $forwIndex > 1 && ($prev = $keys[$forwIndex - 2]) != 'back') &&
            strtotime($workingEvents[$prev]->field_time['und'][0]['value2']) >= strtotime($workingEvents['forw']->field_time['und'][0]['value']))
        {
            $diff = 60 * 10; // tried to move to end times of other classes and it didn't work very well
        }
        elseif(($forwIndex < count($workingEvents) - 1 && ($prev = $keys[$forwIndex + 1]) != 'back' ||
            $forwIndex < count($workingEvents) - 2 && ($prev = $keys[$forwIndex + 2]) != 'back') &&
            strtotime($workingEvents[$prev]->field_time['und'][0]['value']) <= strtotime($workingEvents['forw']->field_time['und'][0]['value2']))
        {
            $diff = 60 * 10;
        }
        else
        {
            // success!
            return array(
                0 => $workingEvents['forw']->field_time['und'][0]['value'],
                1 => $workingEvents['forw']->field_time['und'][0]['value2']);
        }
        $newStart = date('Y/m/d H:i:s', strtotime($workingEvents['forw']->field_time['und'][0]['value']) + $diff);
        $newDate = new DateTime($newStart, new DateTimeZone('UTC'));
        $newDate->setTimezone(new DateTimeZone(date_default_timezone_get()));
        if(intval($newDate->format('H')) >= 6 || intval($newDate->format('H')) <= 2)
        {
            $workingEvents['forw']->field_time['und'][0]['value'] = date('Y/m/d H:i:s', strtotime($workingEvents['forw']->field_time['und'][0]['value']) + $diff);
            $workingEvents['forw']->field_time['und'][0]['value2'] = date('Y/m/d H:i:s', strtotime($workingEvents['forw']->field_time['und'][0]['value2']) + $diff);
        }

        $diff = 0;
        if(($backIndex > 0 && ($next = $keys[$backIndex - 1]) != 'forw' ||
            $backIndex > 1 && ($next = $keys[$backIndex - 2]) != 'forw') &&
            strtotime($workingEvents[$next]->field_time['und'][0]['value2']) >= strtotime($workingEvents['back']->field_time['und'][0]['value']))
        {
            $diff = -60 * 10;
        }
        elseif(($backIndex < count($workingEvents) - 1 && ($next = $keys[$backIndex + 1]) != 'forw' ||
            $backIndex < count($workingEvents) - 2 && ($next = $keys[$backIndex + 2]) != 'forw') &&
            strtotime($workingEvents[$next]->field_time['und'][0]['value']) <= strtotime($workingEvents['back']->field_time['und'][0]['value2']))
        {
            $diff = -60 * 10; // move 15 minutes
        }
        else
        {
            // success!
            return array(
                0 => $workingEvents['back']->field_time['und'][0]['value'],
                1 => $workingEvents['back']->field_time['und'][0]['value2']);
        }
        $newStart = date('Y/m/d H:i:s', strtotime($workingEvents['back']->field_time['und'][0]['value']) + $diff);
        $newDate = new DateTime($newStart, new DateTimeZone('UTC'));
        $newDate->setTimezone(new DateTimeZone(date_default_timezone_get()));
        if((intval($newDate->format('H')) >= 6 || intval($newDate->format('H')) <= 2) &&
            // make sure it doesn't come before the class
            (!$notBefore || $newDate->getTimestamp() >= $notBefore))
        {
            $workingEvents['back']->field_time['und'][0]['value'] = date('Y/m/d H:i:s', strtotime($workingEvents['back']->field_time['und'][0]['value']) + $diff);
            $workingEvents['back']->field_time['und'][0]['value2'] = date('Y/m/d H:i:s', strtotime($workingEvents['back']->field_time['und'][0]['value2']) + $diff);
        }

    }

    if($safety < 2)
    {
        $hit = 'something is wrong';
    }

    return array(
        0 => $events[$cid]->field_time['und'][0]['value'],
        1 => $events[$cid]->field_time['und'][0]['value2']);
}

function studysauce_add_prework_events($node, &$events, $class, $saved)
{
    $classStart = new DateTime($class->field_time['und'][0]['value'], new DateTimeZone('UTC'));
    $classEnd = new DateTime($class->field_time['und'][0]['value2'], new DateTimeZone('UTC'));

    $length = strtotime($classEnd->format('1/1/1970 H:i:s')) - strtotime($classStart->format('1/1/1970 H:i:s'));
    if($length <= 0)
        $length += 86400;
    //if($length > 7200)
    //    $length = 7200;

    // if class is easy only study for 30 minutes
    if(isset($class->field_study_difficulty['und'][0]['value']) && $class->field_study_difficulty['und'][0]['value'] == 'easy')
        $length /= 2;
    elseif(isset($class->field_study_difficulty['und'][0]['value']) && $class->field_study_difficulty['und'][0]['value'] == 'tough')
        $length *= 1.5;

    // add repeating other events
    $startTerm = strtotime('this week', $classStart->getTimestamp());
    $endTerm = strtotime('this week', $classEnd->getTimestamp()) + 604800;
    for ($w = $startTerm; $w < $endTerm; $w += 604800)
    {
        // get events this week plus next 24 hours since we only move forward
        $workingEvents = array();
        foreach($events as $e => $event)
            if(strtotime($event->field_time['und'][0]['value']) > $w - 86400 * 2 && strtotime($event->field_time['und'][0]['value2']) < $w + 604800 + 86400 * 2)
                $workingEvents[$e] = $event;

        foreach ($class->field_day_of_the_week['und'] as $j => $dotw)
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

            if($class->field_class_name['und'][0]['value'] == 'HIST 102')
            {
                $hit = 'something is wrong';
            }

            // check if we have a saved event around the same time
            $lastEid = 0;
            $lastDistance = 172800;
            // TODO: use each so we save our location
            foreach($saved as $eid => $event)
            {
                // if saved is 24 hours before current event, skip and never go back because we are ordered by time
                $distance = abs(strtotime($event->field_time['und'][0]['value']) - $classT->getTimestamp());
                if($distance < $lastDistance)
                {
                    $lastEid = $eid;
                    $lastDistance = $distance;
                }
                elseif($lastEid > 0)
                    break;
            }

            if($lastEid)
            {
                $events[$lastEid] = $saved[$lastEid];
                unset($saved[$lastEid]);
                $events[$lastEid]->field_time[LANGUAGE_NONE][0] = array(
                    'value' => $classT->format('Y/m/d H:i:s'),
                    'value2' => date_add($classT, new DateInterval('PT' . $length . 'S'))->format('Y/m/d H:i:s')
                );
                $workingEvents[$lastEid] = $events[$lastEid];

                // don't change times for events that have been moved
                if(!$events[$lastEid]->field_user_moved[LANGUAGE_NONE][0]['value'])
                    list($events[$lastEid]->field_time[LANGUAGE_NONE][0]['value'], $events[$lastEid]->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_get_preferred_time($node, $events, $lastEid);

                // move saved events if they overlap with a higher priority event, class, other, pre-work, study session
                list($events[$lastEid]->field_time[LANGUAGE_NONE][0]['value'], $events[$lastEid]->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_remove_overlaps($node, $workingEvents, $lastEid);

                studysauce_bucket_event($node, $events, $lastEid);
            }
            else
            {

                $entity = new stdClass();
                $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $class->field_class_name['und'][0]['value'];
                $entity->field_event_type[LANGUAGE_NONE][0]['value'] = 'p';
                $entity->field_time[LANGUAGE_NONE][0] = array(
                    'value' => $classT->format('Y/m/d H:i:s'),
                    'value2' => date_add($classT, new DateInterval('PT' . $length . 'S'))->format('Y/m/d H:i:s')
                );
                $key = 'new' . count($events);
                $events[$key] = $entity;
                $workingEvents[$key] = $entity;

                // find the right time for the event a day before
                list($entity->field_time[LANGUAGE_NONE][0]['value'], $entity->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_get_preferred_time($node, $events, $key);

                // remove overlap from newly placed event
                list($entity->field_time[LANGUAGE_NONE][0]['value'], $entity->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_remove_overlaps($node, $workingEvents, $key);

                studysauce_bucket_event($node, $events, $key);
            }
        }
    }
    // do nothing more for singly occurring events

}

function studysauce_add_study_events($node, &$events, $class, $saved)
{
    $classStart = new DateTime($class->field_time['und'][0]['value'], new DateTimeZone('UTC'));
    $classEnd = new DateTime($class->field_time['und'][0]['value2'], new DateTimeZone('UTC'));

    $length = strtotime($classEnd->format('1/1/1970 H:i:s')) - strtotime($classStart->format('1/1/1970 H:i:s'));
    if($length <= 0)
        $length += 86400;
    $fullLength = $length;
    //if($length > 7200)
    //    $length = 7200;

    // if class is easy only study for 30 minutes
    if(isset($class->field_study_difficulty['und'][0]['value']) && $class->field_study_difficulty['und'][0]['value'] == 'easy')
        $length /= 2;
    elseif(isset($class->field_study_difficulty['und'][0]['value']) && $class->field_study_difficulty['und'][0]['value'] == 'tough')
        $length *= 1.5;

    // SR sessions should go a month after the last class
    $isSr = $class->field_event_type['und'][0]['value'] == 'c' &&
        (!isset($class->field_study_type['und'][0]['value']) || $class->field_study_type['und'][0]['value'] == 'memorization');

    // add repeating other events
    $startTerm = strtotime('this week', $classStart->getTimestamp());
    $endTerm = strtotime('this week', $classEnd->getTimestamp()) + ($isSr ? 2419200 : 604800);
    for ($w = $startTerm; $w < $endTerm; $w += 604800)
    {
        // get events this week plus next 24 hours since we only move forward
        $workingEvents = array();
        foreach($events as $e => $event)
            if(strtotime($event->field_time['und'][0]['value']) > $w - 86400 * 2 && strtotime($event->field_time['und'][0]['value2']) < $w + 604800 + 86400 * 2)
                $workingEvents[$e] = $event;


        foreach ($class->field_day_of_the_week['und'] as $j => $dotw)
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
            if($t < $classStart->getTimestamp() || $t > $classEnd->getTimestamp() + ($isSr ? 2419200 : 0))
                continue;

            $classT = new DateTime();
            $classT->setTimestamp($t);
            $classT->setTimezone(new DateTimeZone('UTC'));
            $classT->setTime($classStart->format('H'), $classStart->format('i'), $classStart->format('s'));
            $classT->add(new DateInterval('PT' . $fullLength . 'S'));

            // check if we have a saved event around the same time
            $lastEid = 0;
            $lastDistance = 172800;
            // TODO: use each so we save our location
            foreach($saved as $eid => $event)
            {
                // if saved is 24 hours before current event, skip and never go back because we are ordered by time
                $distance = abs(strtotime($event->field_time['und'][0]['value']) - $classT->getTimestamp());
                if($distance < $lastDistance)
                {
                    $lastEid = $eid;
                    $lastDistance = $distance;
                }
                elseif($lastEid > 0)
                    break;
            }

            if($lastEid)
            {
                $events[$lastEid] = $saved[$lastEid];
                unset($saved[$lastEid]);
                $events[$lastEid]->field_time[LANGUAGE_NONE][0] = array(
                    'value' => $classT->format('Y/m/d H:i:s'),
                    'value2' => date_add($classT, new DateInterval('PT' . $length . 'S'))->format('Y/m/d H:i:s')
                );
                $workingEvents[$lastEid] = $events[$lastEid];

                // don't change times for events that have been moved
                if(!$events[$lastEid]->field_user_moved[LANGUAGE_NONE][0]['value'])
                    list($events[$lastEid]->field_time[LANGUAGE_NONE][0]['value'], $events[$lastEid]->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_get_preferred_time($node, $events, $lastEid, true);

                // move saved events if they overlap with a higher priority event, class, other, pre-work, study session
                list($events[$lastEid]->field_time[LANGUAGE_NONE][0]['value'], $events[$lastEid]->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_remove_overlaps($node, $workingEvents, $lastEid, $classT->getTimestamp());

                studysauce_bucket_event($node, $events, $lastEid);
            }
            else
            {

                $entity = new stdClass();
                $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $class->field_class_name['und'][0]['value'];
                $entity->field_event_type[LANGUAGE_NONE][0]['value'] = 'sr';
                $entity->field_time[LANGUAGE_NONE][0] = array(
                    'value' => $classT->format('Y/m/d H:i:s'),
                    'value2' => date_add($classT, new DateInterval('PT' . $length . 'S'))->format('Y/m/d H:i:s')
                );
                $key = 'new' . count($events);
                $events[$key] = $entity;
                $workingEvents[$key] = $entity;

                // find the right time for the event a day before
                list($entity->field_time[LANGUAGE_NONE][0]['value'], $entity->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_get_preferred_time($node, $events, $key, true);

                // remove overlap from newly placed event
                list($entity->field_time[LANGUAGE_NONE][0]['value'], $entity->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_remove_overlaps($node, $workingEvents, $key, $classT->getTimestamp());

                // TODO: check if we have late classes and early classes, maybe the afternoon would be better
                /*
                $next = clone $bucket;
                $next->add(new DateInterval('P1D'));
                $n = $next->format('Y/m/d');
                if(($h == 21 || $h == 16) && isset($buckets[$n][6]) && $buckets[$n][6] > 3 &&
                    $events[$cid]->field_event_type['und'][0]['value'] == 'sr' &&
                    (!isset($buckets[$n][11]) || $buckets[$n][11] < 4 || !isset($buckets[$n][16]) || $buckets[$n][16] < 4))
                {
                    if($timeslots[11] > $timeslots[16])
                        $diff = $bucketStart + 86400 + 11 * 3600 - $s;
                    else
                        $diff = $bucketStart + 86400 + 16 * 3600 - $s;
                }
                else
                */

                studysauce_bucket_event($node, $events, $key);
            }
        }
    }
    // do nothing more for singly occurring events

}

function studysauce_add_meals_events($node, &$events)
{
    global $classTotals;

    foreach($classTotals as $w => $totalLength)
    {
        $workingEvents = array();
        foreach($events as $e => $event)
            if(strtotime($event->field_time['und'][0]['value']) > $w - 86400 * 2 && strtotime($event->field_time['und'][0]['value2']) < $w + 604800 + 86400 * 2)
                $workingEvents[$e] = $event;

        // add meals to every day of the week
        for($j = 0; $j < 7; $j++)
        {
            $day = date('Y/m/d', $w + $j * 86400);
            // breakfast
            $classT = new DateTime($day, new DateTimeZone(date_default_timezone_get()));
            $classT->setTime(6,0,0);
            $classT->setTimezone(new DateTimeZone('UTC'));

            $entity = new stdClass();
            $entity->field_class_name[LANGUAGE_NONE][0]['value'] = 'Eat';
            $entity->field_event_type[LANGUAGE_NONE][0]['value'] = 'm';
            $entity->field_time[LANGUAGE_NONE][0] = array(
                'value' => $classT->format('Y/m/d H:i:s'),
                'value2' => date_add($classT, new DateInterval('PT1800S'))->format('Y/m/d H:i:s')
            );
            $key = 'new' . count($events);
            $events[$key] = $entity;
            $workingEvents[$key] = $entity;

            // remove overlap from newly placed event
            list($entity->field_time[LANGUAGE_NONE][0]['value'], $entity->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_remove_overlaps($node, $workingEvents, $key);

            studysauce_bucket_event($node, $events, $key);

            // lunch
            $classT = new DateTime($day, new DateTimeZone(date_default_timezone_get()));
            $classT->setTime(12,0,0);
            $classT->setTimezone(new DateTimeZone('UTC'));

            $entity = new stdClass();
            $entity->field_class_name[LANGUAGE_NONE][0]['value'] = 'Eat';
            $entity->field_event_type[LANGUAGE_NONE][0]['value'] = 'm';
            $entity->field_time[LANGUAGE_NONE][0] = array(
                'value' => $classT->format('Y/m/d H:i:s'),
                'value2' => date_add($classT, new DateInterval('PT1800S'))->format('Y/m/d H:i:s')
            );
            $key = 'new' . count($events);
            $events[$key] = $entity;
            $workingEvents[$key] = $entity;

            // remove overlap from newly placed event
            list($entity->field_time[LANGUAGE_NONE][0]['value'], $entity->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_remove_overlaps($node, $workingEvents, $key);

            studysauce_bucket_event($node, $events, $key);

            // dinner
            $classT = new DateTime($day, new DateTimeZone(date_default_timezone_get()));
            $classT->setTime(18,0,0);
            $classT->setTimezone(new DateTimeZone('UTC'));

            $entity = new stdClass();
            $entity->field_class_name[LANGUAGE_NONE][0]['value'] = 'Eat';
            $entity->field_event_type[LANGUAGE_NONE][0]['value'] = 'm';
            $entity->field_time[LANGUAGE_NONE][0] = array(
                'value' => $classT->format('Y/m/d H:i:s'),
                'value2' => date_add($classT, new DateInterval('PT1800S'))->format('Y/m/d H:i:s')
            );
            $key = 'new' . count($events);
            $events[$key] = $entity;
            $workingEvents[$key] = $entity;

            // remove overlap from newly placed event
            list($entity->field_time[LANGUAGE_NONE][0]['value'], $entity->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_remove_overlaps($node, $workingEvents, $key, $classT->getTimestamp());

            studysauce_bucket_event($node, $events, $key);
        }

    }
}

global $classTotals, $studyTotals;
$classTotals = array();
$studyTotals = array();
function studysauce_add_free_study_events($node, &$events, $saved)
{
    global $classTotals, $studyTotals, $buckets;

    // add free study to every
    foreach($classTotals as $w => $totalLength)
    {
        // get events this week plus next 24 hours since we only move forward
        $workingEvents = array();
        foreach($events as $e => $event)
            if(strtotime($event->field_time['und'][0]['value']) > $w - 86400 * 2 && strtotime($event->field_time['und'][0]['value2']) < $w + 604800 + 86400 * 2)
                $workingEvents[$e] = $event;

        $studyLength = isset($studyTotals[$w]) ? $studyTotals[$w] : 0;
        // use weekends setting to determine if the first free study should fall on a sunday
        $weekends = (isset($node->field_weekends['und'][0]['value']) && $node->field_weekends['und'][0]['value'] == 'hit_hard');

        // TODO: adjust study factor based on nothing but A's preference, only affect free time, can we do more?
        $studyFactor = 2;
        if(isset($node->field_grades['und'][0]['value']) && $node->field_grades['und'][0]['value'] == 'as_only')
            $studyFactor = 2.6;

        // there are 16 usable hours, 4 * (5 - 1) = 16 * 7 days = 112 usable hours in a week
        $shouldStudy = min($totalLength * $studyFactor, ($weekends ? 112 : 80) * 3600 - $totalLength); // a student should study 3 [2.4] times outside of class
        $remainingStudy = min(7 * 3600, // turns out filling up the entire week is too much
            max($shouldStudy - $studyLength, 3600 * 5)); // subtract the hours already accounted for by schedules study sessions
        $freeHours = floor($remainingStudy / 3600);
        for($j = 0; $j < $freeHours; $j++)
        {

            // get the buckets for each day this week and figure out which day have the least obligations
            $bucketSums = array();
            // if no weekends subtract 3 days, don't schedule free study on Fri-Sun, we know they won't study on Fridays
            for($i = ($weekends ? 0 : 1); $i < ($weekends ? 7 : 5); $i++)
                $bucketSums[date('Y/m/d', $w + $i * 86400)] = isset($buckets[date('Y/m/d', $w + $i * 86400)]) ? array_sum($buckets[date('Y/m/d', $w + $i * 86400)]) : 0;
            asort($bucketSums);

            // get the day with the least obligations
            $classT = new DateTime(key($bucketSums), new DateTimeZone(date_default_timezone_get()));
            $classT->setTime(12,0,0);
            $classT->setTimezone(new DateTimeZone('UTC'));


            // check if we have a saved event around the same time
            $lastEid = 0;
            $lastDistance = 86400 * 7;
            // TODO: use each so we save our location
            foreach($saved as $eid => $event)
            {
                // if saved is 24 hours before current event, skip and never go back because we are ordered by time
                $distance = abs(strtotime($event->field_time['und'][0]['value']) - $classT->getTimestamp());
                if($distance < $lastDistance)
                {
                    $lastEid = $eid;
                    $lastDistance = $distance;
                }
                elseif($lastEid > 0)
                    break;
            }

            if($lastEid)
            {
                $events[$lastEid] = $saved[$lastEid];
                unset($saved[$lastEid]);
                $events[$lastEid]->field_time[LANGUAGE_NONE][0] = array(
                    'value' => $classT->format('Y/m/d H:i:s'),
                    'value2' => date_add($classT, new DateInterval('PT3600S'))->format('Y/m/d H:i:s')
                );
                $workingEvents[$lastEid] = $events[$lastEid];

                // don't change times for events that have been moved
                if(!$events[$lastEid]->field_user_moved[LANGUAGE_NONE][0]['value'])
                    list($events[$lastEid]->field_time[LANGUAGE_NONE][0]['value'], $events[$lastEid]->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_get_preferred_time($node, $events, $lastEid);

                // move saved events if they overlap with a higher priority event, class, other, pre-work, study session
                list($events[$lastEid]->field_time[LANGUAGE_NONE][0]['value'], $events[$lastEid]->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_remove_overlaps($node, $workingEvents, $lastEid);

                studysauce_bucket_event($node, $events, $lastEid);
            }
            else
            {
                $entity = new stdClass();
                $entity->field_class_name[LANGUAGE_NONE][0]['value'] = 'Free study';
                $entity->field_event_type[LANGUAGE_NONE][0]['value'] = 'f';
                $entity->field_time[LANGUAGE_NONE][0] = array(
                    'value' => $classT->format('Y/m/d H:i:s'),
                    'value2' => date_add($classT, new DateInterval('PT3600S'))->format('Y/m/d H:i:s')
                );
                $key = 'new' . count($events);
                $events[$key] = $entity;
                $workingEvents[$key] = $entity;

                // find the right time for the event a day before
                list($entity->field_time[LANGUAGE_NONE][0]['value'], $entity->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_get_preferred_time($node, $events, $key);

                // remove overlap from newly placed event
                list($entity->field_time[LANGUAGE_NONE][0]['value'], $entity->field_time[LANGUAGE_NONE][0]['value2']) = studysauce_remove_overlaps($node, $workingEvents, $key);

                studysauce_bucket_event($node, $events, $key);
            }
        }
    }
}

function studysauce_add_deadlines_and_holidays($node, &$events, $saved)
{
    global $holidays;
    // add deadlines and holidays
    $deadlines = studysauce_get_deadlines();
    foreach($deadlines as $did => $d)
    {
        $classT = new DateTime($d->field_due_date['und'][0]['value'], new DateTimeZone(date_default_timezone_get()));
        $classT->setTime(0,0,0);
        $classT->setTimezone(new DateTimeZone('UTC'));

        // check if we have a saved event around the same time
        $lastEid = 0;
        $lastDistance = 172800;
        // TODO: use each so we save our location
        foreach($saved as $eid => $event)
        {
            // if saved is 24 hours before current event, skip and never go back because we are ordered by time
            if($event->field_event_type['und'][0]['value'] != 'd' ||
                $event->field_class_name['und'][0]['value'] != $d->field_class_name['und'][0]['value'])
                continue;

            $distance = abs(strtotime($event->field_time['und'][0]['value']) - $classT->getTimestamp());
            if($distance < $lastDistance)
            {
                $lastEid = $eid;
                $lastDistance = $distance;
            }
            elseif($lastEid > 0)
                break;
        }

        if($lastEid)
        {
            $events[$lastEid] = $saved[$lastEid];
            unset($saved[$lastEid]);
            $events[$lastEid]->field_time[LANGUAGE_NONE][0] = array(
                'value' => $classT->format('Y/m/d H:i:s'),
                'value2' => date_add($classT, new DateInterval('PT86399S'))->format('Y/m/d H:i:s')
            );
        }
        else
        {
            // create a new event
            $entity = new stdClass();
            $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $d->field_class_name['und'][0]['value'];
            $entity->field_event_type[LANGUAGE_NONE][0]['value'] = 'd';
            $entity->field_time[LANGUAGE_NONE][0] = array(
                'value' => $classT->format('Y/m/d H:i:s'),
                'value2' => date_add($classT, new DateInterval('PT86399S'))->format('Y/m/d H:i:s')
            );
            $key = 'new' . count($events);
            $events[$key] = $entity;
        }

        if(isset($d->field_reminder['und']) && is_array($d->field_reminder['und']))
            foreach($d->field_reminder['und'] as $i => $r)
            {
                $classT = new DateTime($d->field_due_date['und'][0]['value'], new DateTimeZone(date_default_timezone_get()));
                $classT->setTime(0,0,0);
                $classT->setTimezone(new DateTimeZone('UTC'));
                $classT->sub(new DateInterval('PT' . $r['value'] . 'S'));


                // check if we have a saved event around the same time
                $lastEid = 0;
                $lastDistance = 172800;
                // TODO: use each so we save our location
                foreach($saved as $eid => $event)
                {
                    // if saved is 24 hours before current event, skip and never go back because we are ordered by time
                    if($event->field_event_type['und'][0]['value'] != 'r' ||
                        $event->field_class_name['und'][0]['value'] != $d->field_class_name['und'][0]['value'])
                        continue;

                    $distance = abs(strtotime($event->field_time['und'][0]['value']) - $classT->getTimestamp());
                    if($distance < $lastDistance)
                    {
                        $lastEid = $eid;
                        $lastDistance = $distance;
                    }
                    elseif($lastEid > 0)
                        break;
                }

                if($lastEid)
                {
                    $events[$lastEid] = $saved[$lastEid];
                    unset($saved[$lastEid]);
                    $events[$lastEid]->field_time[LANGUAGE_NONE][0] = array(
                        'value' => $classT->format('Y/m/d H:i:s'),
                        'value2' => date_add($classT, new DateInterval('PT86399S'))->format('Y/m/d H:i:s')
                    );
                }
                else
                {
                    $entity = new stdClass();
                    $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $d->field_class_name['und'][0]['value'];
                    $entity->field_event_type[LANGUAGE_NONE][0]['value'] = 'r';
                    $entity->field_time[LANGUAGE_NONE][0] = array(
                        'value' => $classT->format('Y/m/d H:i:s'),
                        'value2' => date_add($classT, new DateInterval('PT86399S'))->format('Y/m/d H:i:s')
                    );
                    $key = 'new' . count($events);
                    $events[$key] = $entity;
                }
            }
    }

    foreach($holidays as $k => $h)
    {
        $classT = new DateTime($k, new DateTimeZone(date_default_timezone_get()));
        $classT->setTime(0,0,0);
        $classT->setTimezone(new DateTimeZone('UTC'));


        // check if we have a saved event around the same time
        $lastEid = 0;
        $lastDistance = 172800;
        // TODO: use each so we save our location
        foreach($saved as $eid => $event)
        {
            // if saved is 24 hours before current event, skip and never go back because we are ordered by time
            if($event->field_event_type['und'][0]['value'] != 'h' ||
                $event->field_class_name['und'][0]['value'] != $h)
                continue;

            $distance = abs(strtotime($event->field_time['und'][0]['value']) - $classT->getTimestamp());
            if($distance < $lastDistance)
            {
                $lastEid = $eid;
                $lastDistance = $distance;
            }
            elseif($lastEid > 0)
                break;
        }

        if($lastEid)
        {
            $events[$lastEid] = $saved[$lastEid];
            unset($saved[$lastEid]);
            $events[$lastEid]->field_time[LANGUAGE_NONE][0] = array(
                'value' => $classT->format('Y/m/d H:i:s'),
                'value2' => date_add($classT, new DateInterval('PT86399S'))->format('Y/m/d H:i:s')
            );
        }
        else
        {
            $entity = new stdClass();
            $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $h;
            $entity->field_event_type[LANGUAGE_NONE][0]['value'] = 'h';
            $entity->field_time[LANGUAGE_NONE][0] = array(
                'value' => $classT->format('Y/m/d H:i:s'),
                'value2' => date_add($classT, new DateInterval('PT86399S'))->format('Y/m/d H:i:s')
            );
            $key = 'new' . count($events);
            $events[$key] = $entity;
        }
    }

}

function studysauce_save_classes($node, &$events, $classes)
{
    $oldEntities = array_map(function ($x) {return $x->item_id; }, $events);
    $added = array();
    $renamed = array();

    foreach($classes as $j => $class)
    {
        // check if class entity already exists
        if(!isset($class['cid']) || !isset($events[$class['cid']]))
        {
            $entity = entity_create('field_collection_item', array('field_name' => 'field_classes'));
            $entity->setHostEntity('node', $node);
        }
        else
        {
            $entity = $events[$class['cid']];
            // figure out what changed
            if(isset($entity->field_class_name[LANGUAGE_NONE][0]['value']) &&
                $entity->field_class_name[LANGUAGE_NONE][0]['value'] != $class['className'])
            {
                // remove old reoccurring events
                $renamed[$class['cid']] = $entity->field_class_name[LANGUAGE_NONE][0]['value'];
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

        if(!isset($events[$class['cid']]))
        {
            // we don't need manage removal of new entries
            $added['new' . count($added)] = $entity;
        }
    }

    $entities = array_merge(array_values($events), array_values($added));
    $node->field_classes['und'] = array_map(function ($x) {return array('entity' => $x, 'value' => $x->item_id);}, $entities);
    $removeEntities = array_intersect($oldEntities, array_diff($oldEntities, array_map(function ($x) {return $x['value'];}, $node->field_classes['und'])));
    entity_delete_multiple('field_collection_item', $removeEntities);
    node_save($node);

    return array($added, $renamed);
}

function studysauce_rebuild_schedule($node, $entities, $added = array(), $renamed = array())
{
    $events = array();
    $oldEvents = array();

    // get a list of current events to alter or use in rebuild
    $results = db_select('studysauce_events', 'se')
        ->fields('se', array('eid', 'title', 'nid', 'time_start', 'time_end', 'event_type', 'completed', 'moved', 'deleted'))
        ->condition('se.nid', $node->nid, '=')
        ->execute()->fetchAllAssoc('eid');
    foreach($results as $eid => $r)
    {
        $entity = new stdClass();
        $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $r->title;
        $entity->field_event_type[LANGUAGE_NONE][0]['value'] = $r->event_type;
        $entity->field_time[LANGUAGE_NONE][0] = array(
            'value' => $r->time_start,
            'value2' => $r->time_end
        );
        $entity->field_user_moved[LANGUAGE_NONE][0]['value'] = $r->moved;
        $entity->field_deleted[LANGUAGE_NONE][0]['value'] = $r->deleted;
        $entity->item_id = $eid;
        $oldEvents[$eid] = $entity;
    }


    // TODO: make this more efficient, don't remove class items that haven't changed
    $classes = array();
    $others = array();
    foreach($entities as $cid => $event)
    {
        if(isset($event->field_class_name['und']) && !empty($event->field_class_name['und']) &&
            isset($event->field_day_of_the_week['und']) && is_array($event->field_day_of_the_week['und']) && !empty($event->field_day_of_the_week['und'][0]['value']))
        {
            if((!isset($event->field_event_type['und'][0]['value']) || $event->field_event_type['und'][0]['value'] == 'c'))
                $classes[$cid] = $event;
            elseif($event->field_event_type['und'][0]['value'] == 'o')
                $others[$cid] = $event;
        }

    }

    $saved = studysauce_remove_old_events($node, $oldEvents, $renamed);

    // we don't need manage removal of new entries, so add them after removing is done
    foreach($added as $cid => $event)
    {
        if(isset($event->field_class_name['und']) && !empty($event->field_class_name['und']) &&
            isset($event->field_day_of_the_week['und']) && is_array($event->field_day_of_the_week['und']) && !empty($event->field_day_of_the_week['und'][0]['value']))
        {
            if((!isset($event->field_event_type['und'][0]['value']) || $event->field_event_type['und'][0]['value'] == 'c'))
                $classes[$cid] = $event;
            elseif($event->field_event_type['und'][0]['value'] == 'o')
                $others[$cid] = $event;
        }
    }

    // filter this type and find nearest from saved list
    $workingDeadlines = array();
    foreach($saved as $eid => $event)
        if($event->field_event_type['und'][0]['value'] == 'd' || $event->field_event_type['und'][0]['value'] == 'h' ||
            $event->field_event_type['und'][0]['value'] == 'r')
            $workingDeadlines[$eid] = $event;
    studysauce_add_deadlines_and_holidays($node, $events, $workingDeadlines);

    // rebuild entire schedule
    foreach($classes as $cid => $class)
    {
        $workingEvents = array();
        foreach($saved as $eid => $event)
            if($event->field_event_type['und'][0]['value'] == 'c' &&
                $event->field_class_name['und'][0]['value'] == $class->field_class_name['und'][0]['value'])
                $workingEvents[$eid] = $event;
        studysauce_add_reoccurring_events($node, $events, $class, $workingEvents);
    }
    foreach($others as $cid => $class)
    {
        $workingEvents = array();
        foreach($saved as $eid => $event)
            if($event->field_event_type['und'][0]['value'] == 'o' &&
                $event->field_class_name['und'][0]['value'] == $class->field_class_name['und'][0]['value'])
                $workingEvents[$eid] = $event;
        studysauce_add_reoccurring_events($node, $events, $class, $workingEvents);
    }

    // add meals to be removed later
    studysauce_add_meals_events($node, $events);


    foreach($classes as $cid => $class)
    {
        // set up pre-work
        $workingEvents = array();
        foreach($saved as $eid => $event)
            if($event->field_event_type['und'][0]['value'] == 'p' &&
                $event->field_class_name['und'][0]['value'] == $class->field_class_name['und'][0]['value'])
                $workingEvents[$eid] = $event;
        studysauce_add_prework_events($node, $events, $class, $workingEvents);
    }

    foreach($classes as $cid => $class)
    {
        // set up sr-sessions
        $workingEvents = array();
        foreach($saved as $eid => $event)
            if($event->field_event_type['und'][0]['value'] == 'sr' &&
                $event->field_class_name['und'][0]['value'] == $class->field_class_name['und'][0]['value'])
                $workingEvents[$eid] = $event;
        studysauce_add_study_events($node, $events, $class, $workingEvents);
    }

    // if there are any changes render the free study
    $workingFree = array();
    foreach($saved as $eid => $event)
        if($event->field_event_type['und'][0]['value'] == 'f')
            $workingFree[$eid] = $event;
    studysauce_add_free_study_events($node, $events, $workingFree);

    // don't remove unused saved items because they have data attached, instead mark saved as deleted and hide in historical view
    $deletedIds = array_intersect(array_keys($saved), array_diff(array_keys($saved), array_keys($events)));
    foreach($saved as $i => $s)
        $events[$i] = $s;
    if(count($deletedIds))
    {
        db_update('studysauce_events')
            ->fields(array('deleted' => 1))
            ->condition ('eid', $deletedIds, 'IN')
            ->execute();
    }

    // remove the changes events and save all
    $removeEvents = array_intersect(array_keys($oldEvents), array_diff(array_keys($oldEvents), array_keys($events)));
    if(count($removeEvents))
        db_delete('studysauce_events')->condition('eid', $removeEvents, 'IN')->execute();
    $query = db_insert('studysauce_events')->fields(array('title', 'nid', 'time_start', 'time_end', 'event_type', 'completed'));
    $update = db_update('studysauce_events');
    $insertIds = array();
    $updateIds = array();
    $whenStart = '';
    $whenEnd = '';
    foreach ($events as $eid => $event) {
        // skip meals events
        if($event->field_event_type['und'][0]['value'] == 'm')
            continue;

        if(substr($eid, 0, 3) != 'new')
        {
            if(in_array($eid, $deletedIds))
                continue;
            // update event
            $updateIds[] = $eid;
            $whenStart .= ' WHEN eid=' . $eid . ' THEN \'' . $event->field_time['und'][0]['value'] . '\'';
            $whenEnd .= ' WHEN eid=' . $eid . ' THEN \'' . $event->field_time['und'][0]['value2'] . '\'';
            continue;
        }

        $insertIds[] = $eid;
        $query->values(array(
            'title' => $event->field_class_name['und'][0]['value'],
            'nid' => $node->nid,
            'time_start' => $event->field_time['und'][0]['value'],
            'time_end' => $event->field_time['und'][0]['value2'],
            'event_type' => $event->field_event_type['und'][0]['value'],
            'completed' => 0
        ));
    }
    if(count($updateIds))
    {
        $update->condition('eid', $updateIds, 'IN')
            ->expression('time_start', 'CASE' . $whenStart . ' END')
            ->expression('time_end', 'CASE' . $whenEnd . ' END')
            ->expression('deleted', 0)
            ->execute();
    }
    if(count($insertIds))
        $query->execute();
}

function studysauce_move_calendar()
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
        $node->revision = 0;
        if(isset($_POST['cid']) && isset($_POST['type']) &&
            isset($_POST['start']) && isset($_POST['end']))
            $_POST['classes'][] = array(
                'cid' => $_POST['cid'],
                'type' => $_POST['type'],
                'start' => $_POST['start'],
                'end' => $_POST['end']
            );
        if(count($_POST['classes']) > 0)
        {

            foreach($_POST['classes'] as $i => $class)
            {
                $event = db_select('studysauce_events', 'se')
                    ->fields('se', array('eid', 'title', 'nid', 'time_start', 'time_end', 'event_type', 'completed'))
                    ->condition('se.eid', $class['cid'], '=')
                    ->condition('se.nid', $node->nid, '=')
                    ->execute()->fetchAssoc();

                if($event)
                {
                    $newStart = strtotime(gmdate('Y-m-d H:i:s', strtotime($class['start'])));

                    db_update('studysauce_events')
                        ->fields(array(
                            'time_start' => gmdate('Y-m-d H:i:s', strtotime($class['start'])),
                            'time_end' => gmdate('Y-m-d H:i:s', strtotime($class['end'])),
                            'moved' => true))
                        ->condition('eid', $class['cid'], '=')
                        ->condition('nid', $node->nid, '=')
                        ->execute();

                    $diff = $newStart - strtotime($event['time_start']);

                    // move subsequent events of the same type
                    $events = db_select('studysauce_events', 'se')
                        ->fields('se', array('eid', 'title', 'nid', 'time_start', 'time_end', 'event_type', 'completed', 'moved', 'deleted'))
                        ->condition('se.event_type', $event['event_type'], '=')
                        ->condition('se.title', $event['title'], '=')
                        ->condition('se.time_start', date('Y-m-d H:i:s', $newStart), '>')
                        ->condition('se.deleted', 0, '=')
                        ->condition('se.nid', $node->nid, '=')
                        ->execute()->fetchAllAssoc('eid');

                    foreach($events as $cid => $e)
                    {
                        // if more than a week has passed add a week to $newStart and test again
                        if(strtotime($e->time_start) - $newStart > 60 * 60 * 24 * 8)
                            $newStart += 60 * 60 * 24 * 7;

                        // make sure next event is about a week later
                        if(strtotime($e->time_start) - $newStart > 60 * 60 * 24 * 6 &&
                            date('H:i:s', strtotime($e->time_start) + $diff) == date('H:i:s', $newStart))
                        {
                            // get events for the day to make sure we are not overlapping
                            $possibleOverlaps = db_select('studysauce_events', 'se')
                                ->fields('se', array('eid', 'title', 'nid', 'time_start', 'time_end', 'event_type', 'completed', 'moved', 'deleted'))
                                ->condition('se.time_start', date('Y-m-d H:i:s', strtotime($e->time_start) - 86400), '>')
                                ->condition('se.time_start', date('Y-m-d H:i:s', strtotime($e->time_start) + 86400), '<')
                                ->condition('se.deleted', 0, '=')
                                ->condition('se.nid', $node->nid, '=')
                                ->execute()->fetchAllAssoc('eid');

                            $workingEvents = array();
                            foreach($possibleOverlaps as $eid => $r)
                            {
                                $entity = new stdClass();
                                $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $r->title;
                                $entity->field_event_type[LANGUAGE_NONE][0]['value'] = $r->event_type;
                                $entity->field_time[LANGUAGE_NONE][0] = array(
                                    'value' => $r->time_start,
                                    'value2' => $r->time_end
                                );
                                $entity->field_user_moved[LANGUAGE_NONE][0]['value'] = $r->moved;
                                $entity->field_deleted[LANGUAGE_NONE][0]['value'] = $r->deleted;
                                $entity->item_id = $eid;
                                $workingEvents[$eid] = $entity;
                            }

                            $entity = new stdClass();
                            $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $e->title;
                            $entity->field_event_type[LANGUAGE_NONE][0]['value'] = $e->event_type;
                            $entity->field_time[LANGUAGE_NONE][0] = array(
                                'value' => date('Y-m-d H:i:s', strtotime($e->time_start) + $diff),
                                'value2' => date('Y-m-d H:i:s', strtotime($e->time_end) + $diff)
                            );
                            $entity->field_user_moved[LANGUAGE_NONE][0]['value'] = $e->moved;
                            $entity->field_deleted[LANGUAGE_NONE][0]['value'] = $e->deleted;
                            $entity->item_id = $cid;
                            $workingEvents[$cid] = $entity;

                            $newTime = studysauce_remove_overlaps($node, $workingEvents, $cid);
                            // save event change because it didn't move
                            if($newTime[0] == date('Y-m-d H:i:s', strtotime($e->time_start) + $diff))
                            {
                                db_update('studysauce_events')
                                    ->fields(array(
                                        'time_start' => $newTime[0],
                                        'time_end' => $newTime[1],
                                        'moved' => true))
                                    ->condition('eid', $cid, '=')
                                    ->condition('nid', $node->nid, '=')
                                    ->execute();
                            }
                        }
                    }

                }

            }
        }
    }

    global $studysauceExportClasses, $studysauceExportEvents;
    print json_encode(array(
        'plan' => '<div>' . theme('studysauce-plans') . '</div>',
        'events' => $studysauceExportEvents,
    ));
    drupal_exit();
}

function studysauce_save_calendar()
{
    global $user;

    // skip building only if told to do so
    $skipBuild = (isset($_POST['skipBuild']) && $_POST['skipBuild'] == 'true');

    // check if the use is paid
    $lastOrder = _studysauce_orders_by_uid($user->uid);
    $groups = og_get_groups_by_user();
    $isPaid = isset($groups['node']) || $lastOrder;


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
        $node->revision = 0;
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
        $entities = array();
        if(isset($node->field_classes[LANGUAGE_NONE]) && is_array($node->field_classes[LANGUAGE_NONE]))
            $entities = entity_load('field_collection_item', array_map(function ($x) { return $x['value']; }, $node->field_classes[LANGUAGE_NONE]));
        list($added, $renamed) = studysauce_save_classes($node, $entities, $_POST['classes']);

        if($isPaid && !$skipBuild)
            studysauce_rebuild_schedule($node, $entities, $added, $renamed);
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
        $events = array();
        foreach($node->field_classes['und'] as $i => $c)
        {
            if($c['value'] != $_POST['remove'])
            {
                $events[] = $c;
            }
            else
                entity_delete('field_collection_item', $_POST['remove']);
        }

        $node->field_classes['und'] = $events;
        node_save($node);

        $entities = array();
        if(isset($node->field_classes[LANGUAGE_NONE]) && is_array($node->field_classes[LANGUAGE_NONE]))
            $entities = entity_load('field_collection_item', array_map(function ($x) { return $x['value']; }, $node->field_classes[LANGUAGE_NONE]));
        if($isPaid && !$skipBuild)
            studysauce_rebuild_schedule($node, $entities);
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

        $events = array();
        $classes = array();
        $others = array();
        if (!isset($node->field_classes[LANGUAGE_NONE]) || !is_array($node->field_classes[LANGUAGE_NONE]))
            return array(array(), array(), array());

        $entities = entity_load('field_collection_item', array_map(function ($x) { return $x['value']; }, $node->field_classes[LANGUAGE_NONE]));

        // TODO: return classes, others, and events to avoid confusion
        foreach ($entities as $eid => $entity)
        {
            if (isset($entity->field_class_name['und'][0]['value']) && !empty($entity->field_class_name['und'][0]['value']) &&
                isset($entity->field_time['und'][0]['value']) && isset($entity->field_time['und'][0]['value2']))
            {
                // this is use by schedule only, if everything is filled in display the event
                if((!isset($entity->field_event_type['und'][0]['value']) || $entity->field_event_type['und'][0]['value'] == 'c'))
                    $classes[$eid] = $entity;
                elseif($entity->field_event_type['und'][0]['value'] == 'o')
                    $others[$eid] = $entity;
            }
        }

        $results = db_select('studysauce_events', 'se')
            ->fields('se', array('eid', 'title', 'nid', 'time_start', 'time_end', 'event_type', 'completed', 'moved', 'deleted'))
            ->condition('se.nid', $node->nid, '=')
            ->execute()->fetchAllAssoc('eid');
        foreach($results as $eid => $r)
        {
            $entity = new stdClass();
            $entity->field_class_name[LANGUAGE_NONE][0]['value'] = $r->title;
            $entity->field_event_type[LANGUAGE_NONE][0]['value'] = $r->event_type;
            $entity->field_time[LANGUAGE_NONE][0] = array(
                'value' => $r->time_start,
                'value2' => $r->time_end
            );
            $entity->field_user_moved[LANGUAGE_NONE][0]['value'] = $r->moved;
            $entity->field_deleted[LANGUAGE_NONE][0]['value'] = $r->deleted;
            $events[$eid] = $entity;
        }

        uasort($events, function ($a, $b) {
            return strtotime($a->field_time['und'][0]['value']) - strtotime($b->field_time['und'][0]['value']);
        });

        return array($events, $classes, $others);
    }

    return array(array(), array(), array());
}


function _studysauce_get_schedule_classes($account = null)
{
    global $exportClasses;
    if($exportClasses != null)
        return $exportClasses;
    if($account == null)
    {
        global $user;
        $account = user_load($user->uid);
    }
    $classes = array();
    $query = new EntityFieldQuery();
    $entities = $query->entityCondition('entity_type', 'node')
        ->propertyCondition('type', 'schedule')
        ->propertyCondition('title', isset($account->mail) ? $account->mail : '')
        ->propertyCondition('status', 1)
        ->range(0,1)
        ->execute();
    if (!empty($entities['node']))
    {
        $nodes = array_keys($entities['node']);
        $nid = array_shift($nodes);
        $node = node_load($nid);
        if(isset($node->field_classes[LANGUAGE_NONE]) && is_array($node->field_classes[LANGUAGE_NONE]))
        {
            $events = entity_load('field_collection_item', array_map(function ($x) { return $x['value']; }, $node->field_classes[LANGUAGE_NONE]));

            foreach($events as $eid => $entity)
            {
                if(isset($entity->field_class_name['und'][0]['value']) && !empty($entity->field_class_name['und'][0]['value']) &&
                    (!isset($entity->field_event_type['und'][0]['value']) || $entity->field_event_type['und'][0]['value'] == 'c') &&
                    isset($entity->field_day_of_the_week['und']) && is_array($entity->field_day_of_the_week['und']) &&
                    !empty($entity->field_day_of_the_week['und'][0]['value']))
                    $classes[$eid] = $entity->field_class_name['und'][0]['value'];
            }
        }
    }
    $exportClasses = $classes;
    return $classes;
}

