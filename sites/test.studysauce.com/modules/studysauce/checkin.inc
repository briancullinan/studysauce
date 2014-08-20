<?php


function studysauce_checkin()
{
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

        if(isset($_POST['touchedMusic']))
            $node->field_touched_music['und'][0]['value'] = 1;

        if(isset($node->field_classes['und'][0]['value']) && isset($_POST['eid']))
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
                        $entity->field_checkin['und'][$last]['value2'] = date('Y-m-d H:i:s', strtotime($_POST['date']));

                        end($entity->field_utc_checkin['und']);
                        $last = key($entity->field_utc_checkin['und']);
                        $entity->field_utc_checkin['und'][$last]['value2'] = date('Y-m-d H:i:s', time());
                        $entity->save();
                        //list($awards) = studysauce_get_awards();
                    }
                    else
                    {
                        $entity->field_checklist['und'] = array();
                        foreach(explode(',', $_POST['checklist']) as $check)
                            $entity->field_checklist['und'][] = array('value' => $check);
                        $entity->field_checkin['und'][] = array(
                            'value' => date('Y-m-d H:i:s', strtotime($_POST['date']))
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

    list($times, $rows, $total, $hours) = _studysauce_get_metrics();
    global $exportMetricsClasses, $exportMetricsTimes;
    $metrics = theme('studysauce-metrics');

    print json_encode(array(
        'lastSDS' => $lastSDS,
        'rows' => $rows,
        'times' => $exportMetricsTimes,
        'total' => $total,
        'hours' => $hours,
        'classes' => $exportMetricsClasses,
        //'awards' => $awards,
        'eid' => isset($_POST['eid']) ? $_POST['eid'] : null
    ));
    drupal_exit();
}



