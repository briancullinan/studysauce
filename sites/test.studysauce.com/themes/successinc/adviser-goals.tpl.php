<?php
drupal_add_css(drupal_get_path('theme', 'successinc') .'/goals.css');
drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/goals.js');
$studyConnections = studysauce_get_connections();
?>
<div id="fb-root"></div>
<script>
    (function (d, s, id) {
        var js, fjs = d.getElementsByTagName(s)[0];
        if (d.getElementById(id)) return;
        js = d.createElement(s);
        js.id = id;
        js.src = "//connect.facebook.net/en_US/all.js";
        fjs.parentNode.insertBefore(js, fjs);
    }(document, 'script', 'facebook-jssdk'));
</script>
<?php
if(!isset($account))
{
    global $user;
    $account = user_load($user->uid);
}
$setup = studysauce_is_incentives_setup($account);
?>

<h2>Study goals</h2>

<?php list($b, $m, $o) = _studysauce_unsponsored_goals($account); ?>
<?php if(!isset($b->item_id)): ?>
    <h3>Your student has not completed this section yet.</h3>
<?php endif;
if(isset($b->item_id)): ?>
<div
    class="step_<?php print $setup; ?> <?php print (empty($studyConnections) ? 'not-connected' : 'connected'); ?> <?php print (isset($parent) ? 'sponsored' : ''); ?>">
    <div id="non-sponsored">
        <div class="row draggable odd <?php print (isset($b->item_id) ? ('gid' . $b->item_id) : ''); ?> <?php print (!isset($b->field_hours['und'][0]['value']) ? 'edit unsaved' : ''); ?>">
            <div class="field-name-field-type"><strong>Study Hours</strong></div>
            <div class="field-type-list-integer field-name-field-hours field-widget-options-select form-wrapper">
                <div class="read-only"><label>Goal: </label>
                    <span><?php print (isset($b->field_hours['und'][0]['value']) ? $b->field_hours['und'][0]['value'] : 0); ?></span>

                    <div class="description"><span>hours per week</span><span>hrs / wk</span></div>
                </div>
                <div class="form-item form-type-select">
                    <label>Goal: </label>
                    <select name="goal-hours" class="form-select">
                        <option value="_none">- None -</option>
                        <option
                            value="30" <?php print (isset($b->field_hours['und'][0]['value']) && $b->field_hours['und'][0]['value'] == 30 ? 'selected="selected"' : ''); ?>>
                            30
                        </option>
                        <option
                            value="25" <?php print (isset($b->field_hours['und'][0]['value']) && $b->field_hours['und'][0]['value'] == 25 ? 'selected="selected"' : ''); ?>>
                            25
                        </option>
                        <option
                            value="20" <?php print (isset($b->field_hours['und'][0]['value']) && $b->field_hours['und'][0]['value'] == 20 ? 'selected="selected"' : ''); ?>>
                            20
                        </option>
                        <option
                            value="15" <?php print (isset($b->field_hours['und'][0]['value']) && $b->field_hours['und'][0]['value'] == 15 ? 'selected="selected"' : ''); ?>>
                            15
                        </option>
                        <option
                            value="10" <?php print (isset($b->field_hours['und'][0]['value']) && $b->field_hours['und'][0]['value'] == 10 ? 'selected="selected"' : ''); ?>>
                            10
                        </option>
                        <option
                            value="5" <?php print (isset($b->field_hours['und'][0]['value']) && $b->field_hours['und'][0]['value'] == 5 ? 'selected="selected"' : ''); ?>>
                            5
                        </option>
                    </select>

                    <div class="description"><span>hours per week</span><span>hrs / wk</span></div>
                </div>
            </div>
            <div class="field-type-text-long field-name-field-reward field-widget-text-textarea form-wrapper">
                <div class="read-only"><label>Reward: </label>
                    <span><?php print (isset($b->field_reward['und'][0]['value']) ? $b->field_reward['und'][0]['value'] : ''); ?></span>
                </div>
                <div class="form-item form-type-textarea">
                    <label>Reward: </label>
                    <div class="form-textarea-wrapper resizable textarea-processed resizable-textarea">
                        <textarea placeholder="Ex. $25 gift card"
                                  name="goal-reward" cols="60" rows="2"
                                  class="form-textarea jquery_placeholder-processed"><?php print (isset($b->field_reward['und'][0]['value']) ? $b->field_reward['und'][0]['value'] : ''); ?></textarea>
                    </div>
                </div>
            </div>
        </div>
        <?php if(isset($b->field_hours['und'][0]['value']) && isset($m->field_grade['und'][0]['value'])) : ?>
            <div class="row draggable even <?php print (isset($m->item_id) ? ('gid' . $m->item_id) : ''); ?>">
                <div class="field-name-field-type"><strong>Study Milestone</strong></div>
                <div class="field-type-list-text field-name-field-grade field-widget-options-select form-wrapper">
                    <div class="read-only">
                        <label>Goal: </label>
                        <span><?php print (isset($m->field_grade['und'][0]['value']) ? $m->field_grade['und'][0]['value'] : ''); ?></span>

                        <div class="description"><span>grade on exam/paper</span><span>on exam/paper</span></div>
                    </div>
                    <div class="form-item form-type-select">
                        <label>Goal: </label>
                        <select name="goal-grade" class="form-select">
                            <option value="_none">- None -</option>
                            <option
                                value="A" <?php print (isset($m->field_grade['und'][0]['value']) && $m->field_grade['und'][0]['value'] == 'A' ? 'selected="selected"' : ''); ?>>
                                A
                            </option>
                            <option
                                value="A-" <?php print (isset($m->field_grade['und'][0]['value']) && $m->field_grade['und'][0]['value'] == 'A-' ? 'selected="selected"' : ''); ?>>
                                A-
                            </option>
                            <option
                                value="B+" <?php print (isset($m->field_grade['und'][0]['value']) && $m->field_grade['und'][0]['value'] == 'B+' ? 'selected="selected"' : ''); ?>>
                                B+
                            </option>
                            <option
                                value="B" <?php print (isset($m->field_grade['und'][0]['value']) && $m->field_grade['und'][0]['value'] == 'B' ? 'selected="selected"' : ''); ?>>
                                B
                            </option>
                            <option
                                value="B-" <?php print (isset($m->field_grade['und'][0]['value']) && $m->field_grade['und'][0]['value'] == 'B-' ? 'selected="selected"' : ''); ?>>
                                B-
                            </option>
                            <option
                                value="C+" <?php print (isset($m->field_grade['und'][0]['value']) && $m->field_grade['und'][0]['value'] == 'C+' ? 'selected="selected"' : ''); ?>>
                                C+
                            </option>
                            <option
                                value="C" <?php print (isset($m->field_grade['und'][0]['value']) && $m->field_grade['und'][0]['value'] == 'A' ? 'selected="selected"' : ''); ?>>
                                C
                            </option>
                        </select>

                        <div class="description"><span>grade on exam/paper</span><span>on exam/paper</span></div>
                    </div>
                </div>
                <div class="field-type-text-long field-name-field-reward field-widget-text-textarea form-wrapper">
                    <div class="read-only"><label>Reward: </label>
                        <span><?php print (isset($m->field_reward['und'][0]['value']) ? $m->field_reward['und'][0]['value'] : ''); ?></span>
                    </div>
                    <div class="form-item form-type-textarea">
                        <label>Reward: </label>
                        <div class="form-textarea-wrapper resizable textarea-processed resizable-textarea">
                            <textarea placeholder="Ex. $50 gift card"
                                      name="goal-reward" cols="60" rows="2"
                                      class="form-textarea jquery_placeholder-processed"><?php print (isset($m->field_reward['und'][0]['value']) ? $m->field_reward['und'][0]['value'] : ''); ?></textarea>
                        </div>
                    </div>
                </div>
            </div>
        <?php
        endif;
        if(isset($m->field_grade['und'][0]['value']) && isset($o->field_gpa['und'][0]['value'])) : ?>
            <div class="row draggable odd <?php print (isset($o->item_id) ? ('gid' . $o->item_id) : ''); ?>">
                <div class="field-name-field-type"><strong>Study Outcome</strong></div>
                <div class="field-type-list-float field-name-field-gpa field-widget-options-select form-wrapper">
                    <div class="read-only"><label>Goal: </label>
                        <span><?php print (isset($o->field_gpa['und'][0]['value']) ? $o->field_gpa['und'][0]['value'] : ''); ?></span>

                        <div class="description">Target GPA for the term</div>
                    </div>
                    <div class="form-item form-type-select">
                        <label>Goal: </label>
                        <select name="goal-gpa" class="form-select">
                            <option value="_none">- None -</option>
                            <option
                                value="4" <?php print (isset($o->field_gpa['und'][0]['value']) && $o->field_gpa['und'][0]['value'] == '4' ? 'selected="selected"' : ''); ?>>
                                4.00
                            </option>
                            <option
                                value="3.75" <?php print (isset($o->field_gpa['und'][0]['value']) && $o->field_gpa['und'][0]['value'] == '3.75' ? 'selected="selected"' : ''); ?>>
                                3.75
                            </option>
                            <option
                                value="3.5" <?php print (isset($o->field_gpa['und'][0]['value']) && $o->field_gpa['und'][0]['value'] == '3.5' ? 'selected="selected"' : ''); ?>>
                                3.50
                            </option>
                            <option
                                value="3.25" <?php print (isset($o->field_gpa['und'][0]['value']) && $o->field_gpa['und'][0]['value'] == '3.25' ? 'selected="selected"' : ''); ?>>
                                3.25
                            </option>
                            <option
                                value="3" <?php print (isset($o->field_gpa['und'][0]['value']) && $o->field_gpa['und'][0]['value'] == '3' ? 'selected="selected"' : ''); ?>>
                                3.00
                            </option>
                            <option
                                value="2.75" <?php print (isset($o->field_gpa['und'][0]['value']) && $o->field_gpa['und'][0]['value'] == '2.75' ? 'selected="selected"' : ''); ?>>
                                2.75
                            </option>
                            <option
                                value="2.5" <?php print (isset($o->field_gpa['und'][0]['value']) && $o->field_gpa['und'][0]['value'] == '2.5' ? 'selected="selected"' : ''); ?>>
                                2.50
                            </option>
                            <option
                                value="2.25" <?php print (isset($o->field_gpa['und'][0]['value']) && $o->field_gpa['und'][0]['value'] == '2.25' ? 'selected="selected"' : ''); ?>>
                                2.25
                            </option>
                            <option
                                value="2" <?php print (isset($o->field_gpa['und'][0]['value']) && $o->field_gpa['und'][0]['value'] == '2' ? 'selected="selected"' : ''); ?>>
                                2.00
                            </option>
                            <option
                                value="1.75" <?php print (isset($o->field_gpa['und'][0]['value']) && $o->field_gpa['und'][0]['value'] == '1.75' ? 'selected="selected"' : ''); ?>>
                                1.75
                            </option>
                            <option
                                value="1.5" <?php print (isset($o->field_gpa['und'][0]['value']) && $o->field_gpa['und'][0]['value'] == '1.5' ? 'selected="selected"' : ''); ?>>
                                1.50
                            </option>
                            <option
                                value="1.25" <?php print (isset($o->field_gpa['und'][0]['value']) && $o->field_gpa['und'][0]['value'] == '1.25' ? 'selected="selected"' : ''); ?>>
                                1.25
                            </option>
                            <option
                                value="1" <?php print (isset($o->field_gpa['und'][0]['value']) && $o->field_gpa['und'][0]['value'] == '1' ? 'selected="selected"' : ''); ?>>
                                1.00
                            </option>
                        </select>

                        <div class="description">Target GPA for the term</div>
                    </div>
                </div>
                <div class="field-type-text-long field-name-field-reward field-widget-text-textarea form-wrapper">
                    <div class="read-only">
                        <label>Reward: </label>
                        <span><?php print (isset($o->field_reward['und'][0]['value']) ? $o->field_reward['und'][0]['value'] : ''); ?></span>
                    </div>
                    <div class="form-item form-type-textarea">
                        <label>Reward: </label>

                        <div class="form-textarea-wrapper resizable textarea-processed resizable-textarea">
                            <textarea placeholder="Ex. Fancy dinner" name="goal-reward" cols="60" rows="2"
                                      class="form-textarea jquery_placeholder-processed"><?php print (isset($o->field_reward['und'][0]['value']) ? $o->field_reward['und'][0]['value'] : ''); ?></textarea>
                        </div>
                    </div>
                </div>
            </div>
        <?php endif; ?>
    </div>
    <div id="achievements">
        <?php
        // TODO: make this a theme or something
        print _studysauce_get_achievements($account);
        ?>
    </div>
    <div id="read-more-incentives">
        <img src="/<?php print drupal_get_path('theme', 'successinc'); ?>/images/science.png"/>

        <h3>The Science of Setting Goals</h3>
        <a href="#read-more"
           onclick="jQuery('.page-dashboard #goals #read-more-incentives .grid_6').toggle(); return false;">read
            more</a>
    </div>
</div>
<?php endif; ?>
<hr style="margin-top:20px;" />
