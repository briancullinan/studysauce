<?php


function studysauce_save_partner()
{
    // add a partner entity to current user
    global $user;
    $result = array();
    $shouldEmail = true;
    $account = user_load($user->uid);
    $account->revision = 1;

    if(isset($_POST['first']) && isset($_POST['last']) && isset($_POST['email']))
    {
        // figure out if we should send the e-mail if the address has changed or it hasn't been sent within a day
        if(isset($account->field_partners['und'][0]['value']) &&
            ($old = entity_load('field_collection_item', array($account->field_partners['und'][0]['value']))) &&
            isset($old[$account->field_partners['und'][0]['value']]->field_email['und'][0]['value']) &&
            // check if new email address matches old email address
            $old[$account->field_partners['und'][0]['value']]->field_email['und'][0]['value'] == $_POST['email'] &&
            // check if invite was sent within the last day
            date_timezone_set(new DateTime($old[$account->field_partners['und'][0]['value']]->field_sent['und'][0]['value'], new DateTimeZone('UTC')), new DateTimeZone(date_default_timezone_get()))->getTimestamp() > time() - 86400)
        {
            // copy old code because email is the same
            $code = $old[$account->field_partners['und'][0]['value']]->field_code['und'][0]['value'];
            $shouldEmail = false;
        }
        else
            $code = md5(microtime());

        $entity = entity_create('field_collection_item', array('field_name' => 'field_partners'));
        $entity->field_first_name['und'][0]['value'] = $_POST['first'];
        $entity->field_last_name['und'][0]['value'] = $_POST['last'];
        $entity->field_email['und'][0]['value'] = $_POST['email'];
        $entity->field_code['und'][0]['value'] = $code;

        if(isset($_POST['permissions']))
            $entity->field_permissions['und'] = array_map(function ($x) { return array('value' => $x); }, explode(',', $_POST['permissions']));

        if(isset($_POST['uploads']))
        {
            $entity->field_partner_photo['und'][0]['fid'] = $_POST['uploads'][0]['fid'];
            $file = file_load($_POST['uploads'][0]['fid']);
            $fileName = isset($_POST['uploads'][0]['rename']) ? $_POST['uploads'][0]['rename'] : $file->filename;
            $location = 'public:///';
            $filePath = $location . $fileName;
            if ($file->uri !== $filePath) {
                file_prepare_directory($location, FILE_CREATE_DIRECTORY);
                file_move($file, $filePath);
            }
        }

        if(isset($entity->field_partner_photo['und'][0]['fid']))
        {
            $file = file_load($entity->field_partner_photo['und'][0]['fid']);
            $result['uri'] = image_style_url('achievement', $file->uri);
        }

        if($shouldEmail)
        {
            $message = drupal_mail('studysauce', 'partner', $entity->field_email['und'][0]['value'], language_default(), array(), $account->mail, FALSE);

            $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
            $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('partner-invite'))));
            $message['subject'] = (isset($account->field_first_name[LANGUAGE_NONE][0]['value'])
                    ? ($account->field_first_name[LANGUAGE_NONE][0]['value'] . ' ' . $account->field_last_name[LANGUAGE_NONE][0]['value'])
                    : 'A fellow student') . ' needs your help with school';
            $sentMessage = array();
            $sentMessage[] = $account->field_first_name[LANGUAGE_NONE][0]['value'] . ' wants to invite you to become an accountability partner.';
            $sentMessage[] = '';
            $sentMessage[] = 'Research shows that simply writing down goals makes them more likely to be achieved. Having an accountability partner greatly increases the probability of achievement. All students have ups and downs in school and finding someone to help motivate and challenge them along the way can be invaluable.';
            $sentMessage[] = '';
            $sentMessage[] = 'You can work out the specific expectations with ' . $account->field_first_name[LANGUAGE_NONE][0]['value'] . ', but a good accountability partner tends to have the following attributes:';
            $sentMessage[] = '<ul style="font-family: \'Ubuntu\',Helvetica Neue,Arial,sans-serif; font-size: 16px; color: #555555;"><li>Will challenge a student (needs more than just encouragement)</li>';
            $sentMessage[] = '<li>Will celebrate student successes</li>';
            $sentMessage[] = '<li>Is emotionally invested in the student\'s education</li>';
            $sentMessage[] = '<li>Commits to regular communication about academics</li></ul>';
            $message['body'] = theme('studysauce-email', array(
                'message' => implode('<br />', $sentMessage),
                'greeting' => 'Hello ' . $entity->field_first_name['und'][0]['value'] . ' ' . $entity->field_last_name['und'][0]['value'] . ',',
                'link' => l('If you are prepared to help ' . $account->field_first_name[LANGUAGE_NONE][0]['value'] . ', click here to join Study Sauce and learn more about how we help students achieve their academic goals.', 'partners', array(
                    'query' => array('partner_code' => $entity->field_code['und'][0]['value']),
                    'absolute' => true,
                    'attributes' => array('style' => 'color:#FF9900;')))));

            $system = drupal_mail_system('studysauce', 'partner');
            $message['result'] = $system->mail($message);

            $entity->field_sent['und'][0]['value'] = date('Y-m-d H:i:s', time());
        }

        // remove other partners from user account
        if(isset($account->field_partners['und']) && !empty($account->field_partners['und']))
            entity_delete_multiple('field_collection_item', array_map(function ($x) {return $x['value'];}, $account->field_partners['und']));

        $entity->setHostEntity('user', $account);
        $entity->save(true);

        $edit = array();
        $edit['field_partners']['und'] = array(array('value' => $entity->item_id, 'entity' => $entity));

        user_save($account, $edit);
    }

    print json_encode($result);
}

