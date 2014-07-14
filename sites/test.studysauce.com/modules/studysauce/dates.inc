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

function studysauce_save_dates()
{
    global $user;

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
        $node->revision = 1;
    }
    else
    {
        $node = new stdClass();
        $node->title = isset($user->mail) ? $user->mail : '';
        $node->type = "key_dates";
        $node->language = LANGUAGE_NONE; // Or e.g. 'en' if locale is enabled
        $node->uid = $user->uid;
        $node->status = 1; //(1 or 0): published or not
        $node->revision = 1;
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
        foreach($_POST['dates'] as $j => $date)
        {
            // check if class entity already exists by name
            $dateChanged = false;
            if(isset($date['eid']))
                foreach($node->field_reminders['und'] as $i => $c)
                {
                    if($c['value'] == $date['eid'])
                    {
                        $entity = entity_load('field_collection_item', array($c['value']));
                        $entity = $entity[$c['value']];
                        if(isset($entity->field_due_date['und'][0]['value']) && $entity->field_due_date['und'][0]['value'] != gmdate('Y-m-d H:i:s', strtotime($date['due'])))
                            $dateChanged = true;
                        $entity->revision = 1;
                        break;
                    }
                    unset($entity);
                }
            if(!isset($entity))
            {
                $entity = entity_create('field_collection_item', array('field_name' => 'field_reminders'));
                $entity->setHostEntity('node', $node);
            }

            $entity->field_class_name['und'][0]['value'] = $date['className'];
            $entity->field_assignment['und'][0]['value'] = $date['assignment'];
            $entity->field_reminder['und'] = array_map(function ($x) { return array('value' => $x); }, explode(',', $date['reminders']));
            $entity->field_due_date['und'][0] = array(
                'value' => gmdate('Y-m-d H:i:s', strtotime($date['due'])));
            $entity->field_percent['und'][0]['value'] = $date['percent'];

            // reset the sent reminders if the date changes
            if($dateChanged)
                $entity->field_reminder_sent['und'] = array();

            $entity->save();
        }
    }

    if(isset($_POST['remove']))
    {
        foreach($node->field_reminders['und'] as $i => $c)
        {
            if($c['value'] == $_POST['remove'])
            {
                entity_delete('field_collection_item', array($c['value']));
            }
        }
    }

    node_save($node);

    global $studysauceExportEvents;
    print json_encode(array(
        'reminders' => '<div>' . theme('studysauce-deadlines') . '</div>',
        'plan' => '<div>' . theme('studysauce-plans') . '</div>',
        'events' => $studysauceExportEvents
    ));
    drupal_exit();
}



