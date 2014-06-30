<?php
global $user;

drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/userlist.js');
drupal_add_css(drupal_get_path('theme', 'successinc') .'/adviser-userlist.css');
?>
<div id="select-status">
    <a href="#green"><span>&nbsp;</span></a>
    <a href="#yellow"><span>&nbsp;</span></a>
    <a href="#red"><span>&nbsp;</span></a></div>
<table>
    <thead>
        <tr>
            <th>Status</th>
            <th>Date</th>
            <th>Student</th>
            <th>School</th>
        </tr>
    </thead>
    <tbody>
<?php
$users = array();
$groups = og_get_groups_by_user();
if(isset($groups['node']))
{
    $query = db_select('og_membership', 'ogm');
    $query->fields('ogm', array('entity_type', 'etid'));
    $query->condition('ogm.gid', array_keys($groups['node']), 'IN');
    $result = $query->execute();
    $members = $result->fetchAll();
    // if there are any advisers as members, load their groups and members as well
    //    we only do this recursively one time
    foreach($members as $i => $member)
    {
        $m = user_load($member->etid);
        if(!in_array('adviser', $m->roles) && $m->uid != $user->uid)
            $users[$m->uid] = $m;
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
if(isset($partners['field_collection_item']) && !empty($partners['field_collection_item']))
{
    $partners = array_keys($partners['field_collection_item']);
    foreach($partners as $p)
    {
        $partner = entity_load('field_collection_item', array($p));
        $host = $partner[$p]->hostEntity();
        if($host)
            $users[$host->uid] = $host;
    }
}

// sort users in descending order by creation date, so the newest user is always on top
$created = array_map(function ($u) { return $u->created; }, $users);
array_multisort($created, SORT_NUMERIC, SORT_DESC, $users);

// loop through each member and get the previous login dates
$accessed = array();
foreach($users as $i => $m)
{
    // do not include adviser or self in userlist
    if(in_array('adviser', $m->roles) || $m->uid == $user->uid)
        continue;

    $accesses = db_select('accesslog', 'l')
        ->fields('l', array('timestamp'))
        ->condition('uid', $m->uid, '=')
        ->groupBy('CONCAT(DAY(DATE(FROM_UNIXTIME(l.timestamp))),MONTH(DATE(FROM_UNIXTIME(l.timestamp))),YEAR(DATE(FROM_UNIXTIME(l.timestamp))))')
        ->orderBy('l.timestamp', 'DESC')
        ->execute()
        ->fetchAllAssoc('timestamp');
    $count = 0;
    foreach($accesses as $t => $r)
    {
        $accessed[] = array($t, $m);
        if($count > 10)
            break;
        else
            $count++;
    }
    // add user to list because they have never logged in
    if($count == 0)
        $accessed[] = array(0, $m);
}

$dates = array_map(function ($x) { return $x[0]; } , $accessed);
array_multisort($dates, SORT_NUMERIC, SORT_DESC, $accessed);

foreach($accessed as $i => $a)
{
    list($t, $m) = $a;
    print '<tr class="uid' . $m->uid . ' status_' . (!isset($m->field_adviser_status['und'][0]['value']) || $m->field_adviser_status['und'][0]['value'] == 'green'
            ? 'green'
            : $m->field_adviser_status['und'][0]['value']) . '">';
    print '<td><a href="#change-status"><span>&nbsp;</span></a></td>';
    print '<td>' . ($t == 0 ? 'N/A' : date('d-M', $t)) . '</td>';
    print '<td><a href="#uid-' . $m->uid . '">' . $m->field_first_name['und'][0]['value'] . ' ' . $m->field_last_name['und'][0]['value'] . '</a></td>';
    $query = new EntityFieldQuery();
    $nodes = $query->entityCondition('entity_type', 'node')
        ->propertyCondition('type', 'schedule')
        ->propertyCondition('title', isset($m->mail) ? $m->mail : '')
        ->propertyCondition('status', 1)
        ->range(0, 1)
        ->execute();
    if (!empty($nodes['node']))
    {
        $nodes = array_keys($nodes['node']);
        $nid = array_shift($nodes);
        $node = node_load($nid);
        print '<td>' . (isset($node->field_university['und'][0]['value']) && !empty($node->field_university['und'][0]['value']) ? $node->field_university['und'][0]['value'] : 'Not set') . '</td>';
    }
    else
        print '<td>Not set</td>';

    print '</tr>';
}
?>
    </tbody>
</table>

<?php
if(!isset($user->field_partner_advice['und'][0]['value']) || !$user->field_partner_advice['und'][0]['value'])
{
    $account = user_load($user->uid);
    $edit = array();
    $edit['field_partner_advice']['und'][0]['value'] = true;
    user_save($account, $edit);
    ?><div id="first-time-messages"><?php
    print theme('studysauce-partner-instructions', array('account' => array_shift($users)));
    ?></div><?php
}