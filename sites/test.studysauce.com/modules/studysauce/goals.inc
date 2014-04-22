<?php


/**
 * Plupload's upload function.
 */
function studysauce_goals_upload_page() {

    drupal_add_http_header('Expires', 'Mon, 26 Jul 1997 05:00:00 GMT', TRUE);
    drupal_add_http_header('Last-Modified', gmdate("D, d M Y H:i:s") . ' GMT', TRUE);
    drupal_add_http_header('Cache-Control', 'no-store, no-cache, must-revalidate post-check=0, pre-check=0', TRUE);
    drupal_add_http_header('Pragma', 'no-cache', TRUE);

    // Settings
    $targetDir = $GLOBALS['conf']['file_temporary_path'] .'/';
    // 5 minutes execution time
    @set_time_limit(5 * 60);
    // Uncomment this one to fake upload time
    // usleep(5000);

    // Get parameters
    $chunk = isset($_REQUEST['chunk']) ? $_REQUEST['chunk'] : 0;
    $chunks = isset($_REQUEST['chunks']) ? $_REQUEST['chunks'] : 0;
    $filename = isset($_REQUEST['name']) ? $_REQUEST['name'] : '';

    // Clean the fileName for security reasons
    $extensions = 'jpg jpeg gif png txt doc xls pdf ppt pps odt ods odp *';
    $filename = file_munge_filename($filename, $extensions, FALSE);

    // Make sure the fileName is unique but only if chunking is disabled
    if ($chunks < 2 && file_exists($targetDir . DIRECTORY_SEPARATOR . $filename)) {
        $ext = strrpos($filename, '.');
        $filename_a = substr($filename, 0, $ext);
        $filename_b = substr($filename, $ext);

        $count = 1;
        while (file_exists($targetDir . DIRECTORY_SEPARATOR . $filename_a . '_' . $count . $filename_b))
            $count++;

        $filename = $filename_a . '_' . $count . $filename_b;
    }


    // Look for the content type header
    if (isset($_SERVER['HTTP_CONTENT_TYPE'])) {
        $contentType = $_SERVER['HTTP_CONTENT_TYPE'];
    }

    if (isset($_SERVER['CONTENT_TYPE'])) {
        $contentType = $_SERVER['CONTENT_TYPE'];
    }

    // Handle non multipart uploads older WebKit versions didn't support multipart in HTML5
    if (strpos($contentType, 'multipart') !== false) {
        if (isset($_FILES['file']['tmp_name']) && is_uploaded_file($_FILES['file']['tmp_name'])) {
            // Open temp file
            $out = fopen($targetDir . DIRECTORY_SEPARATOR . $filename, $chunk == 0 ? 'wb' : 'ab');
            if ($out) {
                // Read binary input stream and append it to temp file
                $in = fopen($_FILES['file']['tmp_name'], 'rb');

                if ($in) {
                    while ($buff = fread($in, 4096))
                        fwrite($out, $buff);
                } else {
                    die('{"jsonrpc" : "2.0", "error" : {"code": 101, "message": "Failed to open input stream."}, "id" : "id"}');
                }

                fclose($in);
                fclose($out);
                @unlink($_FILES['file']['tmp_name']);
            } else {
                die('{"jsonrpc" : "2.0", "error" : {"code": 102, "message": "Failed to open output stream."}, "id" : "id"}');
            }
        } else {
            die('{"jsonrpc" : "2.0", "error" : {"code": 103, "message": "Failed to move uploaded file."}, "id" : "id"}');
        }
    } else {
        // Open temp file
        $out = fopen($targetDir . DIRECTORY_SEPARATOR . $filename, $chunk == 0 ? 'wb' : 'ab');
        if ($out) {
            // Read binary input stream and append it to temp file
            $in = fopen("php://input", 'rb');

            if ($in) {
                while ($buff = fread($in, 4096)) {
                    fwrite($out, $buff);
                }
            } else {
                die('{"jsonrpc" : "2.0", "error" : {"code": 101, "message": "Failed to open input stream."}, "id" : "id"}');
            }

            fclose($in);
            fclose($out);
        } else {
            die('{"jsonrpc" : "2.0", "error" : {"code": 102, "message": "Failed to open output stream."}, "id" : "id"}');
        }
    }

    if ((isset($_GET['chunk']) && ($_GET['chunk'] + 1) == $_GET['chunks']) || (!isset($_GET['chunk']))) {
        $source = 'temporary://'. $filename;
        $file = new stdClass();
        $file->uid      = $GLOBALS['user']->uid;
        $file->status   = 0;
        $file->filename = $filename;
        $file->uri      = $source;
        $file->filemime = file_get_mimetype($file->filename);
        $file->filesize = filesize($source);
        // Save url argument(instance_id) so we can validate file by instance settings
        $file->arg = arg(1);

        $e = FALSE;
        // Here we validate file
        studysauce_plupload_file_validation($file, $e);

        if ($e === FALSE) {
            $return = file_save($file);
            // Generate a secure version of file uri for temporary delivery.
            $file->secure_uri = image_style_url('achievement', $file->uri);
            // We build it as a temp but will deliver it as a public.
            $file->secure_uri = str_replace('system/temporary', variable_get('file_public_path', 'sites/default/files'), $file->secure_uri);
        }
        else {
            file_unmanaged_delete($file->uri);
            $return = (object) array('error_message' => $e);
        }

        drupal_json_output($return);
    }
}


/**
 * Default validation function.
 * In most cases Pluplaod will be used for images so we use image-specific validation.
 * Non-image files has no support right now.
 */
function studysauce_plupload_file_validation(&$file, &$e) {
    if (!isset($file->arg) || (isset($file->arg) && is_numeric($file->arg))) {
        // Check if file is image
        $passImage = file_validate_is_image($file);
        if (!empty($passImage)) {
            $e = reset($passImage);
            return;
        }

        // Check if file has allowed extension
        $passExt = file_validate_extensions($file, 'png gif jpg jpeg');
        if (!empty($passExt)) {
            $e = reset($passExt);
            return;
        }

        // Check if file doesn't exceed the maximal allowed size
        $passSize = file_validate_size($file, parse_size(''), 0);
        if (!empty($passSize)) {
            $e = reset($passSize);
            return;
        }

        // Check if file name isn't too long
        $passLength = file_validate_name_length($file);
        if (!empty($passLength)) {
            $e = reset($passLength);
            return;
        }

        // Check if file meet the resolution restrictions
        $passRes = file_validate_image_resolution($file, '', '');
        if (!empty($passRes)) {
            $e = reset($passRes);
            return;
        }
    }
}

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
            foreach ($node->field_goals[LANGUAGE_NONE] as $i => $goal)
            {
                $eid = $goal['value'];
                $entity = entity_load('field_collection_item', array($eid));
                if(($entity[$eid]->field_type[LANGUAGE_NONE][0]['value'] == 'behavior' && $_POST['type'] == 'hours') ||
                    ($entity[$eid]->field_type[LANGUAGE_NONE][0]['value'] == 'milestone' && $_POST['type'] == 'grade') ||
                    ($entity[$eid]->field_type[LANGUAGE_NONE][0]['value'] == 'outcome' && $_POST['type'] == 'gpa'))
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

    if(isset($_POST['claim']) && isset($_POST['uploads']) && isset($_POST['message']))
    {
        // get the entity we are claiming
        if (is_array($node->field_goals[LANGUAGE_NONE]))
        {
            foreach ($node->field_goals[LANGUAGE_NONE] as $i => $goal)
            {
                if($goal['value'] == $_POST['claim'])
                {
                    $eid = $goal['value'];
                    $entity = entity_load('field_collection_item', array($eid));
                    // create a new achievement entity
                    $entity[$eid]->revision = 1;
                    $entity[$eid]->field_photo_evidence['und'][0]['fid'] = $_POST['uploads'][0]['fid'];
                    $entity[$eid]->field_message['und'][0]['value'] = $_POST['message'];
                    $entity[$eid]->save();

                    break;
                }
            }
        }

        foreach ($_POST['uploads'] AS $i => $upload)
        {

            break;
                if ($items && $items[0]['fid'] > 0) {
                    $field = field_info_field($instance['field_name']);
                    $destination = file_field_widget_uri($field, $instance);
                    foreach ($items AS $delta => $item) {
                        $file = file_load($item['fid']);
                        // Filefield paths integration
                        if (module_exists('filefield_paths') && isset($item['rename'])) {
                            $file->origname = $item['rename'];
                            file_save($file);
                        } else {
                            $fileName = isset($item['rename']) ? $item['rename'] : $file->filename;
                            $dir = isset($instance['settings']['file_directory']) ? $instance['settings']['file_directory'] .'/' : '';
                            $location = token_replace($field['settings']['uri_scheme'] .'://'. $dir);
                            $filePath = $location . $fileName;
                            if ($file->uri !== $filePath) {
                                file_prepare_directory($location, FILE_CREATE_DIRECTORY);
                                file_move($file, $filePath);
                            }
                        }
                    }
                }
        }

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
            $commands[] = ajax_command_invoke('.page-dashboard #goals .pane-content > div', 'addClass', array('done'));
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
                'query' => array('achievement' => $last['code'], 'destination' => '#goals'),
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
                'query' => array('reward' => $GLOBALS['rewards_code'], 'destination' => '#goals'),
                'absolute' => true,
                'attributes' => array('style' => 'color:#FF9900;')))));

        $system = drupal_mail_system('studysauce', 'reward-setup');

        $message['result'] = $system->mail($message);
    }

    list($awards) = studysauce_get_awards();
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
            if(isset($goal->field_achievements[LANGUAGE_NONE][0]))
                foreach($goal->field_achievements[LANGUAGE_NONE] as $j => $achievement)
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
    $goals = _studysauce_unsponsored_goals();
    $i = 0;
    foreach($goals as $i => $goal)
    {
        if(isset($goal->field_achievements[LANGUAGE_NONE][0]))
            foreach($goal->field_achievements[LANGUAGE_NONE] as $j => $achievement)
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
    }

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


