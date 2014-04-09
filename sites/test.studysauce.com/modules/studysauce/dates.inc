<?php


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

    if(isset($_POST['className']) && isset($_POST['assignment']) && isset($_POST['reminders']) &&
        isset($_POST['due']) && isset($_POST['percent']))
    {
        // check if class entity already exists by name
        if(isset($_POST['eid']))
            foreach($node->field_reminders['und'] as $i => $c)
            {
                if($c['value'] == $_POST['eid'])
                {
                    $entity = entity_load('field_collection_item', array($c['value']));
                    $entity = $entity[$c['value']];
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

        $entity->field_class_name['und'][0]['value'] = $_POST['className'];
        $entity->field_assignment['und'][0]['value'] = $_POST['assignment'];
        $entity->field_reminder['und'] = array_map(function ($x) { return array('value' => $x); }, explode(',', $_POST['reminders']));
        $entity->field_due_date['und'][0] = array(
            'value' => gmdate('Y-m-d H:i:s', strtotime($_POST['due'])));
        $entity->field_percent['und'][0]['value'] = $_POST['percent'];

        $entity->save();
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

    $node = node_submit($node); // Prepare node for saving
    node_save($node);

    print json_encode(array('reminders' => '<div>' . theme('studysauce-reminders') . '</div>'));
    drupal_exit();
}



