<?php
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
<?php global $user;
$setup = studysauce_is_incentives_setup();
?>
<div
    class="step_<?php print $setup; ?> <?php print (empty($studyConnections) ? 'not-connected' : 'connected'); ?> <?php print (isset($parent) ? 'sponsored' : ''); ?>">

<div id="student_step_1">
    <h2 class="students_only student_step_1">Set goals and rewards, then get sponsored by your parents</h2>

    <h2 class="parents_only student_step_1">Set goals and reward your student's study efforts</h2>

    <div class="grid_6 big-arrow">
        <h3>The Science</h3>
        <img src="/<?php print drupal_get_path('theme', 'successinc'); ?>/images/science.png"/>

        <p>According to the Incentive Theory of motivation, using rewards increases the likelihood of repeating
            the given activity. By incorporating this powerful psychological principle into study behavior,
            students can incentivize optimal study practices. Studies show that the sooner the reward is given,
            the stronger the positive association with the activity.</p>
    </div>
    <div class="grid_6">
        <h3 style="margin-bottom:5px;">The Application</h3>
        <span class="site-name"><strong>Study</strong> Sauce</span>

        <p>Study Sauce combines the best study practices with the incentive to change. Knowledge of the harm or
            benefit of something doesn’t necessarily result in behavioral change (just ask someone that has
            struggled to quit smoking). Creating meaningful study rewards dramatically increases the likelihood
            that a student will adopt effective study habits.</p>
    </div>
</div>

<?php /*


<div id="parent-sponsored">
    <?php
    if (isset($parent)) {
        print drupal_render($parent);
    } else {
    } ?>
</div>

 */ ?>

<div id="goals-brag" class="required-fields group-achievement field-group-div">
    <div><h3>Send your sponsors a photo of yourself and remind them your are studying hard</h3>

        <div class="field-type-image field-name-field-photo-evidence field-widget-image-plupload form-wrapper">
            <div class="form-item form-type-plupload-file">
                <div class="plupload" id="goals-plupload">
                    <div class="plup-list-wrapper">
                        <ul class="plup-list clearfix ui-sortable"></ul>
                    </div>
                    <div class="plup-filelist" id="goal-plupload-filelist">
                        <table>
                            <tbody>
                            <tr class="plup-drag-info">
                                <td>
                                    <div class="drag-main">Click here to select files</div>
                                    <div class="drag-more">
                                        <div>You can upload up to <strong>1</strong> files.</div>
                                        <div>Allowed files types: <strong>png gif jpg jpeg</strong>.</div>
                                    </div>
                                </td>
                            </tr>
                            </tbody>
                        </table>
                    </div>
                    <div class="plup-bar clearfix">
                        <input type="hidden" id="goal-upload-path" value="<?php print url('node/plup/goals', array('query' => array('plupload_token' => drupal_get_token('plupload-handle-uploads')))); ?>" />
                        <a href="#goal-select" class="plup-select" id="goals-plupload-select">Add</a>
                        <a hre="#goal-upload" class="plup-upload" id="goals-plupload-upload">Upload</a>
                        <div class="plup-progress"></div>
                    </div>
                </div>
            </div>
        </div>
        <div class="field-type-text-long field-name-field-message field-widget-text-textarea form-wrapper">
            <div class="form-item form-type-textarea">
                <label>Message </label>

                <div class="form-textarea-wrapper resizable textarea-processed resizable-textarea"><textarea
                        class="text-full form-textarea jquery_placeholder-processed" placeholder="Message"
                        name="field_goals[und][0][field_message][und][0][value]" cols="60" rows="5"></textarea>
                </div>
            </div>
        </div>
        <div class="highlighted-link form-actions">
            <a href="#brag-done" class="more">Send</a>
        </div>
        <p style="clear:both;margin-bottom:0;line-height:0px;">&nbsp;</p>
        <a href="#" onclick="jQuery('#goals').removeClass('achievement-only').scrollintoview(); return false;"
           class="fancy-close">&nbsp;</a></div>
</div>

<div id="non-sponsored">
    <?php list($b, $m, $o) = _studysauce_unsponsored_goals(); ?>
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
                <a href="#edit-reward">&nbsp;</a>
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
        <input type="hidden" name="goal-read-only"
               value="<?php print (isset($b->field_read_only['und'][0]['value']) ? $b->field_read_only['und'][0]['value'] : 0); ?>"/>
        <a href="#cancel-incentive" class="more">Cancel</a>
        <div class="highlighted-link">
            <a href="#save-incentive" class="more">Save</a>
            <a class="brag more read-only" href="#claim">Brag</a>
        </div>
    </div>
    <?php if(isset($b->field_hours['und'][0]['value'])) : ?>
    <div class="row draggable even <?php print (isset($m->item_id) ? ('gid' . $m->item_id) : ''); ?> <?php print (!isset($m->field_grade['und'][0]['value']) ? 'edit unsaved' : ''); ?>">
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
                <a href="#edit-reward">&nbsp;</a>
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
        <input type="hidden" name="goal-read-only"
               value="<?php print (isset($m->field_read_only['und'][0]['value']) ? $m->field_read_only['und'][0]['value'] : 0); ?>"/>
        <a href="#cancel-incentive" class="more">Cancel</a>
        <div class="highlighted-link">
            <a href="#save-incentive" class="more">Save</a>
            <a class="brag more read-only" href="#claim">Brag</a>
        </div>
    </div>
    <?php
    endif;
    if(isset($m->field_grade['und'][0]['value'])) : ?>
    <div class="row draggable odd <?php print (isset($o->item_id) ? ('gid' . $o->item_id) : ''); ?> <?php print (!isset($o->field_gpa['und'][0]['value']) ? 'edit unsaved' : ''); ?>">
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
                <a href="#edit-reward">&nbsp;</a>
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
        <input type="hidden" name="goal-read-only"
               value="<?php print (isset($o->field_read_only['und'][0]['value']) ? $o->field_read_only['und'][0]['value'] : 0); ?>"/>
        <a href="#cancel-incentive" class="more">Cancel</a>
        <div class="highlighted-link">
            <a href="#save-incentive" class="more">Save</a>
            <a class="brag more read-only" href="#claim">Brag</a>
        </div>
    </div>
    <?php endif; ?>
    <p style="clear:both;margin:0;text-align: center;margin-top:20px;">
        <a href="#partner" class="read-only">Now invite someone to help keep you accountable to your goals.</a>
    </p>
</div>
<div id="achievements">
    <?php
    // TODO: make this a theme or something
    print _studysauce_get_achievements();
    ?>
</div>
<div id="read-more-incentives">
    <img src="/<?php print drupal_get_path('theme', 'successinc'); ?>/images/science.png"/>

    <h3>The Science of Setting Goals</h3>
    <a href="#read-more"
       onclick="jQuery('.page-dashboard #goals #read-more-incentives .grid_6').toggle(); return false;">read
        more</a>
</div>
<p style="clear:both; margin:0;line-height:0;">&nbsp;</p>
</div>
