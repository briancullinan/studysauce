<?php

function _studysauce_get_short_class_name($className)
{
    $classNameParts = explode(' ', $className);
    if(count($classNameParts) == 1 && strlen($classNameParts[0]) <= 6)
        $classPart = $classNameParts[0];
    else
        $classPart = substr(strtoupper($classNameParts[0]), 0, 3) . (strpos(strtolower($className), 'lab') !== false ? 'L' : '') . '<br />' . substr(strtoupper(end($classNameParts)), -3, 3);
    return $classPart;
}

function studysauce_form_schedule_node_form_alter(&$form, &$form_state)
{
    global $user;
    $call_names = array('Chem', 'Math', 'Phys', 'Econ', 'Hist');
    $curYir = date("Y");
    $holidays = array();
    $MLK = strtotime("january $curYir third monday");
    if($MLK >= time())
        array_push($holidays, $MLK);
    $PD = strtotime("february $curYir third monday"); //presidents day
    if($PD >= time())
        array_push($holidays, $PD);
    $Est = easter_date($curYir); // easter
    if($Est >= time())
        array_push($holidays, $Est);
    $MDay = strtotime("may $curYir first monday"); // memorial day
    if($MDay >= time())
        array_push($holidays, $MDay);
    $LD = strtotime("september $curYir first monday");  //labor day
    if($LD >= time())
        array_push($holidays, $LD);
    $CD = strtotime("october $curYir third monday"); //columbus day
    if($CD >= time())
        array_push($holidays, $CD);
    $TH = strtotime("november $curYir first thursday"); // thanks giving
    if($TH >= time())
        array_push($holidays, $TH);

    if($user->uid > 0)
        $user = user_load($user->uid);
    $form['actions']['submit']['#id'] = 'submit-schedule';
    $form['field_classes']['und']['add_more']['#value'] = 'Add another class';

    $form['field_holidays']['und']['add_more']['#value'] = 'Add another holiday';
    if(arg(2) != 'edit')
        hide($form['field_holidays']);

    if(isset($user->mail) and !empty($user->mail))
    {
        $form['title']['#default_value'] = $user->mail;
        $form['title']['#type'] = 'hidden';
    }
    elseif(isset($_SESSION['studysauce']['email']))
    {
        $form['title']['#default_value'] = $_SESSION['studysauce']['email'];
        $form['title']['#type'] = 'hidden';
    }
    elseif(isset($_GET['email']) && !empty($_GET['email']) && $user->uid == 0)
    {
        $form['title']['#default_value'] = $_GET['email'];
        $form['title']['#type'] = 'hidden';
    }
    elseif(!empty($form['title']['#value']) || !empty($form['title']['#default_value']))
        $form['title']['#type'] = 'hidden';

    if(isset($_SESSION['studysauce']['first']))
    {
        $form['field_first_name']['und'][0]['value']['#default_value'] = $_SESSION['studysauce']['first'];
        $form['field_first_name']['und'][0]['value']['#type'] = 'hidden';
    }
    elseif(isset($_GET['first']) && !empty($_GET['first']))
    {
        $form['field_first_name']['und'][0]['value']['#default_value'] = $_GET['first'];
        $form['field_first_name']['und'][0]['value']['#type'] = 'hidden';
    }
    elseif(!empty($form['field_first_name']['und'][0]['value']['#value']) || !empty($form['field_first_name']['und'][0]['value']['#default_value']))
        $form['field_first_name']['und'][0]['value']['#type'] = 'hidden';
    elseif(isset($user->field_first_name['und'][0]['value']))
    {
        $form['field_first_name']['und'][0]['value']['#default_value'] = $user->field_first_name['und'][0]['value'];
        $form['field_first_name']['und'][0]['value']['#type'] = 'hidden';
    }

    if(isset($_SESSION['studysauce']['last']))
    {
        $form['field_last_name']['und'][0]['value']['#default_value'] = $_SESSION['studysauce']['last'];
        $form['field_last_name']['und'][0]['value']['#type'] = 'hidden';
    }
    elseif(isset($_GET['last']) && !empty($_GET['last']))
    {
        $form['field_last_name']['und'][0]['value']['#default_value'] = $_GET['last'];
        $form['field_last_name']['und'][0]['value']['#type'] = 'hidden';
    }
    elseif(!empty($form['field_last_name']['und'][0]['value']['#value']) || !empty($form['field_last_name']['und'][0]['value']['#default_value']))
        $form['field_last_name']['und'][0]['value']['#type'] = 'hidden';
    elseif(isset($user->field_last_name['und'][0]['value']))
    {
        $form['field_last_name']['und'][0]['value']['#default_value'] = $user->field_last_name['und'][0]['value'];
        $form['field_last_name']['und'][0]['value']['#type'] = 'hidden';
    }

    //if(!empty($form['field_university']['und'][0]['value']['#value']) || !empty($form['field_university']['und'][0]['value']['#default_value']))
    //    $form['field_university']['und'][0]['value']['#type'] = 'hidden';

    $i = 0;
    do
    {
        // set placeholder and class name changes
        $form['field_classes']['und'][$i]['field_class_name']['und'][0]['value']['#attributes']['autocomplete'] = 'off';
        $form['field_classes']['und'][$i]['field_class_name']['und'][0]['value']['#attributes']['placeholder'] = $call_names[floor(rand(0, 4))] . ' 10' . ($i + 1);
        $form['field_classes']['und'][$i]['field_class_name']['und'][0]['value']['#attributes']['onkeyup'] = 'var startDate = jQuery(this).parents(".field-type-text").parent().find(".start-date-wrapper .form-type-textfield:first-child input"); if(startDate.val() == "" && startDate.attr("placeholder") != "Start") startDate.val(startDate.attr("placeholder"));var endDate = jQuery(this).parents(".field-type-text").parent().find(".end-date-wrapper .form-type-textfield:first-child input"); if(endDate.val() == "" && endDate.attr("placeholder") != "End") endDate.val(endDate.attr("placeholder"));';

        // hide the type
        if(arg(2) != 'edit')
            $form['field_classes']['und'][$i]['field_event_type']['#type'] = 'hidden';

    } while (isset($form['field_classes']['und'][++$i]));

    $i = 0;
    do
    {
        if(isset($holidays[$i]))
            $form['field_holidays']['und'][$i]['#default_value']['value'] = $holidays[$i];
    } while (isset($form['field_holidays']['und'][++$i]));

    if(drupal_is_front_page() || (isset($form_state['build_info']['args'][1]) && $form_state['build_info']['args'][1] == 'with_ajax'))
    {
        $form['#groups']['group_schedule']->label = 'Enter your class information to set up reminders for key dates and to start checking in.';
        $form['actions']['submit']['#ajax'] = array(
            'callback' => '_studysauce_submit_schedule_ajax',
        );
        $form['field_classes']['und']['add_more']['#ajax']['callback'] = '_studysauce_submit_schedule_ajax_add_items';

        $form['actions']['#suffix'] = '<p style="clear:both;margin-bottom:0;line-height:0px;">&nbsp;</p><a href="#" onclick="jQuery(\'#schedule form\')[0].reset(); jQuery(\'#dates\').removeClass(\'edit-schedule\').scrollintoview(); return false;" class="fancy-close">&nbsp;</a>';

        hide($form['field_complete']);
        hide($form['field_grades']);
        hide($form['field_weekends']);
        hide($form['field_plan_notes']);
        hide($form['field_university']);
        // TODO: figure out why this is so hard
        $form['field_university']['#limit_validation_errors'] = array();
        $form['field_university'][LANGUAGE_NONE]['#limit_validation_errors'] = array();
        $form['field_university'][LANGUAGE_NONE][0]['#limit_validation_errors'] = array();
        $form['field_university'][LANGUAGE_NONE][0]['value']['#limit_validation_errors'] = array();
        $form['field_university'][LANGUAGE_NONE][0]['value']['#required'] = 0;
        $form['field_university'][LANGUAGE_NONE][0]['#required'] = 0;
        $form['field_university'][LANGUAGE_NONE]['#required'] = 0;
        $form['field_university']['#required'] = 0;

        field_group_hide_field_groups($form, array('group_study'));
        field_group_hide_field_groups($form, array('group_periods'));

        $i = 0;
        do
        {
            hide($form['field_classes']['und'][$i]['field_locations']);
            hide($form['field_classes']['und'][$i]['remove_button']);
            hide($form['field_classes']['und'][$i]['field_checklist']);
            hide($form['field_classes']['und'][$i]['field_checkin']);
            hide($form['field_classes']['und'][$i]['field_utc_checkin']);
            hide($form['field_classes']['und'][$i]['field_day_of_the_week']);
            hide($form['field_classes']['und'][$i]['field_time']);
            $form_state['values']['field_classes']['und'][$i]['field_time']['#limit_validation_errors'] = array();
        } while (isset($form['field_classes']['und'][++$i]));
    }
    else
    {
        array_unshift($form['#validate'], '_studysauce_classes_validate');
        $i = 0;
        $form['field_classes']['und']['add_more']['#attributes']['style'] = 'margin-top:50px;clear:both;';
        $form['actions']['submit']['#submit'][] = 'studysauce_schedule_submit';
    }

}

function studysauce_form_key_dates_node_form_alter(&$form, &$form_state)
{
    $call_names = array('Chem', 'Math', 'Phys', 'Econ', 'Hist');

    global $user;

    $classes = _studysauce_get_schedule_classes();

    $form['#validate'][] = '_studysauce_dates_validate';
    $form['actions']['#attributes']['class'][] = 'highlighted-link';
    $form['actions']['submit']['#attributes']['class'][] = 'more';
    $form['actions']['submit']['#value'] = 'Save';
    $form['actions']['submit']['#ajax'] = array(
        'callback' => '_studysauce_submit_dates_ajax',
    );

    $form['title']['#default_value'] = $user->mail;
    $form['title']['#type'] = 'hidden';

    if($form['field_class_names']['und']['#max_delta'] > 0)
        unset($form['field_class_names']['und'][$form['field_class_names']['und']['#max_delta']]);

    $last = $form['field_reminders']['und']['#max_delta'];
    if(empty($form['nid']['#value']))
        $form['field_reminders']['und'][$last]['is_new'] = array('#type' => 'markup', '#markup' => '');

    $i = 0;
    do
    {
        $form['field_class_names']['und'][$i]['value']['#attributes']['autocomplete'] = 'off';
        $form['field_class_names']['und'][$i]['value']['#attributes']['placeholder'] = $call_names[floor(rand(0, 4))] . ' 10' . ($i + 1);
        if(isset($classes[$i]))
            $form['field_class_names']['und'][$i]['value']['#default_value'] = $classes[$i];
        else
            $form['field_class_names']['und'][$i]['value']['#default_value'] = '';
        //elseif(!empty($form['field_class_names']['und'][$i]['value']['#default_value']))
        //    $classes[$i] = $form['field_class_names']['und'][$i]['value']['#default_value'];
    } while (isset($form['field_class_names']['und'][++$i]));

    $form['field_reminders']['und']['add_more']['#weight'] = -100;
    $form['field_reminders']['und']['add_more']['#ajax']['callback'] = '_studysauce_submit_dates_ajax_add_items';
    $form['field_reminders']['und']['add_more']['#markup'] = 'Add <span>&nbsp;</span> new';
    $form['field_reminders']['und']['add_more']['#value'] = 'Add new';

    $form['field_class_names']['und']['add_more']['#suffix'] = '<div class="highlighted-link">
            <a href="#" class="more" onclick="jQuery(\'#dates\').find(\'input[value=&quot;Save&quot;]\').trigger(\'mousedown\'); return false;">Save</a></div>
            <p style="clear:both;margin-bottom:0;line-height:0px;">&nbsp;</p>
            <a href="#" onclick="jQuery(\'#dates\').removeClass(\'edit-schedule\').scrollintoview(); return false;" class="fancy-close">&nbsp;</a>';

    $i = 0;
    $classes[-1] = '_none';
    $classes[] = 'Nonacademic';
    ksort($classes);
    $classesText = array_combine($classes, $classes);
    $classesText['_none'] = '- Select a class -';
    do
    {

        $form['field_reminders']['und'][$i]['field_class_name']['und'][0]['value']['#type'] = 'select';
        $form['field_reminders']['und'][$i]['field_class_name']['und'][0]['value']['#options'] = $classesText;
        $form['field_reminders']['und'][$i]['field_class_name']['und'][0]['value']['#size'] = 1;

        $reminders = $form['field_reminders']['und'][$i]['field_reminder'][LANGUAGE_NONE]['#default_value'];

        if(!empty($form['field_reminders']['und'][$i]['field_due_date']['und'][0]['#default_value']['value']))
        {
            $time = strtotime($form['field_reminders']['und'][$i]['field_due_date']['und'][0]['#default_value']['value']);
            if($time < (floor(time() / 86400) - 1) * 86400)
            {
                foreach($form['field_reminders']['und'][$i]['field_reminder'][LANGUAGE_NONE]['#options'] as $t => $text)
                {
                    if(array_search($t, $reminders) !== false)
                    {
                        $form['field_reminders']['und'][$i]['field_reminder']['und'][$t]['#type'] = 'hidden';
                        $form['field_reminders']['und'][$i]['field_reminder']['und'][$t]['#value'] = $t;
                    }
                    else
                        hide($form['field_reminders']['und'][$i]['field_reminder']['und'][$t]);
                }
                $form['field_reminders']['und'][$i]['field_class_name']['und'][0]['value']['#type'] = 'hidden';
                $form['field_reminders']['und'][$i]['field_assignment']['und'][0]['value']['#type'] = 'hidden';
                $form['field_reminders']['und'][$i]['field_percent']['und'][0]['value']['#type'] = 'hidden';
                $form['field_reminders']['und'][$i]['is_hidden'] = array('#type' => 'markup', '#markup' => '');
            }
        }

        // get short class name
        $form['field_reminders']['und'][$i]['field_class_name'][LANGUAGE_NONE][0]['value']['#title'] = '';
        $className = $form['field_reminders']['und'][$i]['field_class_name'][LANGUAGE_NONE][0]['value']['#default_value'];
        $classI = array_search($className, $classes);
        if($classI == -1)
            $form['field_reminders']['und'][$i]['is_new'] = array('#type' => 'markup', '#markup' => '');
        if($className == 'Nonacademic')
            $classI = -1;
        //$classPart = _studysauce_get_short_class_name($className);

        $reminderText = '';
        foreach($form['field_reminders']['und'][$i]['field_reminder'][LANGUAGE_NONE]['#options'] as $t => $text)
            $reminderText .= '<span class="' . (array_search($t, $reminders) !== false ? 'checked' : 'unchecked') . '">' . $text . '</span>';

        $form['field_reminders']['und'][$i]['field_reminder'][LANGUAGE_NONE]['#prefix'] =
            '<div class="read-only"><label>Reminders</label>' . $reminderText . '</div>';

        $form['field_reminders']['und'][$i]['field_class_name'][LANGUAGE_NONE]['#prefix'] .= '<div class="read-only"><span class="class' . $classI . '">&nbsp;</span>' .
            $className . '</div>';

        $form['field_reminders']['und'][$i]['field_assignment'][LANGUAGE_NONE][0]['value']['#attributes']['placeholder'] = 'Paper, exam, project, etc.';
        $assignment = $form['field_reminders']['und'][$i]['field_assignment'][LANGUAGE_NONE][0]['value']['#default_value'];
        if(empty($assignment))
            $assignment = '&lt;enter a title&gt;';
        $form['field_reminders']['und'][$i]['field_assignment'][LANGUAGE_NONE]['#prefix'] .= '<div class="read-only"><label>Assignment</label>' .
             $assignment . '</div>';

        $form['field_reminders']['und'][$i]['field_percent'][LANGUAGE_NONE]['#prefix'] .= '<div class="read-only"><label>% of grade</label>' .
            $form['field_reminders']['und'][$i]['field_percent'][LANGUAGE_NONE][0]['value']['#default_value'] . '%</div>';

        hide($form['field_reminders']['und'][$i]['remove_button']);
        $form['field_reminders']['und'][$i]['field_completed'][LANGUAGE_NONE]['#prefix'] = '<div class="read-only">
            <a href="#edit-reminder" onclick="jQuery(this).parents(\'.row\').addClass(\'edit\').datesFunc(); return false;">&nbsp;</a>' .
            /*<a href="#remove-reminder" onclick="jQuery(this).parents(\'.row\').find(\'input[value=&quot;Remove&quot;]\').trigger(\'mousedown\'); return false;">&nbsp;</a>'*/ '</div>';
        $form['field_reminders']['und'][$i]['field_completed']['#suffix'] = '<p style="margin-bottom:0;" class="highlighted-link"><a href="#" onclick="if(!jQuery(this).parents(\'.row\').is(\'.invalid\')) { jQuery(\'#dates .node-key_dates-form\').find(\'input[value=&quot;Save&quot;]\').trigger(\'mousedown\'); window.lastRow = jQuery(this).parents(\'.row\').attr(\'class\').match(/row_(\d+)\b/)[1]; } return false;" class="more">Save</a></p>';

    } while (isset($form['field_reminders']['und'][++$i]));

}

function studysauce_form_incentive_node_form_alter(&$form, &$form_state)
{
    global $user, $parentIncentive;
    if(!isset($parentIncentive))
        $parentIncentive = studysauce_get_parent_incentive();

    if(isset($parentIncentive) && $parentIncentive !== false)
    {

        foreach($parentIncentive->field_goals[LANGUAGE_NONE] as $i => $goal)
        {
            $entity = entity_load('field_collection_item', array($goal['value']));
            if($entity[$goal['value']]->field_type[LANGUAGE_NONE][0]['value'] == 'milestone')
                $milestone = $entity[$goal['value']];
            if($entity[$goal['value']]->field_type[LANGUAGE_NONE][0]['value'] == 'outcome')
                $outcome = $entity[$goal['value']];
            if($entity[$goal['value']]->field_type[LANGUAGE_NONE][0]['value'] == 'behavior')
                $behavior = $entity[$goal['value']];
        }

    }
    // this applies to a student claiming a parent incentive and setting up their own incentives
    $form['title']['#type'] = 'hidden';
    hide($form['actions']['delete']);
    $form['actions']['submit']['#id'] = drupal_html_id('submit-incentive');
    $form['actions']['submit']['#name'] = $form['actions']['submit']['#id'];
    if(arg(2) != 'edit')
        $form['actions']['submit']['#ajax'] = array(
            'callback' => '_studysauce_submit_incentives_ajax',
        );
    $form['actions']['#suffix'] = '<p style="clear:both;margin:0;line-height:0;">&nbsp</p>';

    $i = 0;
    do
    {
        $j = 0;
        do
        {
            $form['field_goals'][LANGUAGE_NONE][$i]['field_message'][LANGUAGE_NONE][$j]['value']['#attributes']['placeholder'] = 'Message';
            $j++;
        } while(isset($form['field_goals']['und'][$i]['field_message'][LANGUAGE_NONE][$j]));

        $form['field_goals']['und'][$i]['field_other']['#attributes']['style'] = 'display:none;';
        $form['field_goals']['und'][$i]['field_hours']['#attributes']['style'] = 'display:none;';
        $form['field_goals']['und'][$i]['field_gpa']['#attributes']['style'] = 'display:none;';
        $form['field_goals']['und'][$i]['field_grade']['#attributes']['style'] = 'display:none;';

        if($i == 0)
        {
            $form['field_goals']['und'][$i]['field_type']['und']['#default_value'] = 'behavior';
            //$form['field_goals']['und'][$i]['tip'] = array('#type' => 'markup', '#markup' => '<a class="tooltip" title="Change is difficult.  These goals allow you to create manageable, short-term targets.">&nbsp;</a>');
            $form['field_goals']['und'][$i]['field_reward']['und'][0]['value']['#attributes'] = array('placeholder' => 'Ex. $25 gift card');
            $form['field_goals']['und'][$i]['field_hours']['#attributes']['style'] = '';
            $form['field_goals']['und'][$i]['field_type']['#markup'] = '<div class="field-name-field-type"><strong>Study Hours</strong></div>';
        }
        elseif($i == 1)
        {
            $form['field_goals']['und'][$i]['field_type']['und']['#default_value'] = 'milestone';
            //$form['field_goals']['und'][$i]['tip'] = array('#type' => 'markup', '#markup' => '<a class="tooltip" title="Achievements along the way allow you to maintain your momentum.">&nbsp;</a>');
            $form['field_goals']['und'][$i]['field_reward']['und'][0]['value']['#attributes'] = array('placeholder' => 'Ex. $50 gift card');
            $form['field_goals']['und'][$i]['field_grade']['#attributes']['style'] = '';
            $form['field_goals']['und'][$i]['field_type']['#markup'] = '<div class="field-name-field-type"><strong>Study Milestone</strong></div>';
        }
        elseif($i == 2)
        {
            $form['field_goals']['und'][$i]['field_type']['und']['#default_value'] = 'outcome';
            //$form['field_goals']['und'][$i]['tip'] = array('#type' => 'markup', '#markup' => '<a class="tooltip" title="Inspirational, long-term goals give you the ability to dream big and give you a greater sense of purpose.">&nbsp;</a>');
            $form['field_goals']['und'][$i]['field_reward']['und'][0]['value']['#attributes'] = array('placeholder' => 'Ex. Fancy dinner');
            $form['field_goals']['und'][$i]['field_gpa']['#attributes']['style'] = '';
            $form['field_goals']['und'][$i]['field_type']['#markup'] = '<div class="field-name-field-type"><strong>Study Outcome</strong></div>';
        }
        $form['field_goals']['und'][$i]['field_type']['#type'] = 'markup';
        $form['field_goals']['und'][$i]['field_type']['und']['#options']['_none'] = 'Other';
        //$form['field_goals']['und'][$i]['field_other']['und'][0]['value']['#attributes'] = array('placeholder' => 'Description');
        $form['field_goals']['und'][$i]['field_other']['und']['#title'] = 'Goal: ';
        //$form['field_goals']['und'][$i]['field_grades']['und'][0]['value']['#attributes'] = array('placeholder' => 'Grades');
        $form['field_goals']['und'][$i]['field_grade']['und']['#title'] = 'Goal: ';
        //$form['field_goals']['und'][$i]['field_hours']['und'][0]['value']['#attributes'] = array('placeholder' => 'Hours');
        $form['field_goals']['und'][$i]['field_hours']['und']['#title'] = 'Goal: ';
        //$form['field_goals']['und'][$i]['field_gpa']['und'][0]['value']['#attributes'] = array('placeholder' => 'GPA');
        $form['field_goals']['und'][$i]['field_gpa']['und']['#title'] = 'Goal: ';
        $form['field_goals']['und'][$i]['field_reward']['und'][0]['value']['#title'] = 'Reward: ';
        $submitId = $form['actions']['submit']['#id'];
        $onclick = <<<EOC
    var options = Drupal.ajax['$submitId'].options;
    Drupal.ajax['$submitId'].form.ajaxSubmit({
        data: options.data,
        dataType: options.dataType,
        type: options.type,
        url: options.url + '?achievement=true',
        beforeSend: options.beforeSend,
        beforeSerialize:options.beforeSerialize,
        beforeSubmit: options.beforeSubmit,
        complete: options.complete,
        success: options.success
    }); return false;
EOC;

        $form['field_goals'][LANGUAGE_NONE][$i]['field_message']['#suffix'] = '<div class="highlighted-link form-actions">
                        <a href="#done" class="more" onclick="' . $onclick . '">Send</a>
                    </div>
                    <p style="clear:both;margin-bottom:0;line-height:0px;">&nbsp;</p>
                    <a href="#" onclick="jQuery(this).parents(\'tr\').removeClass(\'achievement-only\').scrollintoview(); return false;" class="fancy-close">&nbsp;</a></div>';
        $form['field_goals'][LANGUAGE_NONE][$i]['field_photo_evidence'][LANGUAGE_NONE]['#plup_override']['filters'][0]->extensions .= ',images';
        //hide($form['field_goals'][LANGUAGE_NONE][$i]['field_achievements'][LANGUAGE_NONE]);
        hide($form['field_goals'][LANGUAGE_NONE][$i]['remove_button']);
        $i++;
    } while(isset($form['field_goals']['und'][$i]));

    $setup = studysauce_is_incentives_setup();

    if(isset($form_state['build_info']['args'][1]) && $form_state['build_info']['args'][1] == 'parent-sponsored')
    {
        $i = 0;
        do
        {
            $form['field_goals']['und'][$i]['field_photo_evidence'][LANGUAGE_NONE]['#title'] = '';
            if($form['field_goals']['und'][$i]['field_type'][LANGUAGE_NONE]['#default_value'] == 'behavior' &&
                !empty($form['field_goals']['und'][$i]['field_hours'][LANGUAGE_NONE]['#default_value']))
            {
                $form['field_goals']['und'][$i]['field_photo_evidence']['#prefix'] = '<div><h3>Send your sponsors a photo of yourself and remind them your are studying hard</h3>';
                $form['field_goals']['und'][$i]['field_type']['#markup'] .= '<div class="field-name-field-hours"><label>Goal: </label> ' . $form['field_goals']['und'][$i]['field_hours'][LANGUAGE_NONE]['#default_value'][0] . ' <div class="description">' . $form['field_goals']['und'][$i]['field_hours'][LANGUAGE_NONE]['#description'] . '</div></div>';
                // TODO: change this when we want to support more than 3 goals
                if($setup == 3 && $user->field_parent_student['und'][0]['value'] == 'parent')
                    $sponsor = '<a class="sponsor more" href="#sponsor' . $i . '" onclick="var container_id=jQuery(\'#incentives .field-name-field-type input[value=&quot;behavior&quot;]:checked\').parents(\'td\').addClass(\'transferred\').find(\'select:visible\').val(&quot;' . $form['field_goals']['und'][$i]['field_hours'][LANGUAGE_NONE]['#default_value'][0] . '&quot;); return false;">Sponsor this</a>';
            }
            elseif($form['field_goals']['und'][$i]['field_type'][LANGUAGE_NONE]['#default_value'] == 'milestone' &&
                !empty($form['field_goals']['und'][$i]['field_grade'][LANGUAGE_NONE]['#default_value']))
            {
                $form['field_goals']['und'][$i]['field_photo_evidence']['#prefix'] = '<div><h3>Send your sponsors a photo of yourself your graded paper to earn your reward</h3>';
                $form['field_goals']['und'][$i]['field_type']['#markup'] .= '<div class="field-name-field-grade"><label>Goal: </label> ' . $form['field_goals']['und'][$i]['field_grade'][LANGUAGE_NONE]['#default_value'][0] . ' <div class="description">' . $form['field_goals']['und'][$i]['field_grade'][LANGUAGE_NONE]['#description'] . '</div></div>';
                // TODO: change this when we want to support more than 3 goals
                if($setup == 3 && $user->field_parent_student['und'][0]['value'] == 'parent')
                    $sponsor = '<a class="sponsor more" href="#sponsor' . $i . '" onclick="var container_id=jQuery(\'#incentives .field-name-field-type input[value=&quot;milestone&quot;]:checked\').parents(\'td\').addClass(\'transferred\').find(\'select:visible\').val(&quot;' . $form['field_goals']['und'][$i]['field_grade'][LANGUAGE_NONE]['#default_value'][0] . '&quot;); return false;">Sponsor this</a>';
            }
            elseif($form['field_goals']['und'][$i]['field_type'][LANGUAGE_NONE]['#default_value'] == 'outcome' &&
                !empty($form['field_goals']['und'][$i]['field_gpa'][LANGUAGE_NONE]['#default_value']))
            {
                $form['field_goals']['und'][$i]['field_photo_evidence']['#prefix'] = '<div><h3>Send your parents a picture of yourself to show them you passed all your classes</h3>';
                $form['field_goals']['und'][$i]['field_type']['#markup'] .= '<div class="field-name-field-gpa"><label>Goal: </label> ' . number_format((float)$form['field_goals']['und'][$i]['field_gpa'][LANGUAGE_NONE]['#default_value'][0], 2, '.', '') . ' <div class="description">' . $form['field_goals']['und'][$i]['field_gpa'][LANGUAGE_NONE]['#description'] . '</div></div>';
                // TODO: change this when we want to support more than 3 goals
                if($setup == 3 && $user->field_parent_student['und'][0]['value'] == 'parent')
                    $sponsor = '<a class="sponsor more" href="#sponsor' . $i . '" onclick="var container_id=jQuery(\'#incentives .field-name-field-type input[value=&quot;outcome&quot;]:checked\').parents(\'td\').addClass(\'transferred\').find(\'select:visible\').val(&quot;' . $form['field_goals']['und'][$i]['field_gpa'][LANGUAGE_NONE]['#default_value'][0] . '&quot;); return false;">Sponsor this</a>';
            }
            $form['field_goals']['und'][$i]['field_type']['#markup'] .= '<div class="field-name-field-reward"><label>Reward: </label> ' . (empty($form['field_goals']['und'][$i]['field_reward'][LANGUAGE_NONE][0]['value']['#default_value']) ? 'No reward set.' : $form['field_goals']['und'][$i]['field_reward'][LANGUAGE_NONE][0]['value']['#default_value']) . '</div>';

            if($setup == 3 && $user->field_parent_student['und'][0]['value'] == 'student')
            {
                $form['field_goals']['und'][$i]['field_type']['#markup'] .= '<div class="form-actions highlighted-link"><a class="claim more" href="#claim' . $i . '" onclick="jQuery(this).parents(\'tr\').addClass(\'achievement-only\'); jQuery(\'#incentives\').scrollintoview(); return false;">Claim</a></div>';
            }
            else
            {
                $form['field_goals']['und'][$i]['field_type']['#markup'] .= '<div class="form-actions highlighted-link">' . (isset($sponsor) ? $sponsor : '') . '</div>';
                hide($form['field_goals']['und'][$i]['field_photo_evidence']);
                hide($form['field_goals']['und'][$i]['field_message']);
            }
            hide($form['field_goals']['und'][$i]['tip']);
            hide($form['field_goals']['und'][$i]['field_gpa']);
            hide($form['field_goals']['und'][$i]['field_grade']);
            hide($form['field_goals']['und'][$i]['field_hours']);
            hide($form['field_goals']['und'][$i]['field_reward']);
            hide($form['field_goals']['und'][$i]['field_other']);
            hide($form['field_goals']['und'][$i]['field_type'][LANGUAGE_NONE]);
            if(!isset($form['field_goals']['und'][$i]['field_read_only'][LANGUAGE_NONE]['#default_value']) ||
                $form['field_goals']['und'][$i]['field_read_only'][LANGUAGE_NONE]['#default_value'] == 0)
                hide($form['field_goals']['und'][$i]);

            // change the upload for images
            $i++;
        } while(isset($form['field_goals']['und'][$i]));

        hide($form['field_goals']['und']['add_more']);
    }
    else
    {
        $form['title']['#value'] = $user->mail;
        $form['actions']['#attributes']['class'][] = 'highlighted-link';
        $form['actions']['submit']['#attributes']['class'][] = 'more';

        $invites = studysauce_get_connections(true);
        $conn = studysauce_get_connections();

        $i = 0;
        do
        {
            $form['field_goals']['und'][$i]['field_photo_evidence'][LANGUAGE_NONE]['#title'] = '';
            $form['field_goals']['und'][$i]['field_reward']['#suffix'] = '';
            $form['field_goals']['und'][$i]['field_read_only'][LANGUAGE_NONE]['#type'] = 'hidden';
            if($form['field_goals']['und'][$i]['field_type'][LANGUAGE_NONE]['#default_value'] == 'behavior')
            {
                if(isset($behavior->field_hours[LANGUAGE_NONE][0]['value']) && empty($form['field_goals']['und'][$i]['field_hours'][LANGUAGE_NONE]['#default_value']))
                    $form['field_goals']['und'][$i]['field_hours'][LANGUAGE_NONE]['#default_value'][0] = $behavior->field_hours[LANGUAGE_NONE][0]['value'];
                if(isset($behavior->field_reward[LANGUAGE_NONE][0]['value']) && empty($form['field_goals']['und'][$i]['field_reward'][LANGUAGE_NONE][0]['value']['#default_value']))
                    $form['field_goals']['und'][$i]['field_reward'][LANGUAGE_NONE][0]['value']['#default_value'] = $behavior->field_reward[LANGUAGE_NONE][0]['value'];
                $form['field_goals']['und'][$i]['field_photo_evidence']['#prefix'] = '<div><h3>Send your sponsors a photo of yourself and remind them your are studying hard</h3>';
                if(isset($form['field_goals']['und'][$i]['field_hours'][LANGUAGE_NONE]['#default_value'][0]))
                    $form['field_goals']['und'][$i]['field_hours'][LANGUAGE_NONE]['#prefix'] = '<div class="read-only"><label>Goal: </label> <span>' .
                        $form['field_goals']['und'][$i]['field_hours'][LANGUAGE_NONE]['#default_value'][0] . '</span> <div class="description">' . $form['field_goals']['und'][$i]['field_hours'][LANGUAGE_NONE]['#description'] . '</div></div>';
            }
            elseif($form['field_goals']['und'][$i]['field_type'][LANGUAGE_NONE]['#default_value'] == 'milestone')
            {
                if(isset($milestone->field_grade[LANGUAGE_NONE][0]['value']) && empty($form['field_goals']['und'][$i]['field_grade'][LANGUAGE_NONE]['#default_value']))
                    $form['field_goals']['und'][$i]['field_grade'][LANGUAGE_NONE]['#default_value'][0] = $milestone->field_grade[LANGUAGE_NONE][0]['value'];
                if(isset($milestone->field_reward[LANGUAGE_NONE][0]['value']) && empty($form['field_goals']['und'][$i]['field_reward'][LANGUAGE_NONE][0]['value']['#default_value']))
                    $form['field_goals']['und'][$i]['field_reward'][LANGUAGE_NONE][0]['value']['#default_value'] = $milestone->field_reward[LANGUAGE_NONE][0]['value'];
                $form['field_goals']['und'][$i]['field_photo_evidence']['#prefix'] = '<div><h3>Send your sponsors a photo of yourself your graded paper to earn your reward</h3>';
                if(isset($form['field_goals']['und'][$i]['field_grade'][LANGUAGE_NONE]['#default_value'][0]))
                    $form['field_goals']['und'][$i]['field_grade'][LANGUAGE_NONE]['#prefix'] = '<div class="read-only"><label>Goal: </label> <span>' .
                        $form['field_goals']['und'][$i]['field_grade'][LANGUAGE_NONE]['#default_value'][0] . '</span> <div class="description">' . $form['field_goals']['und'][$i]['field_grade'][LANGUAGE_NONE]['#description'] . '</div></div>';
            }
            elseif($form['field_goals']['und'][$i]['field_type'][LANGUAGE_NONE]['#default_value'] == 'outcome')
            {
                if(isset($outcome->field_gpa[LANGUAGE_NONE][0]['value']) && empty($form['field_goals']['und'][$i]['field_gpa'][LANGUAGE_NONE]['#default_value']))
                    $form['field_goals']['und'][$i]['field_gpa'][LANGUAGE_NONE]['#default_value'][0] = $outcome->field_gpa[LANGUAGE_NONE][0]['value'];
                if(isset($outcome->field_reward[LANGUAGE_NONE][0]['value']) && empty($form['field_goals']['und'][$i]['field_reward'][LANGUAGE_NONE][0]['value']['#default_value']))
                    $form['field_goals']['und'][$i]['field_reward'][LANGUAGE_NONE][0]['value']['#default_value'] = $outcome->field_reward[LANGUAGE_NONE][0]['value'];
                $form['field_goals']['und'][$i]['field_photo_evidence']['#prefix'] = '<div><h3>Send your parents a picture of yourself to show them you passed all your classes</h3>';
                if(isset($form['field_goals']['und'][$i]['field_gpa'][LANGUAGE_NONE]['#default_value'][0]))
                    $form['field_goals']['und'][$i]['field_gpa'][LANGUAGE_NONE]['#prefix'] = '<div class="read-only"><label>Goal: </label> <span>' .
                        number_format((float)$form['field_goals']['und'][$i]['field_gpa'][LANGUAGE_NONE]['#default_value'][0], 2, '.', '') . '</span> <div class="description">' . $form['field_goals']['und'][$i]['field_gpa'][LANGUAGE_NONE]['#description'] . '</div></div>';
            }

            $form['field_goals']['und'][$i]['field_reward'][LANGUAGE_NONE]['#prefix'] .= '<div class="read-only"><label>Reward: </label> <a href="#edit-reward" onclick="jQuery(this).parents(\'tr\').find(\'.field-name-field-read-only input\').val(0); jQuery(this).parents(\'tr\').addClass(\'edit\').goalsFunc(); return false;">&nbsp;</a> <span>' .
                $form['field_goals']['und'][$i]['field_reward'][LANGUAGE_NONE][0]['value']['#default_value'] . '</span></div>';

            if($user->field_parent_student['und'][0]['value'] == 'parent')
                $form['field_goals']['und'][$i]['field_reward']['#suffix'] = '<div class="form-actions highlighted-link"><a class="more form-submit" href="#save" onclick="jQuery(this).parents(\'tr\').find(\'.field-name-field-read-only input\').val(1); jQuery(this).parents(\'form\').find(\'.form-actions .form-submit\').trigger(\'mousedown\'); if(jQuery(window).outerWidth(true) <= 963) { jQuery(this).parents(\'td\').find(\'.field-name-field-type\').scrollintoview({padding: {top:120,bottom:200,left:0,right:0}}); } return false;">Sponsor this</a>';
            else
                $form['field_goals']['und'][$i]['field_reward']['#suffix'] = '<div class="form-actions highlighted-link"><a class="more form-submit" href="#save" onclick="jQuery(this).parents(\'tr\').find(\'.field-name-field-read-only input\').val(1); jQuery(this).parents(\'form\').find(\'.form-actions .form-submit\').trigger(\'mousedown\'); if(jQuery(window).outerWidth(true) <= 963) { jQuery(this).parents(\'td\').find(\'.field-name-field-type\').scrollintoview({padding: {top:120,bottom:200,left:0,right:0}}); } return false;">Save</a>';

            if($setup == 3 && isset($invites[0]->mail) &&
                $user->field_parent_student['und'][0]['value'] == 'student')
                $form['field_goals']['und'][$i]['field_reward']['#suffix'] .= '<a class="brag more read-only" href="#claim" onclick="jQuery(this).parents(\'tr\').addClass(\'achievement-only\'); jQuery(\'#incentives\').scrollintoview(); return false;">Brag</a></div>';
            else
                $form['field_goals']['und'][$i]['field_reward']['#suffix'] .= '</div>';

            $i++;
        } while(isset($form['field_goals']['und'][$i]));

        if($user->field_parent_student['und'][0]['value'] == 'parent')
            $checkinLink = '<p style="clear:left;text-align: center;margin-top:20px;"><a href="#checkin" class="read-only">Your student will be guided by our study detection software every time they check in to study.</a></p>';
        else
            $checkinLink = '<p style="clear:left;text-align: center;margin-top:20px;"><a href="#checkin" class="read-only">Use the check-in tab to start reaching your goal.</a></p>';

        if($setup == 3 && !isset($conn[0]->mail))
            if($user->field_parent_student['und'][0]['value'] == 'parent')
            {
                $form['field_goals']['#suffix'] = '<div class="form-actions highlighted-link full-only"><a class="more read-only" href="#invite">Invite your student</a></div>';
                $form['field_goals']['#prefix'] = '<div class="form-actions highlighted-link mobile-only"><a class="more read-only" href="#invite">Invite your student</a></div>';
            }
            else
            {
                $form['field_goals']['#suffix'] = '<div class="form-actions highlighted-link full-only"><a class="more read-only" href="#invite">Get sponsored</a></div>';
                $form['field_goals']['#prefix'] = '<div class="form-actions highlighted-link mobile-only"><a class="more read-only" href="#invite">Get sponsored</a></div>';
            }

        $form['field_goals']['#suffix'] = (isset($form['field_goals']['#suffix']) ? $form['field_goals']['#suffix'] : '' ) . $checkinLink;

        if($setup == 1)
        {
            $form['actions']['submit']['#value'] = 'Save';
        }
        elseif($setup == 2)
        {
            $form['actions']['submit']['#value'] = 'Finish';
        }
        elseif($setup == 3)
        {
            $form['actions']['submit']['#value'] = 'Save';
        }
    }
    //if($i == 1)
    //    $form['field_goals']['und']['add_more']['#value'] = '+ Add Study Milestone';
    //elseif($i == 2)
    //    $form['field_goals']['und']['add_more']['#value'] = '+ Add Study Outcome';
    //else
    hide($form['field_goals']['und']['add_more']); // = '+ Add another goal';
    $form['field_goals']['und']['add_more']['#ajax']['callback'] = '_studysauce_submit_incentives_ajax_add_items';
}

