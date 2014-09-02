<?php
drupal_add_css(drupal_get_path('theme', 'successinc') .'/adviser-import.css');
drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/import.js');

// get a list of users just like the userlist tab
$users = array();
$groups = og_get_groups_by_user();
$adviserGroups = array();
if (isset($groups['node'])) {
    $query = db_select('og_membership', 'ogm');
    $query->fields('ogm', array('entity_type', 'etid', 'gid'));
    $query->condition('ogm.gid', array_keys($groups['node']), 'IN');
    $result = $query->execute();
    $members = $result->fetchAll();
    // if there are any advisers as members, load their groups and members as well
    //    we only do this recursively one time
    foreach ($members as $i => $member) {
        $m = user_load($member->etid);
        if ((in_array('adviser', $m->roles) || in_array('master adviser', $m->roles)) && $m->uid != $user->uid) {
            // only set master adviser if there are no other advisers for this group
            if (!isset($adviserGroups[$member->gid]) || in_array('adviser', $m->roles))
                $adviserGroups[$member->gid] = $m;
        }
    }

    foreach ($members as $i => $member) {
        $m = user_load($member->etid);

        if (!in_array('adviser', $m->roles) && !in_array('master adviser', $m->roles) && $m->uid != $user->uid)
        {
            if (isset($adviserGroups[$member->gid]))
                $m->otherAdviser = $adviserGroups[$member->gid];
            $users[$m->uid] = $m;
        }
        /*{
            $subgroups = og_get_groups_by_user($m);
            $query = db_select('og_membership', 'ogm');
            $query->fields('ogm', array('entity_type', 'etid'));
            $query->condition('ogm.gid', array_keys($subgroups['node']), 'IN');
            $result = $query->execute();
            $submembers = $result->fetchAll();
            foreach($submembers as $j => $submember)
            {
                $m = user_load($submember->etid);
                $users[$m->uid] = $m;
            }
        }
        else*/
    }
}


// add partners to members list
$partnerQuery = new EntityFieldQuery();
$partners = $partnerQuery->entityCondition('entity_type', 'field_collection_item')
    ->propertyCondition('field_name', 'field_partners')
    ->fieldCondition('field_email', 'value', $user->mail)
    ->execute();
if (isset($partners['field_collection_item']) && !empty($partners['field_collection_item'])) {
    $partners = entity_load('field_collection_item', array_keys($partners['field_collection_item']));
    foreach ($partners as $p) {
        $host = $p->hostEntity();
        if ($host)
            $users[$host->uid] = $host;
    }
}

$emails = array_map(function ($x) {return $x->mail;}, $users);

?>
<h2>Invite students to Study Sauce</h2>
<h3>1) Enter their first name, last name, and email below to invite them to Study Sauce.</h3>
<h3>2) Your students will receive an invitation with a link that will finish setting up their account.</h3>
<h3>3) Voila, you are connected.</h3>
<hr style="margin:30px 0; padding:0;" />
<?php

$account = user_load($user->uid);
$invites = array();
if(isset($account->field_invites[LANGUAGE_NONE]) && is_array($account->field_invites[LANGUAGE_NONE]))
    $invites = entity_load('field_collection_item', array_map(function ($x) { return $x['value']; }, $account->field_invites[LANGUAGE_NONE]));

$dupes = array();
if(!empty($invites))
{
    $first = true;
    foreach ($invites as $i => $invite)
    {

        // skip blank invites
        if(!isset($invite->field_email['und'][0]['value']) || empty($invite->field_email['und'][0]['value']) ||
            !isset($invite->field_sent['und'][0]['value']) || empty($invite->field_sent['und'][0]['value']))
            continue;

        // skip users who have signed up
        //if(in_array($invite->field_email['und'][0]['value'], $emails))
        //    continue;

        // skip duplicate invites because they can be invited more than once
        $dupe = $invite->field_email['und'][0]['value'] . (isset($invite->field_first_name['und'][0]['value']) ? $invite->field_first_name['und'][0]['value'] : '') . (isset($invite->field_last_name['und'][0]['value']) ? $invite->field_last_name['und'][0]['value'] : '');
        if(in_array($dupe, $dupes))
            continue;
        $dupes[] = $dupe;

        $t = strtotime($invite->field_sent['und'][0]['value']);
        ?>
        <div class="row <?php print ($first ? 'first' : ''); ?>">
            <div class="field-name-field-first-name">
                <label>First name</label>
                <div class="read-only"><?php print (isset($invite->field_first_name['und'][0]['value']) ? $invite->field_first_name['und'][0]['value'] : ''); ?></div>
            </div>
            <div class="field-name-field-last-name">
                <label>Last name</label>
                <div class="read-only"><?php print (isset($invite->field_last_name['und'][0]['value']) ? $invite->field_last_name['und'][0]['value'] : ''); ?></div>
            </div>
            <div class="field-name-field-email">
                <label>Email</label>
                <div class="read-only"><?php print $invite->field_email['und'][0]['value']; ?></div>
            </div>
        </div>
<?php

        $first = false;
    }
}

?>
<div id="add-user-row" class="row edit invalid">
    <div class="field-name-field-first-name">
        <label>First name</label>
        <input class="text-full form-text" type="text" placeholder="First name" />
    </div>
    <div class="field-name-field-last-name">
        <label>Last name</label>
        <input class="text-full form-text" type="text" placeholder="Last name" />
    </div>
    <div class="field-name-field-email">
        <label>Email</label>
        <input class="text-full form-text" type="text" placeholder="Email" />
    </div>
</div>
<p class="highlighted-link">
    <a href="#add-user" class="field-add-more-submit ajax-processed" name="field_reminders_add_more">
        Add <span>+</span> user
    </a>
    <a href="#save-group" class="more">Import</a>
</p>
<hr style="margin:30px 0; padding:0;" />
<h2>Use our batch uploader</h2>
<textarea id="user-import" rows="4" placeholder="first,last,email&para;first,last,email"></textarea>
<p class="highlighted-link" style="margin-bottom:0;margin-top:0px;">
    <a href="#import-group" class="more">Import batch</a>
</p>
<fieldset id="user-preview" style="min-height: 100px">
    <legend>Preview</legend>
</fieldset>

