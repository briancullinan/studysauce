<?php


function studysauce_save_partner()
{
    // add a partner entity to current user
    global $user;
    $changes = array();
    $user = user_load($user->uid);

    if(isset($_POST['first']) && isset($_POST['last']) && isset($_POST['email']))
    {
        $entity = entity_create('field_collection_item', array('field_name' => 'field_partners'));
        $entity->setHostEntity('user', $user);
        $entity->field_first_name['und'][0]['value'] = $_POST['first'];
        $entity->field_last_name['und'][0]['value'] = $_POST['last'];
        $entity->field_email['und'][0]['value'] = $_POST['email'];
        $entity->field_code['und'][0]['value'] = md5(microtime());

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

        $message = drupal_mail('studysauce', 'partner', $entity->field_email['und'][0]['value'], language_default(), array(), $user->mail, FALSE);

        $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
        $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('partner-invite'))));
        $message['subject'] = (isset($user->field_first_name[LANGUAGE_NONE][0]['value'])
                ? ($user->field_first_name[LANGUAGE_NONE][0]['value'] . ' ' . $user->field_last_name[LANGUAGE_NONE][0]['value'])
                : 'A fellow student') . ' needs your help with academic accountability';
        $sentMessage = array();
        $sentMessage[] = $user->field_first_name[LANGUAGE_NONE][0]['value'] . ' wants to invite you to become an accountability partner.';
        $sentMessage[] = '';
        $sentMessage[] = 'Research shows that simply writing down goals makes them more likely to be achieved. Having an accountability partner greatly increases the probability of achievement. All students have ups and downs in school and finding someone to help motivate and challenge them along the way can be invaluable.';
        $sentMessage[] = 'You can work out the specific expectations with ' . $user->field_first_name[LANGUAGE_NONE][0]['value'] . ', but a good accountability partner tends to have the following attributes:<ul>';
        $sentMessage[] = '<li>Will challenge a student (needs more than just encouragement)</li>';
        $sentMessage[] = '<li>Will celebrate student successes</li>';
        $sentMessage[] = '<li>Is emotionally invested in the student\'s education</li>';
        $sentMessage[] = '<li>Commits to regular communication about academics</li></ol>';
        $message['body'] = theme('studysauce-email', array(
            'message' => implode('<br />', $sentMessage),
            'greeting' => 'Hello ' . $entity->field_first_name['und'][0]['value'] . ' ' . $entity->field_last_name['und'][0]['value'] . ',',
            'link' => l('If you are prepared to help ' . $user->field_first_name[LANGUAGE_NONE][0]['value'] . ', click here to join Study Sauce and learn more about how we help students achieve their academic goals.', '<front>', array(
                'query' => array('partner_code' => $entity->field_code['und'][0]['value'], 'destination' => '#goals'),
                'absolute' => true,
                'attributes' => array('style' => 'color:#FF9900;')))));

        $system = drupal_mail_system('studysauce', 'achievement');
        $message['result'] = $system->mail($message);
        $entity->field_sent['und'][0]['value'] = date('Y-m-d H:i:s', time());

        $entity->save();
        $changes = array();
        $changes['field_partners']['und'][0]['value'] = $entity->item_id;
        $changes['field_partners']['und'][0]['revision_id'] = $entity->revision_id;
        $changes['field_partners']['und'] = array();
        foreach($user->field_partners['und'] as $i => $c)
        {
            if($c['value'] == $entity->item_id)
            {
                $changes['field_partners']['und'][0] = $user->field_partners['und'][$i];
                continue;
            }
            entity_delete('field_collection_item', array($c['value']));
        }

        user_save($user, $changes);
    }

}

