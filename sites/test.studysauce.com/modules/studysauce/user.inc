<?php

function studysauce_group_save()
{
    // add user invites to adviser account
    global $user;
    $changes = array();
    $account = user_load($user->uid);

    // TODO: check if we have sent an invite already today, field_sent and field_3_day_reminder

    if(isset($_POST['users']))
    {
        foreach($_POST['users'] as $i => $u)
        {
            $entity = entity_create('field_collection_item', array('field_name' => 'field_invites'));
            $entity->setHostEntity('user', $account);
            $entity->field_first_name['und'][0]['value'] = $u['first'];
            $entity->field_last_name['und'][0]['value'] = $u['last'];
            $entity->field_email['und'][0]['value'] = $u['email'];
            $entity->field_code['und'][0]['value'] = md5(microtime());

            // send counselor invite email
            $message = drupal_mail('studysauce', 'invite', $entity->field_email['und'][0]['value'], language_default(), array(), $account->mail, FALSE);

            $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
            $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('counselor-invite'))));
            $message['subject'] = 'Welcome to Study Sauce!';
            $sentMessage = array();
            $sentMessage[] = 'Hello ' . $u['first'] . ',';
            $sentMessage[] = '';
            $sentMessage[] = 'You have been invited to Study Sauce by ' . $account->field_first_name['und'][0]['value'] . ' ' . $account->field_last_name['und'][0]['value'] . '.  Your account has been prepaid.  Please click the link below to activate your account.';
            $message['body'] = theme('studysauce-email', array(
                'message' => implode('<br />', $sentMessage),
                'greeting' => 'Hello ' . $entity->field_first_name['und'][0]['value'] . ' ' . $entity->field_last_name['und'][0]['value'] . ',',
                'link' => l('Set up my Study Sauce account.', '/students', array(
                    'query' => array('invite_code' => $entity->field_code['und'][0]['value'], 'destination' => '#goals'),
                    'absolute' => true,
                    'attributes' => array('style' => '-webkit-appearance: none; margin: 0; text-decoration:none; text-align: left; padding: 10px 25px; color: #1d1d1d; font-weight: 800; font-size: 18px; line-height: 150%; border-radius: 4px; -webkit-border-radius: 4px; background: #FF9900; border-bottom: 3px solid #993300; text-shadow: 0px 1px 2px rgba(255,255,255,0.5);')))));

            $system = drupal_mail_system('studysauce', 'invite');
            $message['result'] = $system->mail($message);

            $entity->field_sent['und'][0]['value'] = date('Y-m-d H:i:s', time());

            $entity->save();
        }
    }


}

function studysauce_form_user_login_alter(&$form, &$form_state)
{
    $form['#validate'][] = 'studysauce_user_login';
}

function studysauce_form_user_register_form_alter(&$form, &$form_state)
{
    $form['#validate'][] = 'studysauce_user_login';
}

function studysauce_user_login()
{
    global $user;
    // do not enter by funnel if the user was invited as an accountability partner
    if(isset($user->mail))
    {
        $partnerQuery = new EntityFieldQuery();
        $partners = $partnerQuery->entityCondition('entity_type', 'field_collection_item')
            ->propertyCondition('field_name', 'field_partners')
            ->fieldCondition('field_email', 'value', $user->mail)
            ->execute();
    }
    // TODO: check for partner code in session


    if(!in_array('adviser', $user->roles) && (!isset($partners['field_collection_item']) || empty($partners['field_collection_item'])))
    {

        // check if the user was invited by an adviser and put the user in that group
        $isBeingAdvised = array();
        if(isset($user->mail))
        {
            $partnerQuery = new EntityFieldQuery();
            $invites = $partnerQuery->entityCondition('entity_type', 'field_collection_item')
                ->propertyCondition('field_name', 'field_invites')
                ->fieldCondition('field_email', 'value', $user->mail)
                ->execute();

            // check for invite code in session
            if(!isset($invites['field_collection_item']) && isset($_SESSION['studysauce']['invite_code']))
            {
                $invites = $partnerQuery->entityCondition('entity_type', 'field_collection_item')
                    ->propertyCondition('field_name', 'field_invites')
                    ->fieldCondition('field_code', 'value', $_SESSION['studysauce']['invite_code'])
                    ->execute();
            }

            // get if host entity is an adviser
            if(isset($invites['field_collection_item']))
            {
                foreach($invites['field_collection_item'] as $i => $inv)
                {
                    $inviteEntity = entity_load('field_collection_item', array($i));
                    $adviserUser = $inviteEntity[$i]->hostEntity();
                    $account = user_load($adviserUser->uid);
                    if(in_array('adviser', $account->roles))
                    {
                        $isBeingAdvised[] = $account;
                    }
                }
            }
        }

        // check if they are a customer
        $lastOrder = _studysauce_orders_by_uid($user->uid);
        $groups = og_get_groups_by_user();

        // if the user is being advised but hasn't been added to the group
        if(count($isBeingAdvised) && !isset($groups['node']))
        {
            foreach($isBeingAdvised as $j => $a)
            {
                // get the adviser group
                $groups = og_get_groups_by_user($a);
                $groups = array_keys($groups['node']);
                $gid = end($groups);
                $group = node_load($gid);

                // Add the user to the group
                og_group('node', $gid, array(
                    'entity type'       => 'user',
                    'entity'        => $user
                ));
            }

            $groups = og_get_groups_by_user();
        }

        if(isset($groups['node']) || $lastOrder)
        {
            // check if buy funnel is complete
            $query = new EntityFieldQuery();
            $nodes = $query->entityCondition('entity_type', 'node')
                ->propertyCondition('type', 'profile')
                ->propertyCondition('title', isset($user->mail) ? $user->mail : '')
                ->propertyCondition('status', 1)
                ->range(0, 1)
                ->execute();
            if (empty($nodes['node']))
                $GLOBALS['studysauce-goto'] = 'profile';
            else
            {
                // check if questions are answered and return to exact step
                $nodes = array_keys($nodes['node']);
                $nid = array_shift($nodes);
                $node = node_load($nid);
                if(!isset($node->field_mindset['und'][0]['value']) || !isset($node->field_time_management['und'][0]['value']) ||
                    !isset($node->field_devices['und'][0]['value']) || !isset($node->field_education['und'][0]['value']) ||
                    !isset($node->field_study_much['und'][0]['value']))
                    $GLOBALS['studysauce-goto'] = 'profile';
            }

            if(!isset($GLOBALS['studysauce-goto']))
            {
                // check if schedule exists
                $query = new EntityFieldQuery();
                $nodes = $query->entityCondition('entity_type', 'node')
                    ->propertyCondition('type', 'schedule')
                    ->propertyCondition('title', isset($user->mail) ? $user->mail : '')
                    ->propertyCondition('status', 1)
                    ->range(0, 1)
                    ->execute();
                if (empty($nodes['node']))
                    $GLOBALS['studysauce-goto'] = 'schedule';
                else
                {
                    // check if we need to visit customization pages
                    $nodes = array_keys($nodes['node']);
                    $nid = array_shift($nodes);
                    $node = node_load($nid);
                    if(!isset($node->field_weekends['und'][0]['value']))
                        $GLOBALS['studysauce-goto'] = 'customization';
                    elseif(!isset($node->field_classes['und'][0]['value']))
                        $GLOBALS['studysauce-goto'] = 'schedule';
                    else
                    {
                        $cid = $node->field_classes['und'][0]['value'];
                        $c = entity_load('field_collection_item', array($cid));
                        if(isset($c[$cid]) && !isset($c[$cid]->field_study_difficulty['und'][0]['value']))
                        {
                            $GLOBALS['studysauce-goto'] = 'customization2';
                        }
                    }
                }
            }

            // TODO: change this when we need to check for different versions of the profile form
        }
    }

    if(!isset($GLOBALS['studysauce-goto']))
        $GLOBALS['studysauce-goto'] = '<front>';
}

function studysauce_drupal_goto_alter(&$path, &$options, &$http_response_code)
{
    if(isset($GLOBALS['studysauce-goto']))
    {
        // alter drupal_goto
        if (is_array($GLOBALS['studysauce-goto'])) {
            $path = $GLOBALS['studysauce-goto'][0];
            $options = array();
            if (count($GLOBALS['studysauce-goto']) > 1) {
                $options = $GLOBALS['studysauce-goto'][1];
            }
        }
        else {
            $path = $GLOBALS['studysauce-goto'];
        }
    }
}

function studysauce_save_status()
{
    $groups = og_get_groups_by_user();
    if(isset($groups['node']) && isset($_POST['uid']) && isset($_POST['status']))
    {
        $query = db_select("og_membership", "ogm");
        $query->condition("ogm.gid", array_keys($groups['node']), "=");
        $query->fields("ogm", array("entity_type", "etid"));
        $result = $query->execute();
        $members = $result->fetchAll();
        foreach($members as $i => $member)
        {
            if($member->etid == $_POST['uid'])
            {
                $m = user_load($member->etid);
                $changes = array('field_adviser_status' => array('und' => array(array('value' => $_POST['status']))));
                user_save($m, $changes);
            }
        }
    }
}

function studysauce_connector_action_alter(&$actions)
{
    $actions['default']['login callback'] = 'studysauce_connector_login';
}

function studysauce_connector_login($connector_name, $cid = NULL, $consumer = NULL, $access_token = NULL, $request_token = NULL)
{
    $connector = _connector_get_connectors($connector_name);
    $authname = $connector_name . '__' . $cid;
    if ($connector) {

        // check if email address already exists
        $info = array();
        if (!empty($connector['information callback']) && is_callable($connector['information callback'])) {
            $info = $connector['information callback']($connector, $cid, array(), $access_token);

            if(isset($info['email']['value']) && !empty($info['email']['value']))
            {
                $account = user_external_load($authname);
                // Check if the external id already connected to someone
                if (!$account)
                {
                    $account = user_load_by_mail($info['email']['value']);
                    if($account)
                    {
                        // add authmap connection before we attempt to login the natural way
                        _connector_add_connection($connector_name, $cid, $account->uid);
                    }
                }
            }
        }
    }

    return _connector_log_in($connector_name, $cid, $consumer, $access_token, $request_token);
}

function studysauce_save_user($account = null, &$edit = array())
{
    if(!isset($edit))
        $edit = array();

    if($account == null)
    {
        global $user;
        $account = user_load($user->uid);
    }

    if(isset($_POST['cancel']))
    {
        $edit = array('user_cancel_notify' => false);
        user_cancel($edit, $account->uid, 'user_cancel_block');
        return;
    }

    if(isset($_POST['first']) && isset($_POST['last']) && isset($_POST['email']))
    {
        $oldEmail = false;
        $edit['field_first_name']['und'][0]['value'] = $_POST['first'];
        $edit['field_last_name']['und'][0]['value'] = $_POST['last'];
        if(strcmp($account->mail, $_POST['email']) <> 0)
        {
            $edit['mail'] = $_POST['email'];
            $oldEmail = $account->mail;
        }
        if(isset($_POST['pass']) && !empty($_POST['pass']))
            $edit['pass'] = $_POST['pass'];

        user_save($account, $edit);

        if($oldEmail)
        {
            // find nodes whose title matches the old e-mail
            $nodes = db_select('node', 'n')
                ->fields('n', array('nid'))
                ->condition('title', $oldEmail, '=')
                ->condition(db_or()->condition('uid', $account->uid, '='))
                ->execute()
                ->fetchAllAssoc('nid');
            foreach($nodes as $nid => $n)
            {
                $node = node_load($nid);
                $node->title = $edit['mail'];
                node_save($node);
            }

            // change accountability partner emails
            $partnerQuery = new EntityFieldQuery();
            $partners = $partnerQuery->entityCondition('entity_type', 'field_collection_item')
                ->propertyCondition('field_name', 'field_partners')
                ->fieldCondition('field_email', 'value', $oldEmail)
                ->execute();
            if(isset($partners['field_collection_item']) && !empty($partners['field_collection_item']))
            {
                $partners = array_keys($partners['field_collection_item']);
                foreach($partners as $p)
                {
                    $partner = entity_load('field_collection_item', array($p));
                    $partner[$p]->field_email['und'][0]['value'] = $edit['mail'];
                    $partner[$p]->save();
                }
            }
        }
    }

    if(isset($_POST['picture'][0]))
    {
        $file = file_load($_POST['picture'][0]['fid']);
        $fileName = isset($_POST['picture'][0]['rename']) ? $_POST['picture'][0]['rename'] : $file->filename;
        $location = 'public:///';
        $filePath = $location . $fileName;
        if ($file->uri !== $filePath) {
            file_prepare_directory($location, FILE_CREATE_DIRECTORY);
            file_move($file, $filePath);
        }
        $edit = array();
        $edit['picture'] = $file;

        user_save($account, $edit);
    }

    // if parent selected jump down below, everything else assume student
    if(substr($account->mail, -strlen('@internal.example.org')) != '@internal.example.org')
        if(isset($_POST['parent-student']) && $_POST['parent-student'] == 'student')
        {
            $msg = <<< EOHTML
Congratulations on taking the first step to improving your study effectiveness!  To get the most out of Study Sauce, we recommend a few key things to do.

1. Enter your important deadlines and we will send you reminders to stay on track.

2. Check in when you study and we will guide you through the best study techniques.

3. Set up study goals and rewards.

As always, we are happy to help with any questions that you might have.  Just email us at admin@studysauce.com.
EOHTML;

            $body = theme('studysauce-email', array(
                'message' => implode('<br />', explode("\n", $msg)),
                'greeting' => 'Dear ' . (isset($account->field_first_name[LANGUAGE_NONE][0]['value'])
                        ? $account->field_first_name[LANGUAGE_NONE][0]['value']
                        : 'student') . ','));

            $new_message = drupal_mail('studysauce', 'welcome', $account->mail, language_default(), array(), variable_get('site_mail', ''), FALSE);

            $new_message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
            $new_message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('welcome-student'))));
            $new_message['subject'] = 'Welcome to Study Sauce';
            $new_message['body'] = str_replace('admin@studysauce.com', '<a href="mailto:admin@studysauce.com" style="color:#FF9900;">admin@studysauce.com</a>', $body);

            $system = drupal_mail_system('studysauce', 'welcome');
            $new_message['result'] = $system->mail($new_message);

        }
        elseif(isset($_POST['parent-student']) && $_POST['parent-student'] == 'parent')
        {
            $msg = <<< EOHTML
Thank you for joining Study Sauce.  To get the most out of our service, we recommend a few key things to do.

1. Send an invitation to your student to join and learn the best study methods.

2. Give your student a little extra motivation. Incentive psychology works, try it!

3. Purchase a personalized study plan for your student. We guarantee a higher GPA or your money back.

As always, we are happy to help with any questions that you might have. Just email us at admin@studysauce.com.
EOHTML;

            $body = theme('studysauce-email', array(
                'message' => implode('<br />', explode("\n", $msg)),
                'greeting' => 'Dear ' . (isset($account->field_first_name[LANGUAGE_NONE][0]['value']) && isset($account->field_last_name[LANGUAGE_NONE][0]['value'])
                        ? $account->field_first_name[LANGUAGE_NONE][0]['value'] . ' ' . $account->field_last_name[LANGUAGE_NONE][0]['value']
                        : 'parent') . ','));

            $new_message = drupal_mail('studysauce', 'welcome', $account->mail, language_default(), array(), variable_get('site_mail', ''), FALSE);

            $new_message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
            $new_message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('welcome-parent'))));
            $new_message['subject'] = 'Welcome to Study Sauce';
            $new_message['body'] = str_replace('admin@studysauce.com', '<a href="mailto:admin@studysauce.com" style="color:#FF9900;">admin@studysauce.com</a>', $body);

            $system = drupal_mail_system('studysauce', 'welcome');
            $new_message['result'] = $system->mail($new_message);

        }

}


