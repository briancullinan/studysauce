<?php
global $classes;
list($times, $rows, $total, $hours) = _studysauce_get_metrics();
?>
<hr />
<div id="metrics" class="<?php print count($times) > 0 ? '' : 'empty'; ?>">
    <div id="metrics-empty">
        <img src="<?php print drupal_get_path('theme', 'successinc'); ?>/images/metrics-sample.png" width="100%" />
        <a href="#checkin" onclick="jQuery('#checkin').scrollintoview();"><h2>Check in to start tracking your study hours</h2></a>
    </div>
    <h2>Study metrics</h2>
    <div id="timeline">
        <h3>Study hours by week</h3>
        <h4 style="margin:5px 0; color:#555;"><?php print $hours > 0 ? ('Goal: ' . $hours . ' hours') : '&nbsp;'; ?></h4>
    </div>
    <div id="pie-chart">
        <h3>Study hours by class</h3>
        <h4 style="margin:5px 0; color:#555;">Total study hours: <strong id="study-total"><?php print $total; ?></strong></h4>
    </div>
    <script type="text/javascript">
        window.initialHistory = <?php print json_encode($times); ?>;
        window.classNames = <?php print json_encode($classes); ?>;
    </script>
    <div id="checkins-list">
    <div class="heading row">
        <label class="class-name">Class</label>
        <label class="class-date"><span class="full-only">Check in date</span><span class="mobile-only">Date</span></label>
        <label class="class-time">Duration</label>
    </div>
    <?php print $rows; ?>
    </div>
</div>
