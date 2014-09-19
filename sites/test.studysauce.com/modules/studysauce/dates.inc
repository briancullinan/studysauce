<?php

function studysauce_any_dates()
{
    global $user;
    $query = new EntityFieldQuery();
    $entities = $query->entityCondition('entity_type', 'node')
        ->propertyCondition('type', 'key_dates')
        ->propertyCondition('title', $user->mail)
        ->propertyCondition('status', 1)
        ->range(0, 1)
        ->execute();

    $rebuild = false;
    if (!empty($entities['node'])) {
        $nodes = array_keys($entities['node']);
        $nid = array_shift($nodes);
        $node = node_load($nid);

        if (isset($node->field_reminders[LANGUAGE_NONE][0]['value']))
        {
            $headStr = '';
            $first = true;
            foreach ($node->field_reminders[LANGUAGE_NONE] as $i => $reminder)
            {
                $eid = $reminder['value'];
                $entity = entity_load('field_collection_item', array($eid));
                if (!empty($entity))
                {
                    if(isset($entity[$eid]->field_due_date['und'][0]['value']))
                        return true;
                }
            }
        }
    }
    return false;
}

function studysauce_rebuild_deadlines()
{
    global $user;
    $oldEvents = array();

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
        return;

    // get a list of current events to alter or use in rebuild
    $results = db_select('studysauce_events', 'se')
        ->fields('se', array('eid', 'title', 'nid', 'time_start', 'time_end', 'event_type', 'completed', 'moved', 'deleted'))
        ->condition('se.nid', $node->nid, '=')
        ->condition('event_type', array('d', 'r', 'h'), 'IN')
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


    $saved = studysauce_remove_old_events($node, $oldEvents, array());


    $workingDeadlines = array();
    foreach($saved as $eid => $event)
        if($event->field_event_type['und'][0]['value'] == 'd' || $event->field_event_type['und'][0]['value'] == 'h' ||
            $event->field_event_type['und'][0]['value'] == 'r')
            $workingDeadlines[$eid] = $event;
    studysauce_add_deadlines_and_holidays($node, $events, $workingDeadlines);



    // don't remove unused saved items because they have data attached, instead mark saved as deleted and hide in historical view
    $deletedIds = array_intersect(array_keys($saved), array_diff(array_keys($saved), array_keys($events)));
    foreach($saved as $i => $s)
        $events[$i] = $s;
    if(count($deletedIds))
    {
        db_update('studysauce_events')
            ->fields(array('deleted' => true))
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
            ->expression('deleted', '0')
            ->execute();
    }
    if(count($insertIds))
        $query->execute();
}

function studysauce_save_dates()
{
    global $user;

    // check if the user is paid
    $lastOrder = _studysauce_orders_by_uid($user->uid);
    $groups = og_get_groups_by_user();
    $isPaid = isset($groups['node']) || $lastOrder;

    // load dates node
    $query = new EntityFieldQuery();
    $nodes = $query->entityCondition('entity_type', 'node')
        ->propertyCondition('type', 'key_dates')
        ->propertyCondition('title', isset($user->mail) ? $user->mail : '')
        ->propertyCondition('status', 1)
        ->range(0, 1)
        ->execute();
    if (!empty($nodes['node']))
    {
        $nodes = array_keys($nodes['node']);
        $nid = array_shift($nodes);
        $node = node_load($nid);
    }
    else
    {
        $node = new stdClass();
        $node->title = isset($user->mail) ? $user->mail : '';
        $node->type = "key_dates";
        $node->language = LANGUAGE_NONE; // Or e.g. 'en' if locale is enabled
        $node->uid = $user->uid;
        $node->status = 1; //(1 or 0): published or not
        node_object_prepare($node); // Sets some defaults. Invokes hook_prepare() and hook_node_prepare().
        $node = node_submit($node); // Prepare node for saving
        node_save($node);
    }

    // save class
    if(!isset($_POST['dates']) || !is_array($_POST['dates']))
        $_POST['dates'] = array();

    if(isset($_POST['className']) && isset($_POST['assignment']) && isset($_POST['reminders']) &&
        isset($_POST['due']) && isset($_POST['percent']))
        $_POST['dates'][] = array(
            'eid' => isset($_POST['eid']) ? $_POST['eid'] : null,
            'className' => $_POST['className'],
            'assignment' => $_POST['assignment'],
            'reminders' => $_POST['reminders'],
            'due' => $_POST['due'],
            'percent' => $_POST['percent']
        );

    if(count($_POST['dates']) > 0)
    {
        if(isset($node->field_reminders['und']) && !empty($node->field_reminders['und']))
            $reminders = entity_load('field_collection_item', array_map(function ($x) {return $x['value']; }, $node->field_reminders['und']));
        else
            $reminders = array();

        foreach($_POST['dates'] as $j => $date)
        {
            // check if class entity already exists by name
            $dateChanged = false;
            if(isset($date['eid']) && isset($reminders[$date['eid']]))
            {
                $entity = $reminders[$date['eid']];
                if(isset($entity->field_due_date['und'][0]['value']) && $entity->field_due_date['und'][0]['value'] != gmdate('Y-m-d H:i:s', strtotime($date['due'])))
                    $dateChanged = true;
            }
            else
            {
                $entity = entity_create('field_collection_item', array('field_name' => 'field_reminders'));
                $entity->setHostEntity('node', $node);
            }

            $entity->field_class_name['und'][0]['value'] = $date['className'];
            $entity->field_assignment['und'][0]['value'] = $date['assignment'];
            $entity->field_reminder['und'] = array_map(function ($x) { return array('value' => $x); }, explode(',', $date['reminders']));
            $entity->field_due_date['und'][0] = array(
                'value' => gmdate('Y-m-d H:i:s', strtotime($date['due'])));
            $entity->field_percent['und'][0]['value'] = intval($date['percent']);

            // reset the sent reminders if the date changes
            if($dateChanged)
                $entity->field_reminder_sent['und'] = array();

            if(!isset($date['eid']) || !isset($reminders[$date['eid']]))
            {
                $reminders['new' . count($reminders)] = $entity;
            }
        }

        $node->field_reminders['und'] = array_map(function ($x) {return array('value' => $x->item_id, 'entity' => $x);}, array_values($reminders));
    }

    if(isset($_POST['remove']))
    {
        foreach($node->field_reminders['und'] as $i => $c)
        {
            if($c['value'] == $_POST['remove'])
            {
                entity_delete('field_collection_item', intval($c['value']));
                unset($node->field_reminders['und'][$i]);
                break;
            }
        }
        $node->field_reminders['und'] = array_values($node->field_reminders['und']);
    }

    node_save($node);

    // only create deadline events if the user has paid
    if($isPaid)
        studysauce_rebuild_deadlines($node);

    global $studysauceExportEvents;
    print json_encode(array(
        'reminders' => '<div>' . theme('studysauce-deadlines') . '</div>',
        'plan' => '<div>' . theme('studysauce-plans') . '</div>',
        'events' => $studysauceExportEvents
    ));
    drupal_exit();
}

function studysauce_get_deadlines($account = null)
{
    if(!isset($account))
    {
        global $user;
        $account = user_load($user->uid);
    }

    $query = new EntityFieldQuery();
    $entities = $query->entityCondition('entity_type', 'node')
        ->propertyCondition('type', 'key_dates')
        ->propertyCondition('title', $account->mail)
        ->propertyCondition('status', 1)
        ->range(0, 1)
        ->execute();

    if (!empty($entities['node'])) {
        $nodes = array_keys($entities['node']);
        $nid = array_shift($nodes);
        $node = node_load($nid);

        if (isset($node->field_reminders[LANGUAGE_NONE][0]['value']))
        {
            $entities = entity_load('field_collection_item', array_map(function ($x) { return $x['value']; }, $node->field_reminders[LANGUAGE_NONE]));

            $result = array();
            foreach($entities as $eid => $entity)
            {
                if(isset($entity->field_due_date['und'][0]['value']))
                    $result[$eid] = $entity;
            }

            uasort($result, function ($a, $b) {
                return strtotime($a->field_due_date['und'][0]['value']) - strtotime($b->field_due_date['und'][0]['value']);
            });

            return $result;
        }
    }

    return array();
}


