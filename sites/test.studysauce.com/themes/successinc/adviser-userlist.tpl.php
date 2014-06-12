<?php
drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/userlist.js');
drupal_add_css(drupal_get_path('theme', 'successinc') .'/adviser-userlist.css');
?>
<h2>Select a user to view their details</h2>
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
$groups = og_get_groups_by_user();
if(isset($groups['node']))
{
    $query = db_select("og_membership", "ogm");
    $query->condition("ogm.gid", array_keys($groups['node']), "=");
    $query->fields("ogm", array("entity_type", "etid"));
    $result = $query->execute();
    $members = $result->fetchAll();
    $accessed = array();
    foreach($members as $i => $member)
    {
        $m = user_load($member->etid);
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
            if($count > 5)
                break;
            else
                $count++;
        }
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
        print '<td>' . date('d-M', $t) . '</td>';
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
}
?>
    </tbody>
</table>