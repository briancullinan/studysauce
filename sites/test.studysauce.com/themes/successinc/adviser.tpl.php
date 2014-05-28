<?php
if(!isset($account))
{
    global $user;
    $account = user_load($user->uid);
}
?>
<h2><?php print $account->field_first_name['und'][0]['value']; ?> <?php print $account->field_last_name['und'][0]['value']; ?></h2>
<div id="metrics">
    <?php print theme('studysauce-metrics', array('account' => $account)); ?>
</div>

<div id="goals">
    <?php print theme('studysauce-goals', array('account' => $account)); ?>
</div>