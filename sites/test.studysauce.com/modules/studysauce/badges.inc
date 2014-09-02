<?php


function studysauce_get_awards()
{
    global $user;
    $account = user_load($user->uid);

    $newAwards = array();
    $awards = array(
        'setup-pulse' => false,
        'setup-linked' => false,
        'setup-hours' => false,
        'setup-milestone' => false,
        'setup-outcome' => false,

        'beginner-checkin' => false,
        'beginner-checklist' => false,
        'beginner-commuter' => false,
        'beginner-mix' => false,
        'beginner-breaks' => false,
        'beginner-cram' => false,
        'beginner-brain' => false,
        'beginner-sponsored' => false,
        'beginner-chicken' => false,
        'beginner-apples' => false,
        'beginner-jackpot' => false,

        'intermediate-flier' => false,
        'intermediate-cross' => false,
        'intermediate-breaker' => false,
        'intermediate-cured' => false,
        'intermediate-disco' => false,
        'intermediate-high' => false,

        'advanced-magellan' => false,
        'advanced-bo' => false,
        'advanced-comber' => false,
        'advanced-nocram' => false,
        'advanced-magneto' => false,
        'advanced-wall' => false,
        'advanced-veni' => false
    );
    $conn = studysauce_get_connections();
    $parent = studysauce_get_shared_goals();
    $incentive = studysauce_get_incentive();
    $goals = array();

    $parentStudentGoalBadges = function (&$awards, $entity, $parent = true, &$goals)
    {
        // check if there are any claims saved
        if(isset($entity->field_achievements[LANGUAGE_NONE][0]))
        {
            foreach($entity->field_achievements[LANGUAGE_NONE] as $i => $achieve)
            {
                $aid = $achieve['value'];
                $achievement = entity_load('field_collection_item', array($aid));
                if((isset($achievement[$aid]->field_photo_evidence[LANGUAGE_NONE][0]['filename']) &&
                        !empty($achievement[$aid]->field_photo_evidence[LANGUAGE_NONE][0]['filename'])) ||
                    (isset($achievement[$aid]->field_message[LANGUAGE_NONE][0]['value']) &&
                        !empty($achievement[$aid]->field_message[LANGUAGE_NONE][0]['value'])))
                {
                    $time = $achievement[$aid]->field_time_achieved[LANGUAGE_NONE][0]['value'];
                    $goals[$time] = 'claim';
                    if($entity->field_type[LANGUAGE_NONE][0]['value'] == 'milestone' && !$awards['beginner-apples'])
                        $awards['beginner-apples'] = date('F jS, Y', $time);
                    if($parent && !$awards['beginner-jackpot'])
                        $awards['beginner-jackpot'] = date('F jS, Y', $time);
                    if($entity->field_type[LANGUAGE_NONE][0]['value'] == 'outcome' && !$awards['advanced-veni'])
                        $awards['advanced-veni'] = date('F jS, Y', $time);
                }

            }
        }
    };

    if(isset($parent->field_goals[LANGUAGE_NONE][0]['value']))
        foreach($parent->field_goals[LANGUAGE_NONE] as $i => $goal)
        {
            $eid = $goal['value'];
            $entity = entity_load('field_collection_item', array($eid));
            if($entity[$eid]->field_type[LANGUAGE_NONE][0]['value'] == 'milestone')
                $parentMilestone = $entity[$eid];
            if($entity[$eid]->field_type[LANGUAGE_NONE][0]['value'] == 'outcome')
                $parentOutcome = $entity[$eid];
            if($entity[$eid]->field_type[LANGUAGE_NONE][0]['value'] == 'behavior')
                $parentBehavior = $entity[$eid];

            $parentStudentGoalBadges($awards, $entity[$eid], true, $goals);
        }

    if(isset($incentive->field_goals[LANGUAGE_NONE][0]['value']))
    {
        foreach($incentive->field_goals[LANGUAGE_NONE] as $i => $goal)
        {
            $eid = $goal['value'];
            $entity = entity_load('field_collection_item', array($eid));
            if($entity[$eid]->field_type[LANGUAGE_NONE][0]['value'] == 'milestone')
                $milestone = $entity[$eid];
            if($entity[$eid]->field_type[LANGUAGE_NONE][0]['value'] == 'outcome')
                $outcome = $entity[$eid];
            if($entity[$eid]->field_type[LANGUAGE_NONE][0]['value'] == 'behavior')
                $behavior = $entity[$eid];

            $roId = db_select('field_revision_field_read_only', 'sp');
            $roId->addExpression('MIN(revision_id)');
            $roId = $roId->condition('entity_id', $eid, '=')
                ->condition('field_read_only_value', 1, '=')
                ->range(0, 1)
                ->execute()
                ->fetchField();
            if(!empty($roId))
            {
                $nr = db_select('field_revision_field_goals', 'sp')
                    ->fields('sp', array('revision_id'))
                    ->condition('field_goals_revision_id', $roId, '=')
                    ->condition('field_goals_value', $eid, '=')
                    ->range(0, 1)
                    ->execute()
                    ->fetchField();

                $date = db_select('node_revision', 'sp')
                    ->fields('sp', array('timestamp'))
                    ->condition('vid', $nr, '=')
                    ->range(0, 1)
                    ->execute()
                    ->fetchField();

                if($entity[$eid]->field_type['und'][0]['value'] == 'behavior')
                    $awards['setup-hours'] = date('F jS, Y', $date);
                elseif($entity[$eid]->field_type['und'][0]['value'] == 'milestone')
                    $awards['setup-milestone'] = date('F jS, Y', $date);
                elseif($entity[$eid]->field_type['und'][0]['value'] == 'outcome')
                    $awards['setup-outcome'] = date('F jS, Y', $date);
            }

            $parentStudentGoalBadges($awards, $entity[$eid], false, $goals);
        }

    }

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
        if(isset($node->field_classes[LANGUAGE_NONE][0]))
        {
            $checkins = array();
            $checkouts = array();
            $validLocations = array();
            foreach($node->field_classes[LANGUAGE_NONE] as $i => $class)
            {
                $eid = $class['value'];
                $entity = entity_load('field_collection_item', array($eid));
                if(!empty($entity))
                {
                    $roId = db_select('field_revision_field_checklist', 'sp');
                    $roId->addExpression('MIN(revision_id)');
                    $roId = $roId->condition('entity_id', $eid, '=')
                        ->condition('delta', 5, '=')
                        ->range(0, 1)
                        ->execute()
                        ->fetchField();
                    if(!empty($roId))
                    {
                        $nr = db_select('field_revision_field_classes', 'sp')
                            ->fields('sp', array('revision_id'))
                            ->condition('field_classes_revision_id', $roId, '=')
                            ->condition('field_classes_value', $eid, '=')
                            ->range(0, 1)
                            ->execute()
                            ->fetchField();

                        $date = db_select('node_revision', 'sp')
                            ->fields('sp', array('timestamp'))
                            ->condition('vid', $nr, '=')
                            ->range(0, 1)
                            ->execute()
                            ->fetchField();

                        $awards['beginner-checklist'] = date('F jS, Y', $date);
                    }

                    if(isset($entity[$eid]->field_class_name['und'][0]['value']) && !empty($entity[$eid]->field_class_name['und'][0]['value']))
                    {
                        $awards['setup-pulse'] = date('F jS, Y', $node->created);
                    }
                    if(isset($entity[$eid]->field_checkin[LANGUAGE_NONE][0]))
                    {
                        foreach($entity[$eid]->field_checkin[LANGUAGE_NONE] as $j => $checkin)
                        {
                            if(empty($checkin['value']))
                                continue;

                            $time = strtotime(trim($checkin['value']));
                            $checkins[$time] = $i;

                            if(!$awards['beginner-checkin'])
                                $awards['beginner-checkin'] = date('F jS, Y', $time);

                            // if the checkin time is before the last checkout time then change the checkout time to match checkin time,
                            //    they switched classes in the middle of the session
                            end($checkins);
                            $lastClass = end($checkouts);
                            $lastTime = key($checkouts);
                            if(key($checkins) == $lastTime)
                            {
                                unset($checkouts[$lastTime]);
                                if($time - $lastTime > 3600)
                                    $checkouts[$lastTime + 3600] = $lastClass;
                                else
                                    $checkouts[$time] = $lastClass;
                            }

                            // add the checkout time either from the set value or assume 3600 seconds passed
                            if(isset($checkin['value2']) && !empty($checkin['value2']))
                                $checkouts[strtotime(trim($checkin['value2']))] = $i;
                            else
                                $checkouts[$time] = $i;
                        }
                    }

                    if(isset($entity[$eid]->field_locations[LANGUAGE_NONE][0]['value']))
                    {
                        foreach(explode("\n", $entity[$eid]->field_locations[LANGUAGE_NONE][0]['value']) as $i => $loc)
                        {
                            $locs = explode(',', trim($loc));
                            if(isset($locs[0]) && isset($locs[1]) && floatval($locs[0]) !== false && floatval($locs[1]) !== false)
                            {
                                $latLng = array(floatval($locs[0]), floatval($locs[1]));
                                if(!in_array($latLng, $validLocations))
                                    $validLocations[$locs[2]] = $latLng;
                            }
                        }
                    }
                }
            } // end classes loop

            ksort($checkins);
            ksort($checkouts);
            // check if study checkins are within 2 hours of eachother
            $prev = 0;
            $daysCount = 0;
            $weeks = array();
            $mixCount = 0;
            $breaksCount = 0;
            foreach($checkins as $time => $checkin)
            {
                if($prev > 0 && $time - $prev < 7200 && $checkins[$prev] != $checkins[$time])
                {
                    $mixCount++;
                    if(!$awards['beginner-mix'])
                        $awards['beginner-mix'] = date('F jS, Y', $time);
                    if($mixCount == 5 && !$awards['intermediate-cross'])
                        $awards['intermediate-cross'] = date('F jS, Y', $time);
                    if($mixCount == 10 && !$awards['advanced-bo'])
                        $awards['advanced-bo'] = date('F jS, Y', $time);
                }

                // count sequential days
                if($prev > 0 && $time - $prev < 172799)
                {
                    if(date('F jS, Y', $time) != date('F jS, Y', $prev))
                        $daysCount++;
                }
                else
                    $daysCount = 0;
                if($daysCount == 4 && !$awards['beginner-cram']) // -1 less because $prev counts as the first day
                    $awards['beginner-cram'] = date('F jS, Y', $time);
                if($daysCount == 9 && !$awards['intermediate-cured'])
                    $awards['intermediate-cured'] = date('F jS, Y', $time);
                if($daysCount == 24 && !$awards['advanced-nocram'])
                    $awards['advanced-nocram'] = date('F jS, Y', $time);

                // count weeks of study hours
                if(date('Y', $time) == date('Y', time()))
                {
                    $w = date('W', $time);
                    $weeks[$w] = (isset($weeks[$w])
                            ? $weeks[$w]
                            : 0) +
                        (empty($checkin['value2'])
                            ? 1
                            : ((strtotime($checkin['value2']) - $time) / 3600));
                }

                // find breaks 10 to 15 minutes in length
                $j = 0;
                $checkoutTimes = array_keys($checkouts);
                do
                {
                    if($time - $checkoutTimes[$j] > 600 && $time - $checkoutTimes[$j] < 900)
                    {
                        $breaksCount++;
                        // only use first breaks date
                        if(!$awards['beginner-breaks'])
                            $awards['beginner-breaks'] = date('F jS, Y', $time);
                        if($breaksCount == 5 && !$awards['intermediate-breaker'])
                            $awards['intermediate-breaker'] = date('F jS, Y', $time);
                        if($breaksCount == 10 && !$awards['advanced-comber'])
                            $awards['advanced-comber'] = date('F jS, Y', $time);
                    }

                    $j++;
                } while(isset($checkoutTimes[$j]) && $checkoutTimes[$j] < $time);

                $prev = $time;
            } // end checkins loop

            foreach($weeks as $w => $hoursCount)
            {
                if((isset($behavior->field_hours[LANGUAGE_NONE][0]['value']) &&
                        $hoursCount >= $behavior->field_hours[LANGUAGE_NONE][0]['value'] &&
                        $behavior->field_hours[LANGUAGE_NONE][0]['value'] > 0) || (
                        isset($parentBehavior->field_hours[LANGUAGE_NONE][0]['value']) &&
                        $hoursCount >= $parentBehavior->field_hours[LANGUAGE_NONE][0]['value'] &&
                        $parentBehavior->field_hours[LANGUAGE_NONE][0]['value'] > 0))
                {
                    // keep track of the time goals were achieved
                    $goals[$time] = 'goal';
                    if(!$awards['beginner-chicken'])
                        $awards['beginner-chicken'] = date('F jS, Y', $time);
                }
            }

            // check location based achievements
            $locCount = 0;
            foreach($validLocations as $t => $location)
            {
                $locCount++;
                if($locCount > 1 && !$awards['beginner-commuter'])
                    $awards['beginner-commuter'] = date('F jS, Y', $t);
                if($locCount > 4 && !$awards['intermediate-flier'])
                    $awards['intermediate-flier'] = date('F jS, Y', $t);
                if($locCount > 9 && !$awards['advanced-magellan'])
                    $awards['advanced-magellan'] = date('F jS, Y', $t);
            }
        }
    }

    // sort the goals and get the awards
    ksort($goals);
    $goalCount = 0;
    $claimCount = 0;
    foreach($goals as $time => $goal)
    {
        $goalCount++;
        if($goal != 'goal')
            $claimCount++;
        if($goalCount == 5 && !$awards['intermediate-disco'])
            $awards['intermediate-disco'] = date('F jS, Y', $time);
        if($claimCount == 5 && !$awards['intermediate-high'])
            $awards['intermediate-high'] = date('F jS, Y', $time);
        if($goalCount == 10 && !$awards['advanced-magneto'])
            $awards['advanced-magneto'] = date('F jS, Y', $time);
        if($claimCount == 10 && !$awards['advanced-wall'])
            $awards['advanced-wall'] = date('F jS, Y', $time);
    }

    if(isset($conn[0]))
    {
        $awards['setup-linked'] = date('F jS, Y', max($conn[0]->created, $account->created));
    }

    if($parent)
    {
        $awards['beginner-sponsored'] = date('F jS, Y', $parent->created);
    }

    // loop through awards and figure out which ones are new
    $userBadges = array();

    if(isset($account->field_badges[LANGUAGE_NONE][0]['value']))
    {
        foreach($account->field_badges[LANGUAGE_NONE] as $i => $award)
        {
            $changes['field_badges'][LANGUAGE_NONE][]['value'] = $award['value'];
            $userBadges[] = $award['value'];
        }
    }

    // loop through submissions to find test results
    $quizSubmissions = array();
    $quiz = db_select('webform_submissions', 's')
        ->fields('s', array('sid', 'submitted'))
        ->condition('uid', $account->uid, '=')
        ->condition('nid', 17, '=')
        ->execute();
    while(($sub = $quiz->fetchAssoc()) && isset($sub['sid']))
    {
        $quizSubmissions[$sub['sid']] = $sub['submitted'];
    }

    if(count($quizSubmissions))
    {
        // check if any of the submissions are 100% correct
        $quizSub = db_select('webform_submitted_data', 's')
            ->fields('s', array('sid', 'cid', 'data'))
            ->condition('sid', array_keys($quizSubmissions), 'IN')
            ->execute();
        while(($qSub = $quizSub->fetchAssoc()) && isset($qSub['cid']))
        {
            if($qSub['data'] == 'True')
                unset($quizSubmissions[$qSub['sid']]);
        }
    }

    foreach($quizSubmissions as $sid => $time)
    {
        if(!$awards['beginner-brain'])
        {
            $awards['beginner-brain'] = date('F jS, Y', $time);
            break;
        }
    }

    foreach($awards as $a => $date)
    {
        if($date && !in_array($a, $userBadges))
        {
            $newAwards[] = $a;
            $changes['field_badges'][LANGUAGE_NONE][]['value'] = $a;
        }
    }

    if(count($newAwards))
        user_save($account, $changes);

    return array($awards, end($newAwards));
}
