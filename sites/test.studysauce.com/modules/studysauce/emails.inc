<?php

function studysauce_welcome_partner($account)
{
    $msg = <<< EOHTML
Thank you for joining Study Sauce on behalf of your student.  To get the most out of our service, we recommend a few key things to get started.

1. Set up regular check in meetings with your student to discuss the highs and lows of school.  We recommend weekly meetings.

2. Lean on your experience to help students avoid common pitfalls.  Try to find a good balance between being supportive and challenging your student during your weekly meetings.

3. Consider purchasing a personalized study plan for your student to help take their studying to the next level.

As always, we are happy to help with any questions that you might have. Just email us at admin@studysauce.com.
EOHTML;

    $body = theme('studysauce-email', array(
        'message' => implode('<br />', explode("\n", $msg)),
        'greeting' => 'Dear ' . (isset($account->field_first_name[LANGUAGE_NONE][0]['value'])
                ? $account->field_first_name[LANGUAGE_NONE][0]['value']
                : 'student') . ','));

    $new_message = drupal_mail('studysauce', 'welcome', $account->mail, language_default(), array(), variable_get('site_mail', ''), FALSE);

    $new_message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
    $new_message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('welcome-partner'))));
    $new_message['subject'] = 'Welcome to Study Sauce';
    $new_message['body'] = str_replace('admin@studysauce.com', '<a href="mailto:admin@studysauce.com" style="color:#FF9900;">admin@studysauce.com</a>', $body);

    $system = drupal_mail_system('studysauce', 'welcome');
    $new_message['result'] = $system->mail($new_message);
}

function studysauce_welcome_student($account)
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

function studysauce_admin_email($post)
{
    $message = drupal_mail('studysauce', 'reminder', variable_get('site_mail', 'admin@studysauce.com'), language_default(), array(), null, FALSE);

    $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
    $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('reminders'))));
    $message['subject'] = 'New submission from: ' . current_path();

    $results = '';
    array_walk($post, function ($v, $k) use (&$results) {
        $results .= $k . ' = ' . $v . '<br />';
    });
    $message['body'] = theme('studysauce-email', array(
        'message' => $results,
        'greeting' => 'Hi Admin',
        'link' => l('Click here to view submission', '<front>', array(
            'absolute' => true,
            'attributes' => array('style' => 'color:#FF8900;')))));

    $system = drupal_mail_system('studysauce', 'reminder');

    $message['result'] = $system->mail($message);
}

function studysauce_invite_billing_send()
{
    global $user;
    $account = user_load($user->uid);

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
        $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('sponsor-invite'))));
        $message['subject'] = (isset($account->field_first_name['und'][0]['value'])
                ? $account->field_first_name['und'][0]['value']
                : 'Your student') . ' has asked for your help with school';

        $message['body'] = theme('studysauce-email', array(
            'message' => (isset($account->field_first_name['und'][0]['value'])
                    ? $account->field_first_name['und'][0]['value']
                    : 'Your student') . ' has asked for your help to sponsor a premium Study Sauce account.' .
                '<br /><br />' .
                'We started Study Sauce based on the realization that no one ever teaches students the most effective study methods. We thought that was crazy when considering the fact that students spend up to 75% of their time outside the classroom.' .
                '<br /><br />' .
                'We have carefully researched the most effective study techniques and have incorporated the leading science into our online service to teach our students how to employ the best study methods.' .
                '<br /><br />' .
                'Click on the below link to find out more about how we can help your student become a better studier.',
            'greeting' => 'Dear ' . (isset($entity->field_first_name['und'][0]['value'])
                    ? $entity->field_first_name['und'][0]['value']
                    : 'parent') . ',',
            'link' => l('Go to Study Sauce', 'billmyparents', array(
                'query' => array('invite_code' => $entity->field_code['und'][0]['value']),
                'absolute' => true,
                'attributes' => array('style' => 'color:#FF8900;')))));

        $system = drupal_mail_system('studysauce', 'invite');
        $message['result'] = $system->mail($message);

        $entity->field_sent['und'][0]['value'] = date('Y-m-d H:i:s', time());
        $entity->save();
    }

    drupal_json_output(true);
}

function studysauce_invite_parent_student_send()
{
    global $user;
    $account = user_load($user->uid);

    if(isset($_POST['first']) && isset($_POST['last']) && isset($_POST['email']))
    {
        $entity = entity_create('field_collection_item', array('field_name' => 'field_invites'));

        $entity->field_first_name['und'][0]['value'] = $_POST['first'];
        $entity->field_last_name['und'][0]['value'] = $_POST['last'];
        $entity->field_email['und'][0]['value'] = $_POST['email'];
        $entity->field_code['und'][0]['value'] = md5(microtime());

        $message = drupal_mail('studysauce', 'invite', $entity->field_email['und'][0]['value'], language_default(), array(), null, FALSE);

        $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
        $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('sponsor-invite'))));
        $message['subject'] = 'Your parent has asked for you to join Study Sauce';

        $message['body'] = theme('studysauce-email', array(
            'message' => 'We started Study Sauce based on the realization that no one ever teaches students the most effective study methods.  We thought that was crazy when considering the fact that students spend up to 75% of their time outside the classroom.' .
                '<br /><br />' .
                'We have carefully researched the most effective study techniques and have incorporated the leading science into our online service to teach our students how to employ the best study methods.  Come discover the secret sauce to studying and find out how other students like you are raising their GPAs.',
            'greeting' => 'Dear ' . $entity->field_first_name['und'][0]['value'] . ',',
            'link' => l('Go to Study Sauce', 'parentinvite', array(
                'query' => array('invite_code' => $entity->field_code['und'][0]['value']),
                'absolute' => true,
                'attributes' => array('style' => 'color:#FF8900;')))));

        $system = drupal_mail_system('studysauce', 'invite');
        $message['result'] = $system->mail($message);

        $entity->field_sent['und'][0]['value'] = date('Y-m-d H:i:s', time());
        $entity->setHostEntity('user', $account);
        $entity->save(true);
        field_attach_update('user', $account);
    }


    drupal_json_output(true);
}

function studysauce_prepay_send($order, $account, $invite_email)
{
    $entity = entity_create('field_collection_item', array('field_name' => 'field_invites'));
    $entity->setHostEntity('user', $account);
    $invite = studysauce_get_invite_entity();
    if(!isset($invite))
    {
        $invite = studysauce_get_partner_entity();
    }
    if(isset($invite))
    {
        $host = $invite->hostEntity();
        if(isset($host->field_first_name['und'][0]['value']))
            $entity->field_first_name['und'][0]['value'] = $host->field_first_name['und'][0]['value'];
        if(isset($host->field_last_name['und'][0]['value']))
            $entity->field_last_name['und'][0]['value'] = $host->field_last_name['und'][0]['value'];
        if(isset($host->mail))
            $entity->field_email['und'][0]['value'] = $invite_email;
    }
    else
    {
        $entity->field_email['und'][0]['value'] = $invite_email;
    }
    $otherUser = user_load_by_mail($invite_email);
    $entity->field_code['und'][0]['value'] = md5(microtime());

    $message = drupal_mail('studysauce', 'invite', $entity->field_email['und'][0]['value'], language_default(), array(), null, FALSE);

    $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
    $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('sponsor-invite'))));
    $message['subject'] = $order->billing_first_name . ' has prepaid for your study plan';

    $message['body'] = theme('studysauce-email', array(
        'message' => 'Your personal study plan has been prepaid by ' . $order->billing_first_name . ' ' . $order->billing_last_name . '.' .
            '<br /><br />' .
            'We started Study Sauce based on the realization that no one ever teaches students the most effective study methods.  Considering the fact that students spend 75% of college studying outside of the classroom.  We thought that was crazy.  Especially when considering the fact that we have carefully researched the science behind the most effective studying techniques and use them to build your study plan for the entire semester.' .
            '<br /><br />' .
            'Our students find that they no longer have to cram for tests and are far less stressed when they use our methods (the same methods that Jeopardy champions use to retain information - btw).  Come discover the secret sauce to studying and find out how other students like you are raising their GPAs.' .
            '<br /><br />' .
            'Please click below to finish filling out your class information, so we can finalize your study plan.',
        'greeting' => 'Dear ' . (isset($entity->field_first_name['und'][0]['value'])
            ? $entity->field_first_name['und'][0]['value']
            : 'student') . ',',
        'link' => l('Go to Study Sauce', !empty($otherUser) ? 'userprepaid' : 'prepaid', array(
            'query' => array('invite_code' => $entity->field_code['und'][0]['value']),
            'absolute' => true,
            'attributes' => array('style' => 'color:#FF8900;')))));

    $system = drupal_mail_system('studysauce', 'invite');
    $message['result'] = $system->mail($message);

    $entity->field_sent['und'][0]['value'] = date('Y-m-d H:i:s', time());
    $entity->save();
}

function studysauce_3_day_invite($invite_user, $invite_entity)
{
    $message = drupal_mail('studysauce', 'invite', $invite_entity->field_email['und'][0]['value'], language_default(), array(), empty($invite_user->mail) ? null : $invite_user->mail, FALSE);

    $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
    $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('3-day-invite-students'))));
    $message['subject'] = 'Your invitation' . (isset($invite_user->field_first_name['und'][0]['value'])
            ? (' from ' . $invite_user->field_first_name['und'][0]['value'])
            : '') . ' to join Study Sauce is still pending';

    $message['body'] = theme('studysauce-email', array(
        'message' => 'We started Study Sauce based on the realization that no one ever teaches students the most effective study methods.  We thought that was crazy when considering the fact that students spend up to 75% of their time outside the classroom.' .
            '<br /><br />' .
            'We have carefully researched the most effective study techniques and have incorporated the leading science into our online service to teach our students how to employ the best study methods.  Come discover the secret sauce to studying and find out how other students like you are raising their GPAs.',
        'greeting' => 'Dear ' . (isset($invite_entity->field_first_name['und'][0]['value'])
                ? $invite_entity->field_first_name['und'][0]['value']
                : 'student') . ',',
        'link' => l('Study Sauce', 'students', array(
            'query' => array('invite_code' => $invite_entity->field_code['und'][0]['value']),
            'absolute' => true,
            'attributes' => array('style' => 'color:#FF8900;')))));

    $system = drupal_mail_system('studysauce', 'invite');
    $message['result'] = $system->mail($message);

    $invite_entity->field_3_day_reminder['und'][0]['value'] = date('Y-m-d H:i:s', time());
    $invite_entity->save(true);
}

function studysauce_3_day_prepaid_invite($invite_user, $invite_entity)
{
    $message = drupal_mail('studysauce', 'invite', $invite_entity->field_email['und'][0]['value'], language_default(), array(), null, FALSE);

    $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
    $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('bill-my-parents-reminder'))));
    $message['subject'] = 'Your prepaid Study Sauce account is still pending';

    $otherUser = user_load_by_mail($invite_entity->field_email['und'][0]['value']);

    $message['body'] = theme('studysauce-email', array(
        'message' => 'We wanted to remind you that your premium Study Sauce account has been prepaid.  Please follow the link below to build a personalized study plan for your current school term.' .
            '<br /><br />' .
            'If you have any questions at all, please feel free to email us.',
        'greeting' => 'Dear ' . (isset($invite_entity->field_first_name['und'][0]['value'])
                ? $invite_entity->field_first_name['und'][0]['value']
                : 'student') . ',',
        'link' => l('Go to Study Sauce', !empty($otherUser) ? 'userprepaid' : 'prepaid', array(
            'query' => array('invite_code' => $invite_entity->field_code['und'][0]['value']),
            'absolute' => true,
            'attributes' => array('style' => 'color:#FF8900;')))));

    $system = drupal_mail_system('studysauce', 'invite');
    $message['result'] = $system->mail($message);

    $invite_entity->field_3_day_reminder['und'][0]['value'] = date('Y-m-d H:i:s', time());
    $invite_entity->save();
}

function studysauce_3_day_billmyparents_invite($invite_user, $invite_entity)
{
    $message = drupal_mail('studysauce', 'invite', $invite_entity->field_email['und'][0]['value'], language_default(), array(), $invite_user->mail, FALSE);

    $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
    $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('bill-my-parents-reminder'))));
    $message['subject'] = 'Your invitation' . (isset($invite_user->field_first_name['und'][0]['value'])
            ? (' from ' . $invite_user->field_first_name['und'][0]['value'])
            : '') . ' to join Study Sauce is still pending';

    $message['body'] = theme('studysauce-email', array(
        'message' => (isset($invite_user->field_first_name['und'][0]['value'])
                ? $invite_user->field_first_name['und'][0]['value']
                : 'Your student') . ' has asked for your help to sponsor a premium Study Sauce account.' .
            '<br /><br />' .
            'We started Study Sauce based on the realization that no one ever teaches students the most effective study methods. We thought that was crazy when considering the fact that students spend up to 75% of their time outside the classroom.' .
            '<br /><br />' .
            'We have carefully researched the most effective study techniques and have incorporated the leading science into our online service to teach our students how to employ the best study methods.' .
            '<br /><br />' .
            'Click on the below link to find out more about how we can help your student become a better studier.',
        'greeting' => 'Dear ' . (isset($invite_entity->field_first_name['und'][0]['value'])
                ? $invite_entity->field_first_name['und'][0]['value']
                : 'student') . ',',
        'link' => l('Go to Study Sauce', 'billmyparents', array(
            'query' => array('invite_code' => $invite_entity->field_code['und'][0]['value']),
            'absolute' => true,
            'attributes' => array('style' => 'color:#FF8900;')))));

    $system = drupal_mail_system('studysauce', 'invite');
    $message['result'] = $system->mail($message);

    $invite_entity->field_3_day_reminder['und'][0]['value'] = date('Y-m-d H:i:s', time());
    $invite_entity->save();
}

function studysauce_3_day_b2b_partner_invite($invite_user, $invite_entity)
{
    $message = drupal_mail('studysauce', 'invite', $invite_entity->field_email['und'][0]['value'], language_default(), array(), $invite_user->mail, FALSE);

    $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
    $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('3-day-invite-partner'))));
    $message['subject'] = 'Your prepaid Study Sauce account is still pending';

    $message['body'] = theme('studysauce-email', array(
        'message' => 'We wanted to remind you that your premium Study Sauce account has been prepaid. Please follow the link below to build a personalized study plan for your current school term.' .
                     '<br /><br />' .
                     'Once you create your account, you will answer a short survey about your study preferences which we will use to build your personalized study tools.' .
                     '<br /><br />' .
                     'If you have any questions at all during the process, please feel free to contact Stephen Houghton.  I can be reached by email at stephen@studysauce.com or by phone at 480-331-8570.' .
                     '<br />',
        'greeting' => 'Dear ' . (isset($invite_entity->field_first_name['und'][0]['value'])
                ? $invite_entity->field_first_name['und'][0]['value']
                : 'student') . ',',
        'link' => l('Set up my Study Sauce account.', 'adviserprepaid', array(
            'query' => array('invite_code' => $invite_entity->field_code['und'][0]['value']),
            'absolute' => true,
            'attributes' => array('style' => '-webkit-appearance: none; margin: 0; text-decoration:none; text-align: left; padding: 10px 25px; color: #1d1d1d; font-weight: 800; font-size: 18px; line-height: 150%; border-radius: 4px; -webkit-border-radius: 4px; background: #FF9900; border-bottom: 3px solid #993300; text-shadow: 0px 1px 2px rgba(255,255,255,0.5);')))));

    $system = drupal_mail_system('studysauce', 'invite');
    $message['result'] = $system->mail($message);

    $invite_entity->field_3_day_reminder['und'][0]['value'] = date('Y-m-d H:i:s', time());
    $invite_entity->save();
}

function studysauce_3_day_partner_invite($invite_user, $invite_entity)
{
    $message = drupal_mail('studysauce', 'invite', $invite_entity->field_email['und'][0]['value'], language_default(), array(), $invite_user->mail, FALSE);

    $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
    $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('3-day-invite-partner'))));
    $message['subject'] = 'Your invitation' . (isset($invite_user->field_first_name['und'][0]['value'])
            ? (' from ' . $invite_user->field_first_name['und'][0]['value'])
            : '') . ' to join Study Sauce is still pending';

    $message['body'] = theme('studysauce-email', array(
        'message' => $invite_user->field_first_name[LANGUAGE_NONE][0]['value'] . ' wants to invite you to become an accountability partner.' .
            '<br /><br />' .
            'Research shows that simply writing down goals makes them more likely to be achieved. Having an accountability partner greatly increases the probability of achievement. All students have ups and downs in school and finding someone to help motivate and challenge them along the way can be invaluable.' .
            '<br /><br />' .
            'You can work out the specific expectations with ' . $invite_user->field_first_name[LANGUAGE_NONE][0]['value'] . ', but a good accountability partner tends to have the following attributes:' .
            '<br />' .
            '<ul style="font-family: \'Ubuntu\',Helvetica Neue,Arial,sans-serif; font-size: 16px; color: #555555;"><li>Will challenge a student (needs more than just encouragement)</li>' .
            '<br />' .
            '<li>Will celebrate student successes</li>' .
            '<br />' .
            '<li>Is emotionally invested in the student\'s education</li>' .
            '<br />' .
            '<li>Commits to regular communication about academics</li></ul>',
        'greeting' => 'Dear ' . (isset($invite_entity->field_first_name['und'][0]['value'])
                ? $invite_entity->field_first_name['und'][0]['value']
                : 'student') . ',',
        'link' => l('If you are prepared to help ' . $invite_user->field_first_name[LANGUAGE_NONE][0]['value'] . ', click here to join Study Sauce and learn more about how we help students achieve their academic goals.', 'partners', array(
            'query' => array('invite_code' => $invite_entity->field_code['und'][0]['value']),
            'absolute' => true,
            'attributes' => array('style' => 'color:#FF8900;')))));

    $system = drupal_mail_system('studysauce', 'invite');
    $message['result'] = $system->mail($message);

    $invite_entity->field_3_day_reminder['und'][0]['value'] = date('Y-m-d H:i:s', time());
    $invite_entity->save();
}

function studysauce_3_day_marketing($account)
{
    $message = drupal_mail('studysauce', 'followup', $account->mail, language_default(), array(), null, FALSE);

    $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
    $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('3-day-marketing-students'))));
    $message['subject'] = 'Get the most out of your Study Sauce account';

    $message['body'] = theme('studysauce-followup');

    $system = drupal_mail_system('studysauce', 'followup');

    $message['result'] = $system->mail($message);

    $edit['field_3_day_marketing']['und'][0]['value'] = 1;
    user_save($account, $edit);
}

function studysauce_reminders($account, $reminders)
{
    $reminderOutput = count($reminders) > 1 ? 'Below are your reminders.<br /><br />' : 'Below is your reminder.<br /><br />';
    $classes = array();
    foreach($reminders as $reminder)
    {
        $classI = 0;
        $found = false;
        $classNames = _studysauce_get_schedule_classes($account);
        $classI = array_search($reminder->field_class_name['und'][0]['value'], array_values($classNames));

        if($classI === false)
            $classI = -1;

        if($classI == 0)
            $color = '#FF0D00';
        elseif($classI == 1)
            $color = '#FF8900';
        elseif($classI == 2)
            $color = '#FFD700';
        elseif($classI == 3)
            $color = '#BAF300';
        elseif($classI == 4)
            $color = '#2DD700';
        elseif($classI == 5)
            $color = '#009999';
        elseif($classI == 6)
            $color = '#162EAE';
        elseif($classI == 7)
            $color = '#6A0AAB';
        elseif($classI == 8)
            $color = '#BE008A';
        else
            $color = '#DDDDDD';

        $timespan = strtotime($reminder->field_due_date['und'][0]['value']) - time();
        if(floor($timespan / 86400) + 1 <= 0)
            $days = 'today';
        elseif(floor($timespan / 86400) + 1 > 1)
            $days = floor($timespan / 86400) + 1 . ' days';
        else
            $days = '1 day';

        $reminderOutput .= '<br /><strong>Subject:</strong><br /><span style="height:24px;width:24px;background-color:' . $color . ';display:inline-block;border-radius:100%;border: 3px solid #555555;">&nbsp;</span> ' . $reminder->field_class_name['und'][0]['value'] . '<br /><br /><strong>Assignment:</strong><br />' . (isset($reminder->field_assignment['und'][0]['value']) ? $reminder->field_assignment['und'][0]['value'] : '') . '<br /><br /><strong>Days until due date:</strong><br />' . $days . '<br /><br />';
        if(array_search($reminder->field_class_name['und'][0]['value'], $classes) === false)
            $classes[] = $reminder->field_class_name['und'][0]['value'];

        // save the sent status of the reminder
        foreach(array(86400,172800,345600,604800,1209600) as $i => $t)
        {
            if($timespan - $t <= 0)
            {
                $reminder->revision = 1;
                $reminder->field_reminder_sent['und'][]['value'] = $t;
                $reminder->save();
                break;
            }
        }
    }

    $message = drupal_mail('studysauce', 'reminder', $account->mail, language_default(), array(), null, FALSE);

    $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
    $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('reminders'))));
    $message['subject'] = 'You have a notification for ' . implode(', ', $classes);

    $message['body'] = theme('studysauce-email', array(
        'footer' => '<h2 style="font-size:18px;text-align:center;color: #555555;margin-top:100px;">Take your studying to the next level!</h2><p style="text-align:center;font-family: \'Ubuntu\',Helvetica Neue,Arial,sans-serif;color: #555555;"><a href="' . url('buy', array('absolute' => true)) . '" style="font-size:18px;font-weight:700;padding:12px 25px;border-radius:4px;border-bottom: 3px solid #004477;background: #0099be;display:inline-block;margin-top:5px;color:#FFFFFF;text-decoration:none;">Buy a personalized study plan</a></p>',
        'message' => $reminderOutput,
        'greeting' => 'Hi ' . (isset($account->field_first_name['und'][0]['value'])
                ? $account->field_first_name['und'][0]['value']
                : 'student') . ',',
        'link' => l('Click here to log in to Study Sauce and edit your deadlines', '<front>', array(
            'fragment' => 'dates',
            'absolute' => true,
            'attributes' => array('style' => 'color:#FF8900;')))));

    $system = drupal_mail_system('studysauce', 'reminder');

    $message['result'] = $system->mail($message);

}

function studysauce_b2b_inactivity($account, $partner)
{
    $message = drupal_mail('studysauce', 'inactivity', $account->mail, language_default(), array(), null, FALSE);

    $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
    $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('inactivity'))));
    $message['subject'] = 'Improve your grades with Study Sauce';

    // check if student is being advised
    // TODO: move this to cron logic
    $adviser = 'Additionally, your adviser, ' . $partner->field_first_name['und'][0]['value'] . ' ' . $partner->field_last_name['und'][0]['value'] . ', will be viewing the data from Study Sauce for your periodic check ins.<br /><br />';

    $message['body'] = theme('studysauce-email', array(
        'message' => 'We haven\'t seen you in a while.  By using Study Sauce when you study, you can apply the very best study methods to make sure you do your best in every class.<br /><br />' . $adviser . 'If you have any questions, comments, or feedback, we would love to hear them.  Just reply to this email.',
        'greeting' => 'Hello ' . (isset($account->field_first_name['und'][0]['value'])
                ? $account->field_first_name['und'][0]['value']
                : 'student') . ',',
        'link' => l('Use Study Sauce to improve your grades', '<front>', array(
            'absolute' => true,
            'attributes' => array('style' => 'color:#FF8900;')))));

    $system = drupal_mail_system('studysauce', 'inactivity');

    $message['result'] = $system->mail($message);

    // save inactivity date so we don't send this more than once every two weeks
    $edit = array();
    $edit['field_inactivity_sent']['und'][0]['value'] = date('Y-m-d H:i:s', time());
    user_save($account, $edit);
}
