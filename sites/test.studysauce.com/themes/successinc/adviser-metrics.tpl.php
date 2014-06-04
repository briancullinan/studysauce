<?php
drupal_add_css(drupal_get_path('theme', 'successinc') .'/metrics.css');
drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/metrics.js');

if(!isset($account))
{
    global $user;
    $account = user_load($user->uid);
}

$classesNames = _studysauce_get_schedule_classes($account);
list($times, $rows, $total, $hours) = _studysauce_get_metrics($account);

if(empty($times)):
    ?>
    <script type="text/javascript">
        jQuery(document).ready(function () {
            jQuery('#metrics').addClass('empty');
        });
    </script>
<? endif; ?>
<div id="metrics-empty">
    <img src="<?php print drupal_get_path('theme', 'successinc'); ?>/images/metrics-sample.png" width="100%" />
    <div class="middle-wrapper">
        <a href="#checkin" onclick="jQuery('#checkin').scrollintoview();"><h2>Check in to start tracking your study hours</h2></a>
    </div>
</div>
<h2>Study metrics</h2>
<div class="centrify">
    <div id="timeline">
        <h3>Study hours by week</h3>
        <h4 style="margin:5px 0; color:#555;"><?php print ($hours > 0 ? ('Goal: ' . $hours . ' hours') : '&nbsp;'); ?></h4>
    </div>
    <div id="pie-chart">
        <h3>Study hours by class</h3>
        <h4 style="margin:5px 0; color:#555;">Total study hours: <strong id="study-total"><?php print $total; ?></strong></h4>
    </div>
</div>
<hr/>
<script type="text/javascript">
    window.initialHistory = <?php print json_encode($times); ?>;
    window.classNames = <?php print json_encode(array_values($classesNames)); ?>;
</script>
<div id="checkins-list">
    <div class="heading row">
        <label class="class-name">Class</label>
        <label class="class-date"><span class="full-only">Check in date</span><span class="mobile-only">Date</span></label>
        <label class="class-time">Duration</label>
    </div>
    <?php print $rows; ?>
</div>
<hr/>
