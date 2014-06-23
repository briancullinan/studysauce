<?php


function studysauce_3_day_invite($invite_user, $invite_entity)
{
    $message = drupal_mail('studysauce', 'invite', $invite_entity->field_email['und'][0]['value'], language_default(), array(), $invite_user->mail, FALSE);

    $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
    $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('3-day-invite-students'))));
    $message['subject'] = 'Your invitation' . (isset($invite_user->field_first_name['und'][0]['value'])
            ? (' from ' . $invite_user->field_first_name['und'][0]['value'])
            : '') . ' to join Study Sauce is still pending';

    $message['body'] = theme('studysauce-email', array(
        'message' => implode('<br />', explode("\n", 'We started Study Sauce based on the realization that no one ever teaches students the most effective study methods.  We thought that was crazy when considering the fact that students spend up to 75% of their time outside the classroom.

        We have carefully researched the most effective study techniques and have incorporated the leading science into our online service to teach our students how to employ the best study methods.  Come discover the secret sauce to studying and find out how other students like you are raising their GPAs.')),
        'greeting' => 'Dear ' . (isset($invite_entity->field_first_name['und'][0]['value']) && isset($invite_entity->field_last_name['und'][0]['value'])
                ? $invite_entity->field_first_name['und'][0]['value'] . ' ' . $invite_entity->field_last_name['und'][0]['value']
                : 'student') . ',',
        'link' => l('Study Sauce', 'students', array(
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
    list($reminder) = $reminders;
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

    $message = drupal_mail('studysauce', 'reminder', $account->mail, language_default(), array(), null, FALSE);

    $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
    $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('reminders'))));
    $message['subject'] = 'You have a notification for ' . $reminder->field_class_name['und'][0]['value'];

    $message['body'] = theme('studysauce-email', array(
        'footer' => '<h2 style="font-size:18px;text-align:center;color: #555555;margin-top:100px;">Take your studying to the next level!</h2><p style="text-align:center;font-family: \'Ubuntu\',Helvetica Neue,Arial,sans-serif;color: #555555;"><a href="' . url('buy', array('absolute' => true)) . '" style="font-size:18px;font-weight:700;padding:12px 25px;border-radius:4px;border-bottom: 3px solid #004477;background: #0099be;display:inline-block;margin-top:5px;color:#FFFFFF;text-decoration:none;">Buy a personalized study plan</a></p>',
        'message' => 'Below is your reminder.<br /><br /><br /><strong>Subject:</strong><br /><span style="height:24px;width:24px;background-color:' . $color . ';display:inline-block;border-radius:100%;border: 3px solid #555555;">&nbsp;</span> ' . $reminder->field_class_name['und'][0]['value'] . '<br /><br /><strong>Assignment:</strong><br />' . (isset($reminder->field_assignment['und'][0]['value']) ? $reminder->field_assignment['und'][0]['value'] : '') . '<br /><br /><strong>Days until due date:</strong><br />' . $days . '<br />',
        'greeting' => 'Hi ' . (isset($account->field_first_name['und'][0]['value'])
                ? $account->field_first_name['und'][0]['value']
                : 'student') . ',',
        'link' => l('Click here to log in to Study Sauce and edit your deadlines', '<front>', array(
            'fragment' => 'dates',
            'absolute' => true,
            'attributes' => array('style' => 'color:#FF8900;')))));

    $system = drupal_mail_system('studysauce', 'reminder');

    $message['result'] = $system->mail($message);

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

function studysauce_inactivity($account)
{
    $message = drupal_mail('studysauce', 'inactivity', $account->mail, language_default(), array(), null, FALSE);

    $message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
    $message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('inactivity'))));
    $message['subject'] = 'Improve your grades with Study Sauce';

    $message['body'] = theme('studysauce-email', array(
        'message' => 'We haven\'t seen you in a while.  By using Study Sauce when you study, you can apply the very best study methods to make sure you do your best in every class.<br /><br />Additionally, your advisor, [advisor fname] [advisor lname], will be viewing the data from Study Sauce for your periodic check ins.<br /><br />If you have any questions, comments, or feedback, we would love to hear them.  Just reply to this email.',
        'greeting' => 'Hello ' . (isset($account->field_first_name['und'][0]['value'])
                ? $account->field_first_name['und'][0]['value']
                : 'student') . ',',
        'link' => l('Use Study Sauce to improve your grades', '<front>', array(
            'fragment' => 'dates',
            'absolute' => true,
            'attributes' => array('style' => 'color:#FF8900;')))));

    $system = drupal_mail_system('studysauce', 'inactivity');

    $message['result'] = $system->mail($message);
}

function studysauce_counselor_invite()
{

}
