<?php
drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/userlist.js');
?>
<h2>Select a user to view their details</h2>
<table>
    <thead>
        <tr>
            <td>Status</td>
            <td>Date</td>
            <td>Student</td>
            <td>School</td>
        </tr>
    </thead>
    <tbody>
<?php
$query = db_select("og_membership", "ogm");
$query->condition("ogm.gid", 624, "=");
$query->fields("ogm", array("entity_type", "etid"));
$result = $query->execute();
$members = $result->fetchAll();
foreach($members as $i => $member)
{
    print '<tr>';
    $m = user_load($member->etid);
    print '<td>&nbsp;</td>';
    print '<td>' . date('d-M', $m->access) . '</td>';
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