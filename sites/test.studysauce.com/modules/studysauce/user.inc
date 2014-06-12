<?php

function studysauce_group_save()
{

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


function studysauce_save_user($user = null, &$edit = array())
{
    if(!isset($edit))
        $edit = array();

    if($user == null)
    {
        global $user;
        $user = user_load($user->uid);
    }

    if(isset($_POST['cancel']))
    {
        $edit = array('user_cancel_notify' => false);
        user_cancel($edit, $user->uid, 'user_cancel_block');
        return;
    }

    if(isset($_POST['first']) && isset($_POST['last']) && isset($_POST['email']))
    {
        $oldEmail = false;
        $edit['field_first_name']['und'][0]['value'] = $_POST['first'];
        $edit['field_last_name']['und'][0]['value'] = $_POST['last'];
        if(strcmp($user->mail, $_POST['email']) <> 0)
        {
            $edit['mail'] = $_POST['email'];
            $oldEmail = $user->mail;
        }
        if(isset($_POST['pass']) && !empty($_POST['pass']))
            $edit['pass'] = $_POST['pass'];

        user_save($user, $edit);

        if($oldEmail)
        {
            // find nodes whose title matches the old e-mail
            $nodes = db_select('node', 'n')
                ->fields('n', array('nid'))
                ->condition('title', $oldEmail, '=')
                ->condition(db_or()->condition('uid', $user->uid, '='))
                ->execute()
                ->fetchAllAssoc('nid');
            foreach($nodes as $nid => $n)
            {
                $node = node_load($nid);
                $node->title = $edit['mail'];
                node_save($node);
            }

            // change accountability partner emails
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

        user_save($user, $edit);
    }

    // if parent selected jump down below, everything else assume student
    if(substr($user->mail, -strlen('@internal.example.org')) != '@internal.example.org')
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
                'greeting' => 'Dear ' . (isset($user->field_first_name[LANGUAGE_NONE][0]['value'])
                        ? $user->field_first_name[LANGUAGE_NONE][0]['value']
                        : 'student') . ','));

            $new_message = drupal_mail('studysauce', 'welcome', $user->mail, language_default(), array(), variable_get('site_mail', ''), FALSE);

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

        }

}


