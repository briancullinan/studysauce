<?php


function studysauce_checkin()
{
    module_load_include('inc', 'node', 'node.pages');
    global $user;
    $query = new EntityFieldQuery();
    $entities = $query->entityCondition('entity_type', 'node')
        ->propertyCondition('type', 'schedule')
        ->propertyCondition('title', $user->mail)
        ->propertyCondition('status', 1)
        ->range(0,1)
        ->execute();

    if (!empty($entities['node']))
    {
        $nodes = array_keys($entities['node']);
        $nid = array_shift($nodes);
        $node = node_load($nid);
        $node->revision = 1;
        if(isset($node->field_classes['und'][0]['value']))
        {
            foreach($node->field_classes['und'] as $i => $class)
            {
                if($class['value'] == $_POST['eid'])
                {
                    $entity = entity_load('field_collection_item', array($class['value']));
                    $entity = $entity[$class['value']];
                    $entity->revision = 1;

                    if(isset($_POST['checkedIn']) && $_POST['checkedIn'] == 'true')
                    {
                        end($entity->field_checkin['und']);
                        $last = key($entity->field_checkin['und']);
                        $entity->field_checkin['und'][$last]['value2'] = date('Y-m-d H:i:s', $_POST['date']);

                        end($entity->field_utc_checkin['und']);
                        $last = key($entity->field_utc_checkin['und']);
                        $entity->field_utc_checkin['und'][$last]['value2'] = date('Y-m-d H:i:s', time());
                        $entity->save();

                        $awards = studysauce_get_awards();
                    }
                    else
                    {
                        $entity->field_checklist['und'] = array();
                        foreach(explode(',', $_POST['checklist']) as $check)
                            $entity->field_checklist['und'][] = array('value' => $check);
                        $entity->field_checkin['und'][] = array(
                            'value' => date('Y-m-d H:i:s', $_POST['date'])
                        );
                        $entity->field_utc_checkin['und'][] = array(
                            'value' => date('Y-m-d H:i:s', time())
                        );
                        $entity->field_locations['und'][0]['value'] = (isset($entity->field_locations['und'][0]['value']) ? ($entity->field_locations['und'][0]['value'] . "\r\n") : '') . $_POST['location'] . ',' . time();
                        $entity->save();
                    }

                    break;
                }
            }
        }
    }

    $newSDS = studysauce_get_sds_messages();
    $lastSDS = end($newSDS);

    global $classes;
    $classes = _studysauce_get_schedule_classes();
    list($times, $rows, $total, $hours) = _studysauce_get_metrics();

    print json_encode(array(
        'lastSDS' => $lastSDS,
        'rows' => $rows,
        'times' => $times,
        'total' => $total,
        'hours' => $hours,
        'awards' => $awards,
        'eid' => $_POST['eid']
    ));
    drupal_exit();
}


