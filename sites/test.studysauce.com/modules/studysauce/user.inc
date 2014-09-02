<?php

function studysauce_group_save()
{
    set_time_limit(0);
    // add user invites to adviser account
    global $user;
    $account = user_load($user->uid);

    if(isset($_POST['users']))
    {
        foreach($_POST['users'] as $i => $u)
        {
            // check if we have sent an invite already today, field_sent and field_3_day_reminder
            // find other invitations to this user
            $query = db_select('field_data_field_invites', 'inv');
            $query->fields('inv', array('field_invites_value'));
            $query->fields('em', array('field_email_value'));
            $query->fields('se', array('field_sent_value'));
            $query->fields('re', array('field_3_day_reminder_value'));
            $query->join('field_data_field_email', 'em', 'em.entity_id = inv.field_invites_value');
            $query->condition('em.field_email_value', $u['email'], '=');

            // check there is no sent entry from the last 24 hours
            $query->join('field_data_field_sent', 'se', 'se.entity_id = inv.field_invites_value');
            $sentAnd = db_and();
            $sentAnd->condition('se.field_sent_value', date('Y-m-d H:i:s', time() - 3600*24), '>=');
            $sentAnd->condition('se.field_sent_value', date('Y-m-d H:i:s', time()), '<=');

            $query->leftJoin('field_data_field_3_day_reminder', 're', 're.entity_id = inv.field_invites_value');
            $remAnd = db_and();
            $remAnd->condition('re.field_3_day_reminder_value', date('Y-m-d H:i:s', time() - 3600*24), '>=');
            $remAnd->condition('re.field_3_day_reminder_value', date('Y-m-d H:i:s', time()), '<=');

            $query->condition(db_or()->condition($sentAnd)->condition($remAnd));
            $entities = $query->execute()->fetchAllAssoc('field_invites_value');
            //if (!empty($entities))
            //    continue;

            $entity = entity_create('field_collection_item', array('field_name' => 'field_invites'));
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
            $sentMessage[] = 'You have been invited to Study Sauce by ' . $account->field_first_name['und'][0]['value'] . ' ' . $account->field_last_name['und'][0]['value'] . '.  Your account has been prepaid.  Please click the link below to activate your account.';
            $sentMessage[] = '';
            $sentMessage[] = '';
            $sentMessage[] = 'Once you create your account, you will answer a short survey about your study preferences which we will use to build your personalized study tools.';
            $sentMessage[] = '';
            $sentMessage[] = '';
            $sentMessage[] = 'If you have any questions at all during the process, please feel free to contact Stephen Houghton.  I can be reached by email at stephen@studysauce.com or by phone at 480-331-8570.';
            $sentMessage[] = '';
            $message['body'] = theme('studysauce-email', array(
                'message' => implode('<br />', $sentMessage),
                'greeting' => 'Hello ' . $entity->field_first_name['und'][0]['value'] . ' ' . $entity->field_last_name['und'][0]['value'] . ',',
                'link' => l('Set up my Study Sauce account.', 'adviserprepaid', array(
                    'query' => array('invite_code' => $entity->field_code['und'][0]['value']),
                    'absolute' => true,
                    'attributes' => array('style' => '-webkit-appearance: none; margin: 0; text-decoration:none; text-align: left; padding: 10px 25px; color: #1d1d1d; font-weight: 800; font-size: 18px; line-height: 150%; border-radius: 4px; -webkit-border-radius: 4px; background: #FF9900; border-bottom: 3px solid #993300; text-shadow: 0px 1px 2px rgba(255,255,255,0.5);')))));

            $system = drupal_mail_system('studysauce', 'invite');
            $message['result'] = $system->mail($message);

            $entity->field_sent['und'][0]['value'] = date('Y-m-d H:i:s', time());

            $edit = array();
            if(isset($account->field_invites['und']) && !empty($account->field_invites['und']))
                $edit['field_invites']['und'] = array_values($account->field_invites['und']);

            $entity->setHostEntity('user', $account);
            $entity->save(true);

            $edit['field_invites']['und'][] = array('value' => $entity->item_id, 'entity' => $entity);

            user_save($account, $edit);

        }
    }

    print json_encode(array(
        'import' => theme('studysauce-adviser-import')
    ));
    drupal_exit();
}

function studysauce_form_user_login_alter(&$form, &$form_state)
{
    $form['#validate'][] = 'studysauce_user_login';
}

function studysauce_form_user_register_form_alter(&$form, &$form_state)
{
    $form['#validate'][] = 'studysauce_user_associate';
    $form['#validate'][] = 'studysauce_user_login';
}

function studysauce_user_associate()
{
    global $user;
    $account = user_load($user->uid);

    //only do this for new accounts
    if(!isset($account->mail) || empty($account->mail) || $user->uid == 0)
        return;

    // check for partner code in session
    if(isset($_SESSION['studysauce']['partner_code']))
    {
        $partnerQuery = new EntityFieldQuery();
        $partners = $partnerQuery->entityCondition('entity_type', 'field_collection_item')
            ->propertyCondition('field_name', 'field_partners')
            ->fieldCondition('field_code', 'value', $_SESSION['studysauce']['partner_code'])
            ->execute();

        // update inviters email address
        if(isset($partners['field_collection_item']) && !empty($partners['field_collection_item']))
            foreach($partners['field_collection_item'] as $eid => $p)
            {
                $e = entity_load('field_collection_item', array($eid));
                $host = $e[$eid]->hostEntity();
                if(isset($host) &&
                    $e[$eid]->field_email['und'][0]['value'] != $account->mail &&
                    $account->uid != $host->uid)
                {
                    $e[$eid]->revision = 1;
                    $e[$eid]->field_email['und'][0]['value'] = $account->mail;
                    $e[$eid]->save();
                }
            }
    }

    // check for invite code in session
    if(isset($_SESSION['studysauce']['invite_code']))
    {
        $partnerQuery = new EntityFieldQuery();
        $invites = $partnerQuery->entityCondition('entity_type', 'field_collection_item')
            ->propertyCondition('field_name', 'field_invites')
            ->fieldCondition('field_code', 'value', $_SESSION['studysauce']['invite_code'])
            ->execute();

        // re-associate invite email and sign up email by modifying the invite record
        if(isset($invites['field_collection_item']) && !empty($invites['field_collection_item']))
            foreach($invites['field_collection_item'] as $i => $inv)
            {
                $inviteEntity = entity_load('field_collection_item', array($i));
                $adviserUser = $inviteEntity[$i]->hostEntity();
                if(isset($adviserUser) &&
                    $inviteEntity[$i]->field_email['und'][0]['value'] != $account->mail &&
                    $account->uid != $adviserUser->uid)
                {
                    $inviteEntity[$i]->revision = 1;
                    $inviteEntity[$i]->field_email['und'][0]['value'] = $account->mail;
                    $inviteEntity[$i]->save();
                }
            }
    }


    // TODO: the same thing for anonymous order invites
}

function studysauce_user_login()
{
    global $user;
    $account = user_load($user->uid);
    // do not enter by funnel if the user was invited as an accountability partner

    //only do this for new accounts
    if(!isset($account->mail) || empty($account->mail) || $user->uid == 0)
        return;

    if(!in_array('adviser', $account->roles) && !in_array('master adviser', $account->roles) &&
        !in_array('partner', $account->roles) && !in_array('parent', $account->roles))
    {
        // check if the user was invited by an adviser and put the user in that group
        $isBeingAdvised = array();
        if(isset($account->mail))
        {
            $partnerQuery = new EntityFieldQuery();
            $invites = $partnerQuery->entityCondition('entity_type', 'field_collection_item')
                ->propertyCondition('field_name', 'field_invites')
                ->fieldCondition('field_email', 'value', $account->mail)
                ->execute();

            // check for invite code in session
            if(isset($invites['field_collection_item']) && !empty($invites['field_collection_item']))
                foreach($invites['field_collection_item'] as $i => $inv)
                {
                    $inviteEntity = entity_load('field_collection_item', array($i));
                    if(($adviserUser = $inviteEntity[$i]->hostEntity()) != null)
                    {
                        $adviser = user_load($adviserUser->uid);
                        if(in_array('adviser', $adviser->roles) || in_array('master adviser', $adviser->roles))
                        {
                            $isBeingAdvised[] = $adviser;
                        }
                    }
                }
        }

        // check if they are a customer
        $lastOrder = _studysauce_orders_by_uid($account->uid);
        $groups = og_get_groups_by_user();

        // if the user is being advised but hasn't been added to the group
        if(count($isBeingAdvised) && !isset($groups['node']))
        {
            foreach($isBeingAdvised as $j => $a)
            {
                // get the adviser group
                $groups = og_get_groups_by_user($a);
                if(is_array($groups['node']))
                {
                    $groups = array_keys($groups['node']);
                    $gid = end($groups);

                    // Add the user to the group
                    og_group('node', $gid, array(
                        'entity type'       => 'user',
                        'entity'        => $account
                    ));
                }
            }

            // refresh the groups list for use below
            $groups = og_get_groups_by_user();
        }

        if(isset($groups['node']) || $lastOrder)
        {
            $GLOBALS['studysauce-goto'] = studysauce_funnel_progress();
        }
    }

    if(!isset($GLOBALS['studysauce-goto']))
        $GLOBALS['studysauce-goto'] = '<front>';
}

function studysauce_funnel_progress()
{
    global $user;
    $account = user_load($user->uid);

    // check if buy funnel is complete
    $query = new EntityFieldQuery();
    $nodes = $query->entityCondition('entity_type', 'node')
        ->propertyCondition('type', 'profile')
        ->propertyCondition('title', isset($account->mail) ? $account->mail : '')
        ->propertyCondition('status', 1)
        ->range(0, 1)
        ->execute();
    if (empty($nodes['node']))
        return 'profile';
    else
    {
        // check if questions are answered and return to exact step
        $nodes = array_keys($nodes['node']);
        $nid = array_shift($nodes);
        $node = node_load($nid);
        if(!isset($node->field_mindset['und'][0]['value']) || !isset($node->field_time_management['und'][0]['value']) ||
            !isset($node->field_devices['und'][0]['value']) || !isset($node->field_education['und'][0]['value']) ||
            !isset($node->field_study_much['und'][0]['value']))
            return 'profile';
    }

    $classes = _studysauce_get_schedule_classes();
    if(!count($classes))
        return 'schedule';
    else
    {
        $query = new EntityFieldQuery();
        $nodes = $query->entityCondition('entity_type', 'node')
            ->propertyCondition('type', 'schedule')
            ->propertyCondition('title', isset($account->mail) ? $account->mail : '')
            ->propertyCondition('status', 1)
            ->range(0, 1)
            ->execute();

        $nodes = array_keys($nodes['node']);
        $nid = array_shift($nodes);
        $node = node_load($nid);
        if(!isset($node->field_weekends['und'][0]['value']))
            return 'customization';
    }

    foreach($classes as $e => $class)
    {
        $entity = entity_load('field_collection_item', array($e));
        if(isset($entity[$e]) && !isset($entity[$e]->field_study_difficulty['und'][0]['value']))
            return 'customization2';
    }

    // TODO: change this when we need to check for completion of different versions of the profile form
    return '<front>';
}

function studysauce_create_order($account)
{
    global $user;
    $order = uc_order_new($account->uid, 'completed');
    uc_order_comment_save($order->order_id, $user->uid, t('Order created programmatically.'), 'admin');
    $product = node_load(13);
    uc_product_load(array($product->nid => $product));
    $attribute = uc_attribute_load(4);
    $option = uc_attribute_option_load(14);
    if(is_string($product->data))
        $product->data = unserialize($product->data);
    $product->data['attributes']['Payment Option'][14] = 'Yearly';
    $product->module = 'uc_recurring_order';
    //$product->price =
    $product->order = $order;
    $product->order_uid = $order->uid;
    $product->qty = 1;
    $product->price = 99.99;
    $order->products[] = $product;
    $products = uc_recurring_product_get_recurring_products_in_order($order);
    uc_order_module_invoke('load', $order, NULL);
    $order->line_items = uc_order_load_line_items($order);
    uc_order_save($order);
}

function studysauce_drupal_goto_alter(&$path, &$options, &$http_response_code)
{
    // don't modify goto if it leads to a product page
    if(strpos($path, '<front>') === false && $path != '/' && $path != '' &&
        strpos($path, 'user') === false && strpos($path, 'node') === false)
        return;
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
        $query = db_select('og_membership', 'ogm');
        $query->condition('ogm.gid', array_keys($groups['node']), 'IN');
        $query->fields('ogm', array('entity_type', 'etid'));
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

    $result = _connector_log_in($connector_name, $cid, $consumer, $access_token, $request_token);
    if($result)
    {
        studysauce_user_associate();
    }
    return $result;
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

    $password_error = false;
    if(isset($_POST['first']) && isset($_POST['last']) && isset($_POST['email']))
    {
        $oldEmail = false;
        $edit['field_first_name']['und'][0]['value'] = $_POST['first'];
        $edit['field_last_name']['und'][0]['value'] = $_POST['last'];
        if(strcmp($account->mail, $_POST['email']) <> 0 &&
            isset($_POST['pass']) && !empty($_POST['pass']))
        {
            require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');
            if(user_check_password($_POST['pass'], $account))
            {
                $edit['mail'] = $_POST['email'];
                $oldEmail = $account->mail;
            }
        }
        if(isset($_POST['pass']) && !empty($_POST['pass']) &&
            isset($_POST['newPass']) && !empty($_POST['newPass']))
        {
            require_once DRUPAL_ROOT . '/' . variable_get('password_inc', 'includes/password.inc');
            if(user_check_password($_POST['pass'], $account))
                $edit['pass'] = $_POST['newPass'];
            else
                $password_error = true;
        }

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

        print json_encode(array(
            'password_error' => $password_error
        ));
    }

    if(isset($_POST['picture'][0]))
    {
        /*$fileName = isset($_POST['picture'][0]['rename']) ? $_POST['picture'][0]['rename'] : $file->filename;
        $location = 'public:///';
        $filePath = $location . $fileName;
        if ($file->uri !== $filePath) {
            file_prepare_directory($location, FILE_CREATE_DIRECTORY);
            file_move($file, $filePath);
        }*/
        $file = file_load($_POST['picture'][0]['fid']);
        $edit = array();
        $edit['picture'] = $file;

        user_save($account, $edit);
    }

    // add user to partner role
    $partnerQuery = new EntityFieldQuery();
    $partners = $partnerQuery->entityCondition('entity_type', 'field_collection_item')
        ->propertyCondition('field_name', 'field_partners')
        ->fieldCondition('field_email', 'value', $account->mail)
        ->execute();
    if(isset($partners['field_collection_item']) && !empty($partners['field_collection_item']) &&
        !in_array('partner', $account->roles))
    {
        if ($role = user_role_load_by_name('partner'))
        {
            $account->roles[$role->rid] = 'partner';
            $edit['roles'][$role->rid] = 'partner';
        }
    }

    // if parent selected jump down below, everything else assume student
    if(substr($account->mail, -strlen('@internal.example.org')) != '@internal.example.org')
        if(!in_array('parent', $account->roles) && !in_array('partner', $account->roles) &&
            (!isset($edit['roles']) || (!in_array('parent', $edit['roles']) && !in_array('partner', $edit['roles']))))
        {
            studysauce_welcome_student($account);
        }
        elseif(in_array('partner', $account->roles) || (isset($edit['roles']) && in_array('partner', $edit['roles'])))
        {
            studysauce_welcome_partner($account);
        }

}


