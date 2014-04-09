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

function studysauce_form_incentive_node_form_alter(&$form, &$form_state)
{
    global $user, $parentIncentive;
    if(!isset($parentIncentive))
        $parentIncentive = studysauce_get_shared_goals();

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
    if(isset($form_state['build_info']['args'][1]) && $form_state['build_info']['args'][1] == 'parent-sponsored')
        $form['actions']['submit']['#id'] = drupal_html_id('submit-parent-incentive');
    else
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
                $form['field_goals']['und'][$i]['field_reward']['#suffix'] = '<div class="form-actions highlighted-link"><a class="more form-submit" href="#save-incentive">Save</a>';

            if($setup == 3 && isset($invites[0]->mail) &&
                $user->field_parent_student['und'][0]['value'] == 'student')
                $form['field_goals']['und'][$i]['field_reward']['#suffix'] .= '<a class="brag more read-only" href="#claim" onclick="jQuery(this).parents(\'tr\').addClass(\'achievement-only\'); jQuery(\'#incentives\').scrollintoview(); return false;">Brag</a></div>';
            else
                $form['field_goals']['und'][$i]['field_reward']['#suffix'] .= '</div>';

            $i++;
        } while(isset($form['field_goals']['und'][$i]));

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

