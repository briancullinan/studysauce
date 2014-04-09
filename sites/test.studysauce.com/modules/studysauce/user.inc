<?php

function studysauce_save_user()
{
    global $user;
    $user = user_load($user->uid);
    $changes = array();
    // if parent selected jump down below, everything else assume student
    if(substr($user->mail, -strlen('@internal.example.org')) != '@internal.example.org')
        if(!isset($_POST['parent-student']) || $_POST['parent-student'] != 'parent')
        {
            $msg = <<< EOHTML
Congratulations on taking the first step to becoming a great studier.  To get the most out of Study Sauce, we recommend a few key things to do.

1. Enter your important deadlines and we will send you reminders to stay on track.

2. Check in when you study and we will guide you through the best study techniques.

3. Set up study goals and rewards. Parents are often happy to sponsor them.

As always, we are happy to help with any questions that you might have. Just email us at admin@studysauce.com.
EOHTML;

            $body = theme('studysauce-email', array(
                'message' => implode('<br />', explode("\n", $msg)),
                'greeting' => 'Dear ' . (isset($user->field_first_name[LANGUAGE_NONE][0]['value']) && isset($user->field_last_name[LANGUAGE_NONE][0]['value'])
                        ? $user->field_first_name[LANGUAGE_NONE][0]['value'] . ' ' . $user->field_last_name[LANGUAGE_NONE][0]['value']
                        : 'student') . ','));

            $new_message = drupal_mail('studysauce', 'welcome', $user->mail, language_default(), array(), variable_get('site_mail', ''), FALSE);

            $new_message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
            $new_message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('welcome-student'))));
            $new_message['subject'] = 'Welcome to Study Sauce';
            $new_message['body'] = str_replace('admin@studysauce.com', '<a href="mailto:admin@studysauce.com" style="color:#FF9900;">admin@studysauce.com</a>', $body);

            $system = drupal_mail_system('studysauce', 'welcome');
            $new_message['result'] = $system->mail($new_message);

            $changes['field_parent_student']['und'][0]['value'] = 'student';
        }
        else
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
                'greeting' => 'Dear ' . (isset($user->field_first_name[LANGUAGE_NONE][0]['value']) && isset($user->field_last_name[LANGUAGE_NONE][0]['value'])
                        ? $user->field_first_name[LANGUAGE_NONE][0]['value'] . ' ' . $user->field_last_name[LANGUAGE_NONE][0]['value']
                        : 'parent') . ','));

            $new_message = drupal_mail('studysauce', 'welcome', $user->mail, language_default(), array(), variable_get('site_mail', ''), FALSE);

            $new_message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
            $new_message['headers']['X-SMTPAPI'] = preg_replace('/(.{1,72})(\s)/i', "\1\n   ", json_encode(array('category' => array('welcome-parent'))));
            $new_message['subject'] = 'Welcome to Study Sauce';
            $new_message['body'] = str_replace('admin@studysauce.com', '<a href="mailto:admin@studysauce.com" style="color:#FF9900;">admin@studysauce.com</a>', $body);

            $system = drupal_mail_system('studysauce', 'welcome');
            $new_message['result'] = $system->mail($new_message);

            $changes['field_parent_student']['und'][0]['value'] = 'parent';
        }

    user_save($user, $changes);
}


