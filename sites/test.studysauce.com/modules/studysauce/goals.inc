<?php

function _studysauce_unsponsored_goals()
{
    global $user;
    $query = new EntityFieldQuery();
    $entities = $query->entityCondition('entity_type', 'node')
        ->propertyCondition('type', 'incentive')
        ->propertyCondition('title', $user->mail)
        ->propertyCondition('status', 1)
        ->range(0,1)
        ->execute();
    if (!empty($entities['node']))
    {
        $nodes = array_keys($entities['node']);
        $nid = array_shift($nodes);
        $node = node_load($nid);
        if(isset($node->field_goals[LANGUAGE_NONE][0]['value']))
            foreach($node->field_goals[LANGUAGE_NONE] as $i => $g)
            {
                $eid = $g['value'];
                $entity = entity_load('field_collection_item', array($eid));
                if(isset($entity[$eid]))
                    $entity = $entity[$eid];

                if($entity->field_type[LANGUAGE_NONE][0]['value'] == 'milestone')
                    $milestone = $entity;
                if($entity->field_type[LANGUAGE_NONE][0]['value'] == 'outcome')
                    $outcome = $entity;
                if($entity->field_type[LANGUAGE_NONE][0]['value'] == 'behavior')
                    $behavior = $entity;
            }

        return array(
            isset($behavior) ? $behavior : null,
            isset($milestone) ? $milestone : null,
            isset($outcome) ? $outcome : null);
    }
}


function studysauce_save_goals()
{
    global $user;
    $conn = studysauce_get_connections(true);

    $query = new EntityFieldQuery();
    $nodes = $query->entityCondition('entity_type', 'node')
        ->propertyCondition('type', 'incentive')
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
        $node->type = 'incentive';
        $node->language = LANGUAGE_NONE; // Or e.g. 'en' if locale is enabled
        $node->uid = $user->uid;
        $node->status = 1; //(1 or 0): published or not
        $node->revision = 1;
        node_object_prepare($node); // Sets some defaults. Invokes hook_prepare() and hook_node_prepare().
        $node = node_submit($node); // Prepare node for saving
        node_save($node);
    }

    // check for goal of the same type

    if(isset($_POST['type']) && isset($_POST['value']) && isset($_POST['reward']))
    {
        $entities = array();
        if (is_array($node->field_goals[LANGUAGE_NONE]))
        {
            foreach ($node->field_goals[LANGUAGE_NONE] as $i => $class)
            {
                $eid = $class['value'];
                $entity = entity_load('field_collection_item', array($eid));
                if($entity[$eid]->field_type[LANGUAGE_NONE][0]['value'] == $_POST['type'])
                    $thisEid = $entity[$eid]->item_id;
                $entities[$eid] = $entity[$eid];
            }
        }
        if(!isset($thisEid))
        {
            $entity = entity_create('field_collection_item', array('field_name' => 'field_goals'));
            $entity->setHostEntity('node', $node);
        }
        else
            $entity = $entities[$thisEid];
        $entity->revision = 1;
        $entity->field_reward[LANGUAGE_NONE][0]['value'] = $_POST['reward'];
        $entity->field_read_only[LANGUAGE_NONE][0]['value'] = 1;
        if($_POST['type'] == 'hours')
        {
            $entity->field_type[LANGUAGE_NONE][0]['value'] = 'behavior';
            $entity->field_hours[LANGUAGE_NONE][0]['value'] = $_POST['value'];
        }
        if($_POST['type'] == 'grade')
        {
            $entity->field_type[LANGUAGE_NONE][0]['value'] = 'milestone';
            $entity->field_grade[LANGUAGE_NONE][0]['value'] = $_POST['value'];
        }
        if($_POST['type'] == 'gpa')
        {
            $entity->field_type[LANGUAGE_NONE][0]['value'] = 'outcome';
            $entity->field_gpa[LANGUAGE_NONE][0]['value'] = $_POST['value'];
        }
        $entity->save();
    }

    $node = node_submit($node); // Prepare node for saving
    node_save($node);

    // load parent sponsored node
    /*
    $_POST = array('ajax_iframe_upload' => true);
    if(!isset($form_state['build_info']['args'][1]) || $form_state['build_info']['args'][1] != 'parent-sponsored')
    {
        if(isset($parent))
        {
            $commands[] = ajax_command_invoke('#parent-sponsored .node-incentive-form, #parent-sponsored .grid_6, #parent-sponsored h4', 'remove');
            $commands[] = array('command' => 'insert', 'method' => 'append', 'selector' => '#parent-sponsored', 'data' => drupal_render($parent), 'settings' => null);
            $commands[] = ajax_command_invoke('.page-dashboard #incentives .pane-content > div', 'addClass', array('done'));
        }

        $form['changed']['#value'] = $form['changed']['#default_value'];
        $form_state['values']['changed'] = $form['changed']['#default_value'];
        $achievementContent = _studysauce_get_achievements($parent, $form, $achievements);
        $content = drupal_render($form);
        $commands[] = array('command' => 'insert', 'method' => null, 'selector' => '#non-sponsored form', 'data' => $content, 'settings' => null);
    }
    // this only occurs when a form is claimed, the other form doesn't really need to be replaced unless a student and a parent are using it at the same time
    //   the form values are also used in _studysauce_get_achievements, which should just use the node instead
    else
    */

    $achievementContent = _studysauce_get_achievements();
    //if(isset($_GET['achievement']))
    //    $commands[] = ajax_command_invoke('#achievements .grid_3:last-of-type', 'scrollintoview', array(array('padding' => array('top' => 120, 'left' => 0, 'bottom' => 320, 'right' => 0))));

    if(isset($_GET['achievement']) && isset($conn[0]->mail))
    {
        $message = drupal_mail('studysauce', 'achievement', $conn[0]->mail, language_default(), array(), $user->mail, FALSE);

        $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
        $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('achievement'))));
        $message['subject'] = (isset($user->field_first_name[LANGUAGE_NONE][0]['value'])
                ? $user->field_first_name[LANGUAGE_NONE][0]['value']
                : 'Your student') . ' has a study achievement and wanted you to know.';

        $last = end($achievements);
        $sentMessage = array();
        $sentMessage[] = 'Log in to Study Sauce to see your student\'s accomplishment.';
        $message['body'] = theme('studysauce-email', array(
            'message' => implode('<br />', $sentMessage),
            'greeting' => 'Dear ' . (isset($conn[0]->field_first_name[LANGUAGE_NONE][0]['value']) && isset($conn[0]->field_last_name[LANGUAGE_NONE][0]['value'])
                    ? $conn[0]->field_first_name[LANGUAGE_NONE][0]['value'] . ' ' . $conn[0]->field_last_name[LANGUAGE_NONE][0]['value']
                    : 'parent') . ',',
            'link' => l('Study Sauce', 'user/login', array(
                'query' => array('achievement' => $last['code'], 'destination' => '#incentives'),
                'absolute' => true,
                'attributes' => array('style' => 'color:#FF9900;')))));

        $system = drupal_mail_system('studysauce', 'achievement');

        $message['result'] = $system->mail($message);
    }
    elseif(isset($conn[0]->mail) &&
        // only trigger if logged in as a parent setting up rewards
        $user->field_parent_student['und'][0]['value'] == 'parent' &&
        // check if rewards have changed, set in workflows
        isset($GLOBALS['rewards_code']))
    {
        $message = drupal_mail('studysauce', 'reward-setup', $conn[0]->mail, language_default(), array(), $user->mail, FALSE);

        $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
        $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('reward-setup'))));
        $message['subject'] = (isset($user->field_first_name[LANGUAGE_NONE][0]['value'])
                ? $user->field_first_name[LANGUAGE_NONE][0]['value']
                : $user->mail) . ' has set up rewards for you in Study Sauce.';

        $sentMessage = array();
        $sentMessage[] = 'Use Study Sauce to earn rewards when you study.  Log in to see how you can redeem the rewards.';
        $message['body'] = theme('studysauce-email', array(
            'message' => implode('<br />', $sentMessage),
            'greeting' => 'Dear ' . (isset($conn[0]->field_first_name[LANGUAGE_NONE][0]['value']) && isset($conn[0]->field_last_name[LANGUAGE_NONE][0]['value'])
                    ? $conn[0]->field_first_name[LANGUAGE_NONE][0]['value'] . ' ' . $conn[0]->field_last_name[LANGUAGE_NONE][0]['value']
                    : 'parent') . ',',
            'link' => l('Study Sauce', 'user/login', array(
                'query' => array('reward' => $GLOBALS['rewards_code'], 'destination' => '#incentives'),
                'absolute' => true,
                'attributes' => array('style' => 'color:#FF9900;')))));

        $system = drupal_mail_system('studysauce', 'reward-setup');

        $message['result'] = $system->mail($message);
    }

    $awards = studysauce_get_awards();
    print json_encode(array(
        'goals' => '<div>' . theme('studysauce-goals') . '</div>',
        'awards' => $awards,
        'achievements' => '<div>' . $achievementContent . '</div>'
    ));
}

function _studysauce_get_achievements()
{
    // TODO: load from the node instead of the form?
    $parent = studysauce_get_shared_goals();
    if(isset($parentIncentive) && $parentIncentive !== false)
    {
        $i = 0;
        do
        {
            $goal = $parent->field_goals[LANGUAGE_NONE][$i];
            if(isset($goal['#entity']->field_achievements[LANGUAGE_NONE][0]))
                foreach($goal['#entity']->field_achievements[LANGUAGE_NONE] as $j => $achievement)
                {
                    $entity = entity_load('field_collection_item', array($achievement['value']));
                    $achievement = $entity[$achievement['value']];
                    $time = $achievement->field_time_achieved[LANGUAGE_NONE][0]['value'];
                    $achievements[$time]['code'] = $achievement->field_code[LANGUAGE_NONE][0]['value'];
                    $achievements[$time]['activated'] = $achievement->field_activated[LANGUAGE_NONE][0]['value'];
                    $image = '';
                    $message = '';
                    if(isset($achievement->field_photo_evidence[LANGUAGE_NONE][0]['filename']))
                    {
                        $uri = $achievement->field_photo_evidence[LANGUAGE_NONE][0]['uri'];
                        $image =  '<img src="' . image_style_url('achievement', $uri) . '" />';
                    }
                    if(isset($achievement->field_message[LANGUAGE_NONE][0]['value']) &&
                        isset($achievement->field_time_achieved[LANGUAGE_NONE][0]['value']))
                    {
                        $message = '<p>' . $achievement->field_message[LANGUAGE_NONE][0]['value'] . '</p>';
                    }
                    $achievements[$time]['content'] = '<div class="grid_' . (!empty($image) && !empty($message) ? 6 : 3) .
                        (isset($_SESSION['studysauce']['achievement']) && $_SESSION['studysauce']['achievement'] == $achievements[$time]['code'] ? ' highlighted' : '') . '"><strong>' .
                        date('F jS, Y', $time) . '</strong>' . $image . $message . '</div>';
                }
        } while (isset($parent['field_goals'][LANGUAGE_NONE][++$i]));
    }

    $achievements = array();
    $i = 0;
    if(isset($form['field_goals'][LANGUAGE_NONE][0]))
        do
        {
            $goal = $form['field_goals'][LANGUAGE_NONE][$i];
            if(isset($goal['#entity']->field_achievements[LANGUAGE_NONE][0]))
                foreach($goal['#entity']->field_achievements[LANGUAGE_NONE] as $j => $achievement)
                {
                    $entity = entity_load('field_collection_item', array($achievement['value']));
                    $achievement = $entity[$achievement['value']];
                    $time = $achievement->field_time_achieved[LANGUAGE_NONE][0]['value'];
                    $achievements[$time]['code'] = $achievement->field_code[LANGUAGE_NONE][0]['value'];
                    $achievements[$time]['activated'] = $achievement->field_activated[LANGUAGE_NONE][0]['value'];
                    $image = '';
                    $message = '';
                    if(isset($achievement->field_photo_evidence[LANGUAGE_NONE][0]['filename']))
                    {
                        $uri = $achievement->field_photo_evidence[LANGUAGE_NONE][0]['uri'];
                        $image =  '<img src="' . image_style_url('achievement', $uri) . '" />';
                    }
                    if(isset($achievement->field_message[LANGUAGE_NONE][0]['value']) &&
                        isset($achievement->field_time_achieved[LANGUAGE_NONE][0]['value']))
                    {
                        $message = '<p>' . $achievement->field_message[LANGUAGE_NONE][0]['value'] . '</p>';
                    }
                    $achievements[$time]['content'] = '<div class="grid_' . (!empty($image) && !empty($message) ? 6 : 3) .
                        (isset($_SESSION['studysauce']['achievement']) && $_SESSION['studysauce']['achievement'] == $achievements[$time]['code'] ? ' highlighted' : '') . '"><strong>' .
                        date('F jS, Y', $time) . '</strong>' . $image . $message . '</div>';
                }
        } while (isset($form['field_goals'][LANGUAGE_NONE][++$i]));

    ksort($achievements);
    $result = '';
    foreach($achievements as $t => $a)
        $result .= $a['content'];
    if (isset($_SESSION['studysauce']['achievement']))
        unset($_SESSION['studysauce']['achievement']);
    return $result . '<p style="margin:0;clear:both;line-height:0;">&nbsp;</p>';
}

function studysauce_get_shared_goals()
{
    global $user;
    $user = user_load($user->uid);
    if(!empty($user->field_invites) && !empty($user->field_invites[LANGUAGE_NONE]))
    {
        foreach($user->field_invites[LANGUAGE_NONE] as $i => $invite)
        {
            $entity = entity_load('field_collection_item', array($invite['value']));
            $entity = $entity[$invite['value']];
            if(isset($entity->field_activated[LANGUAGE_NONE][0]['value']) &&
                isset($entity->field_email[LANGUAGE_NONE][0]['value']) &&
                $entity->field_activated[LANGUAGE_NONE][0]['value'] &&
                $entity->field_email[LANGUAGE_NONE][0]['value'] != $user->mail)
            {
                $query = new EntityFieldQuery();
                $incentives = $query->entityCondition('entity_type', 'node')
                    ->propertyCondition('type', 'incentive')
                    ->propertyCondition('title', $entity->field_email[LANGUAGE_NONE][0]['value'])
                    ->propertyCondition('status', 1)
                    ->range(0,1)
                    ->execute();

                if (isset($incentives['node']) && !empty($incentives['node']))
                    break;
            }
        }
    }

    if(!isset($incentives['node']) || empty($incentives['node']))
    {
        // try to find invite
        $inviteQuery = new EntityFieldQuery();
        $invites = $inviteQuery->entityCondition('entity_type', 'field_collection_item')
            ->propertyCondition('field_name', 'field_invites')
            ->fieldCondition('field_email', 'value', $user->mail)
            ->fieldCondition('field_activated', 'value', 1)
            ->execute();

        if (isset($invites['field_collection_item']) && !empty($invites['field_collection_item']))
        {
            $invites = array_keys($invites['field_collection_item']);
            $eid = array_shift($invites);
            $invite = entity_load('field_collection_item', array($eid));
            $invite = $invite[$eid];
            $otherUser = studysauce_get_inviting_user($invite);

            $query = new EntityFieldQuery();
            $incentives = $query->entityCondition('entity_type', 'node')
                ->propertyCondition('type', 'incentive')
                ->propertyCondition('title', $otherUser->mail)
                ->propertyCondition('status', 1)
                ->range(0,1)
                ->execute();
        }
    }

    if (isset($incentives['node']) && !empty($incentives['node']))
    {
        $nodes = array_keys($incentives['node']);
        $nid = array_shift($nodes);
        $node = node_load($nid);
        return $node;
    }
    return false;
}


