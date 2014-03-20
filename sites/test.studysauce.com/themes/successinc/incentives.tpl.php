<?php
global $studyConnections;
?>
<div id="fb-root"></div>
<script>
    (function(d, s, id) {
            var js, fjs = d.getElementsByTagName(s)[0];
            if (d.getElementById(id)) return;
            js = d.createElement(s); js.id = id;
            js.src = "//connect.facebook.net/en_US/all.js";
            fjs.parentNode.insertBefore(js, fjs);
        }(document, 'script', 'facebook-jssdk'));
</script>
<div id="incentives-saved">
    <h2>The incentives have been saved.</h2>
</div>
<?php global $user;
$setup = studysauce_is_incentives_setup();
$parent = studysauce_get_parent_goals();
?>
<div
    class="step_<?php print $setup; ?> <?php print empty($studyConnections) ? 'not-connected' : 'connected'; ?> <?php print isset($parent) ? 'sponsored' : ''; ?>">
    <? /*
<p style="text-align:center;">
<div class="grid_3 goal-status">
<strong><?php print studysauce_get_study_weeks(); ?></strong>
<span>Study weeks completed</span>
</div>
<div class="grid_3 goal-status">
<strong><?php print studysauce_get_milestones(); ?></strong>
<span>Milestones achieved</span>
</div>
<div class="grid_3 goal-status">
<strong><?php print studysauce_get_last_achievement(); ?></strong>
<span>Last achievement</span>
</div>
</p>
*/
    ?>
    <div id="student_step_1">
        <h2 class="students_only student_step_1">Set goals and rewards, then get sponsored by your parents</h2>

        <h2 class="parents_only student_step_1">Set goals and reward your student's study efforts</h2>

        <div class="grid_6 big-arrow">
            <h3>The Science</h3>
            <img src="/[custom:theme-path]/images/science.png"/>

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
    <div id="invite-sent">
        <h2>
            <a href="#" onclick="jQuery('#incentives').removeClass('invite-sent-only').scrollintoview(); return false;">
                <span>Invite has been sent. Thank you.<br/>
                <small>Click to continue.</small></span>
            </a>
            <a href="#" onclick="jQuery('#incentives').removeClass('invite-sent-only').scrollintoview(); return false;" class="fancy-close">&nbsp;</a>
        </h2>
    </div>
 */
    ?>
    <div id="invite">
        <?php
        if (empty($studyConnections)) {
            ?>
            <div class="students_only not-connected">
                <?php
                $node = node_load(157);
                webform_node_view($node, 'full');
                print theme_webform_view($node->content); ?>
                <h3 class="parents_only">Recommend to your friends</h3>
                <h3 class="students_only">Recommend to your classmates</h3>
                <p style="margin-bottom:0;" class="like-us"><a target="_blank" href="https://www.facebook.com/sharer/sharer.php?u=https://www.studysauce.com">&nbsp;</a>
                    <a href="https://plus.google.com/share?url=https://www.studysauce.com">&nbsp;</a>
                    <a href="https://twitter.com/intent/tweet?source=webclient&text=Check+out+www.StudySauce.com+for+your+online+study+assistant.">&nbsp;</a></p>
                <a href="#" onclick="jQuery('#incentives').removeClass('invite-only').scrollintoview(); return false;" class="fancy-close">&nbsp;</a>
            </div>
            <div class="parents_only not-connected">
                <?php
                $node = node_load(250);
                webform_node_view($node, 'full');
                print theme_webform_view($node->content); ?>
                <h3 class="parents_only">Recommend to your friends</h3>
                <h3 class="students_only">Recommend to your classmates</h3>
                <p style="margin-bottom:0;" class="like-us"><a target="_blank" href="https://www.facebook.com/sharer/sharer.php?u=https://www.studysauce.com">&nbsp;</a>
                    <a href="https://plus.google.com/share?url=https://www.studysauce.com">&nbsp;</a>
                    <a href="https://twitter.com/intent/tweet?source=webclient&text=Check+out+www.StudySauce.com+for+your+online+study+assistant.">&nbsp;</a></p>
                <a href="#" onclick="jQuery('#incentives').removeClass('invite-only').scrollintoview(); return false;" class="fancy-close">&nbsp;</a>
            </div>
        <?php } else { ?>
            <div>
                <h2>Your account is connected to:</h2>
                <?php
                global $studyConnections;
                foreach ($studyConnections as $i => $conn) {
                    if (isset($conn->field_first_name['und'][0]['value']) && isset($conn->field_last_name['und'][0]['value']))
                        $displayName = $conn->field_first_name['und'][0]['value'] . ' ' . $conn->field_last_name['und'][0]['value'];
                    else
                        $displayName = $conn->mail;

                    print '<span class="' . (isset($conn->uid) ? 'connected' : 'not-connected') . '">' . $displayName . ' (' . $conn->mail . ')</span>';
                }
                ?>
                <h3 class="parents_only" style="margin-right:0;">Recommend to your friends</h3>
                <h3 class="students_only" style="margin-right:0;">Recommend to your classmates</h3>
                <p style="margin-bottom:0;margin-right:0;" class="like-us"><a target="_blank" href="https://www.facebook.com/sharer/sharer.php?u=https://www.studysauce.com">&nbsp;</a>
                    <a href="https://plus.google.com/share?url=https://www.studysauce.com">&nbsp;</a>
                    <a href="https://twitter.com/intent/tweet?source=webclient&text=Check+out+www.StudySauce.com+for+your+online+study+assistant.">&nbsp;</a></p>
                <a href="#" onclick="jQuery('#incentives').removeClass('invite-only').scrollintoview(); return false;" class="fancy-close">&nbsp;</a>
            </div>
        <?php } ?>
    </div>
    <div id="parent-sponsored">
        <?php
        if (isset($parent)) {
            print drupal_render($parent);
        } else {
        } ?>
    </div>
    <?php
    if ($setup < 3) {
        ?>
        <div id="student_step_2" class="student_step_2">
            <h2>What are your rewards?<span>Step 2 of 2</span></h2>

            <div class="grid_5 highlighted-link">
                <p>Our students find that parents are often happy to reward good study behavior with money or other
                    rewards. We have also found that parents are eager to help students improve their study behaviors in
                    any way possible. Involving them in your goals can be a win-win.</p>
                <a href="#"
                   onclick="jQuery('.logged-in.page-home #student_step_2 .highlighted-link').hide(); jQuery('#parent-sponsored').addClass('setup-invite'); return false;"
                   class="more">Invite my parents</a><br/>
                <a href="#"
                   onclick="jQuery('.logged-in.page-home #student_step_2 .highlighted-link').hide(); jQuery('.logged-in.page-home #incentives .field-name-field-reward, .logged-in.page-home #incentives .form-actions').show(); jQuery('.logged-in.page-home .node-incentive-form input[name=&quot;field_invite[und]&quot;]').attr('checked', 'checked'); return false;">Set
                    up my own rewards</a>
            </div>
            <p style="clear:both; margin:0;line-height:0;">&nbsp;</p>
        </div>
    <?php } //endif; ?>
    <div id="non-sponsored">
        <?php
        $form = _studysauce_get_incentives_form();
        print drupal_render($form);
        ?>
        <p style="clear:both;margin:0;line-height:0;">&nbsp;</p>
    </div>
    <div id="achievements">
        <?php
        // TODO: make this a theme or something
        print _studysauce_get_achievements($parent, $form);
        if (isset($_SESSION['studysauce']['achievement']))
            unset($_SESSION['studysauce']['achievement']);
        ?>
    </div>
    <div id="read-more-incentives">
        <img src="/[custom:theme-path]/images/science.png"/>

        <h3>The Science of Setting Goals</h3>
        <a href="#read-more"
           onclick="jQuery('.page-dashboard #incentives #read-more-incentives .grid_6').toggle(); return false;">read
            more</a>
    </div>
    <p style="clear:both; margin:0;line-height:0;">&nbsp;</p>
</div>
