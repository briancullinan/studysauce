<?php

function studysauce_save_profile()
{
    global $user;

    $query = new EntityFieldQuery();
    $nodes = $query->entityCondition('entity_type', 'node')
        ->propertyCondition('type', 'profile')
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
        $node->type = 'profile';
        $node->language = LANGUAGE_NONE; // Or e.g. 'en' if locale is enabled
        $node->uid = $user->uid;
        $node->status = 1; //(1 or 0): published or not
        $node->revision = 1;
        node_object_prepare($node); // Sets some defaults. Invokes hook_prepare() and hook_node_prepare().
        $node = node_submit($node); // Prepare node for saving
        node_save($node);
    }

    if(isset($_POST['devices']))
        $node->field_devices['und'][0]['value'] = $_POST['devices'];
    if(isset($_POST['education']))
        $node->field_education['und'][0]['value'] = $_POST['education'];
    if(isset($_POST['mindset']))
        $node->field_mindset['und'][0]['value'] = $_POST['mindset'];
    if(isset($_POST['study-much']))
        $node->field_study_much['und'][0]['value'] = $_POST['study-much'];
    if(isset($_POST['time-management']))
        $node->field_time_management['und'][0]['value'] = $_POST['time-management'];

    node_save($node);

    list($events, $node, $classes, $entities) = studysauce_get_events();

    print json_encode(array(
        'events' => $events,
        'plan' => '<div>' . theme('studysauce-plans') . '</div>'
    ));
    drupal_exit();
}
