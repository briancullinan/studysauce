<?php
drupal_add_css(drupal_get_path('theme', 'successinc') .'/adviser.css');
if(!isset($account))
{
    global $user;
    $account = user_load($user->uid);
}
?>
<div class="user-name">
    <div>
        <h2>
            <?php print $account->field_first_name['und'][0]['value']; ?> <?php print $account->field_last_name['und'][0]['value']; ?>
            <span><a href="mailto:<?php print $account->mail; ?>">(Email)</a></span>
        </h2>
        <ul>
            <li><a href="#metrics">Metrics</a></li>
            <li><a href="#goals">Goals</a></li>
            <li><a href="#deadlines">Deadlines</a></li>
            <li><a href="#uploads">Uploads</a></li>
            <li><a href="#plan">Plan</a></li>
        </ul>
        <p style="line-height: 1px; font-size: 0; margin-bottom: 0;clear:both;">&nbsp;</p>
    </div>
</div>

<div id="metrics">
    <?php print theme('studysauce-adviser-metrics', array('account' => $account)); ?>
</div>

<div id="goals">
    <?php print theme('studysauce-adviser-goals', array('account' => $account)); ?>
</div>

<div id="deadlines">
    <?php print theme('studysauce-adviser-deadlines', array('account' => $account)); ?>
</div>

<div id="uploads">
    <?php print theme('studysauce-adviser-uploads', array('account' => $account)); ?>
</div>

<div id="plan">
    <?php print theme('studysauce-adviser-plans', array('account' => $account)); ?>
</div>

