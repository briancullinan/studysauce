<?php

/**
 * @file
 * Customize confirmation screen after successful submission.
 *
 * This file may be renamed "webform-confirmation-[nid].tpl.php" to target a
 * specific webform e-mail on your site. Or you can leave it
 * "webform-confirmation.tpl.php" to affect all webform confirmations on your
 * site.
 *
 * Available variables:
 * - $node: The node object for this webform.
 * - $progressbar: The progress bar 100% filled (if configured). This may not
 *   print out anything if a progress bar is not enabled for this node.
 * - $confirmation_message: The confirmation message input by the webform
 *   author.
 * - $sid: The unique submission ID of this submission.
 */
?>
<?php
list($awards) = studysauce_get_awards();
$s = db_select('studysauce_quiz', 'q')
    ->fields('q', array('place', 'underlining', 'same_subject', 'laying_down', 'longer_sessions'))
    ->condition('q.sid', $sid, '=')
    ->orderBy('time_completed')
    ->execute()
    ->fetchAssoc();
?>

<div class="webform-confirmation dialog">
    <div>
        <div class="grid_5 even full-only">
            <h2>What you answered...</h2>
        </div>
        <div class="grid_5 right full-only">
            <h2>What the science says...</h2>
        </div>
        <h2 class="mobile-only">See how your study habits stack up with the science in the dark gray boxes</h2>
    </div>
    <div class="quiz-wrapper">
        <div class="grid_5 even first">
            <p><em><strong>1</strong></em>I always study in the same place</p>
            <p><span class="<?php print ($s['place'] ? 'incorrect' : 'correct'); ?>"><span>True</span><br /><span>False</span></span></p>
        </div>
        <div class="grid_5 right first">
            <p><em><strong>1</strong></em>Simply varying the location of where you study has been proven to dramatically improve information retention.</p>
        </div>
    </div>
    <div class="quiz-wrapper">
        <div class="grid_5 even">
            <p><em><strong>2</strong></em>I underline and highlight materials to help me remember information</p>
            <p><span class="<?php print ($s['underlining'] ? 'incorrect' : 'correct'); ?>"><span>True</span><br /><span>False</span></span></p>
        </div>
        <div class="grid_5 right">
            <p><em><strong>2</strong></em>It turns out that highlighting and underlining are some of the least effective study methods. &nbsp;Don't spend too much time doing them. Instead, quickly identify the important material and then create flash cards. &nbsp;Flash cards are a very effective way to train your brain to remember important information.</p>
        </div>
    </div>
    <div class="quiz-wrapper">
        <div class="grid_5 even">
            <p><em><strong>3</strong></em>I focus on one subject until I am sure I understand everything</p>
            <p><span class="<?php print ($s['same_subject'] ? 'incorrect' : 'correct'); ?>"><span>True</span><br /><span>False</span></span></p>
        </div>
        <div class="grid_5 right">
            <p><em><strong>3</strong></em>By alternating study material, your brain is better able to retain information. &nbsp;Think of this as the equivalent of cross-training for athletes.</p>
        </div>
    </div>
    <div class="quiz-wrapper">
        <div class="grid_5 even">
            <p><em><strong>4</strong></em>I frequently study while lying down or in a comfortable place</p>
            <p><span class="<?php print ($s['laying_down'] ? 'incorrect' : 'correct'); ?>"><span>True</span><br /><span>False</span></span></p>
        </div>
        <div class="grid_5 right">
            <p><em><strong>4</strong></em>Studying in bed is a definite no-no. &nbsp;Your brain associates your bed with sleeping and therefore will likely make you more drowsy while you try to study. &nbsp;This in turn reduces your effectiveness in studying.</p>
        </div>
    </div>
    <div class="quiz-wrapper">
        <div class="grid_5 even last">
            <p><em><strong>5</strong></em>I prefer longer study sessions to power through material</p>
            <p><span class="<?php print ($s['longer_sessions'] ? 'incorrect' : 'correct'); ?>"><span>True</span><br /><span>False</span></span></p>
        </div>
        <div class="grid_5 right last">
            <p><em><strong>5</strong></em>Individual tolerance will vary, but in general you should follow a 50/10 rule for studying. &nbsp;That is 50 minutes of studying followed by a 10 minute break. &nbsp;This keeps you fresh and will allow you to perform better.</p>
        </div>
    </div>
    <p style="clear: both; margin-bottom:0px;">&nbsp;</p>
    <hr />
    <p class="highlighted-link">
        <a href="#close" class="more">Close</a>
    </p>
    <a href="#close">&nbsp;</a>
</div>
