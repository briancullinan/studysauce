<?php

function studysauce_invite_send()
{
    // add a partner entity to current user
    global $user;
    $changes = array();
    $account = user_load($user->uid);

    // TODO: check if we have sent an invite already today, field_sent and field_3_day_reminder

    if(isset($_POST['first']) && isset($_POST['last']) && isset($_POST['email']))
    {
        $entity = entity_create('field_collection_item', array('field_name' => 'field_invites'));
        $entity->setHostEntity('user', $account);
        $entity->field_first_name['und'][0]['value'] = $_POST['first'];
        $entity->field_last_name['und'][0]['value'] = $_POST['last'];
        $entity->field_email['und'][0]['value'] = $_POST['email'];
        $entity->field_code['und'][0]['value'] = md5(microtime());

        $message = drupal_mail('studysauce', 'invite', $entity->field_email['und'][0]['value'], language_default(), array(), $account->mail, FALSE);

        $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
        $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('friend-invite'))));
        $message['subject'] = (isset($account->field_first_name[LANGUAGE_NONE][0]['value'])
                ? ($account->field_first_name[LANGUAGE_NONE][0]['value'] . ' ' . $account->field_last_name[LANGUAGE_NONE][0]['value'])
                : 'A fellow student') . ' has invited you to join Study Sauce';
        $sentMessage = array();
        $sentMessage[] = 'Have you ever wondered why students aren\'t taught how to study?';
        $sentMessage[] = '';
        $sentMessage[] = 'We certainly did, which is why we pulled together all the resources you need to be maximize your study time.  Come check us out and stop wasting your time with ineffective study approaches.';
        $message['body'] = theme('studysauce-email', array(
            'message' => implode('<br />', $sentMessage),
            'greeting' => 'Hello ' . $entity->field_first_name['und'][0]['value'] . ' ' . $entity->field_last_name['und'][0]['value'] . ',',
            'link' => l('Go to Study Sauce', '/students', array(
                    'query' => array('invite_code' => $entity->field_code['und'][0]['value'], 'destination' => '#goals'),
                    'absolute' => true,
                    'attributes' => array('style' => '-webkit-appearance: none; margin: 0; text-decoration:none; text-align: left; padding: 10px 25px; color: #1d1d1d; font-weight: 800; font-size: 18px; line-height: 150%; border-radius: 4px; -webkit-border-radius: 4px; background: #FF9900; border-bottom: 3px solid #993300; text-shadow: 0px 1px 2px rgba(255,255,255,0.5);')))));

        $system = drupal_mail_system('studysauce', 'invite');
        $message['result'] = $system->mail($message);

        $entity->field_sent['und'][0]['value'] = date('Y-m-d H:i:s', time());

        $entity->save();
    }

}



