<h2>Enter important dates and we will send you reminders</h2>
<div id="schedule" class="students-only">
    <?php
    $form = studysauce_build_schedule_form();
    $i = 0;
    $classes = array();
    do
    {
        if(!empty($form['field_classes']['und'][$i]['field_class_name']['und'][0]['value']['#value']))
            $classes[] = $form['field_classes']['und'][$i]['field_class_name']['und'][0]['value']['#value'];
        $i++;
    } while (isset($form['field_classes']['und'][$i]));

    print drupal_render($form);
    ?>
</div>
<?php
$form = _studysauce_get_dates_form();
$empty = true;
if(isset($form['field_class_names']['und'][0]))
    foreach($form['field_class_names']['und'] as $i => $val)
        if(isset($val['value']['#value']) && !empty($val['value']['#value']))
            $empty = false;
print drupal_render($form);
if($empty) :
?>
<script type="text/javascript">
    jQuery(document).ready(function () {
        jQuery('#dates,#checkin').addClass('empty edit-schedule');
    });
</script>
<?php endif; ?>
<p style="clear: both;margin:0;"><a href="#edit-schedule"><span>Edit schedule</span></a></p>