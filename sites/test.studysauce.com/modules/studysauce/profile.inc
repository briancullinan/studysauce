<?php

function studysauce_save_profile()
{
    global $user;

    // skip building only if told to do so
    $skipBuild = isset($_POST['skipBuild']) && $_POST['skipBuild'] == 'true';

    // check if the user is paid
    $lastOrder = _studysauce_orders_by_uid($user->uid);
    $groups = og_get_groups_by_user();
    $isPaid = isset($groups['node']) || $lastOrder;

    // load the profile node
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

    // load the schedule node
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

        $entities = array();
        if(isset($node->field_classes[LANGUAGE_NONE]) && is_array($node->field_classes[LANGUAGE_NONE]))
            $entities = entity_load('field_collection_item', array_map(function ($x) { return $x['value']; }, $node->field_classes[LANGUAGE_NONE]));
        if($isPaid && !$skipBuild)
            studysauce_rebuild_schedule($node, $entities);
    }

    global $studysauceExportEvents;
    print json_encode(array(
        'plan' => '<div>' . theme('studysauce-plans') . '</div>',
        'events' => $studysauceExportEvents,
        'classes' => _studysauce_get_schedule_classes($user)
    ));
    drupal_exit();
}
