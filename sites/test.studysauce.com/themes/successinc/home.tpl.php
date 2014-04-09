<?php
global $user;
$user = user_load($user->uid);

?>
<div id="parent_home" class="parents_only">
    <h2 class="full-size">Thank you for taking a strong interest in the study behavior of your student</h2>
    <h2 class="mobile-size">Getting started is easy</h2>
    <div class="grid_6 big-arrow">
        <h3>What we do</h3>
        <p>Study Sauce was born from the realization that no one ever teaches students how to study.  This is crazy considering how much time is spent studying.</p><p>We have integrated the best scientific findings into our service to teach students the most effective methods.  We teach students how to study and change behavior along the way to create a lasting, positive change.</p><p>Our software automatically detects many of the most important study behaviors and alerts the students to both good and bad habits.  We make it easy to become a great studier.</p>
    </div>
    <div class="grid_6 highlighted-link">
        <h3>What you can do as a parent</h3>
        <ol>
            <li><h4><span>1</span> Invite student <a href="#invite2" onclick="jQuery.fancybox({href: jQuery(this).is('.connected') ? '#connections' : '#webform-ajax-wrapper-250', hideOnContentClick: false, centerOnScroll: true, padding:0, type: 'inline'}); return false;" class="more">Invite</a></h4><span>Send an invitation to your student to join and learn the best study methods.</span></li>
            <li><h4><span>2</span> Set up incentives <a href="#incentives" class="more">Set goals</a></h4><span>Give your student a little extra motivation. Incentive psychology works, try it!</span></li>
            <li><h4><span>3</span> Upgrade <a href="#plan" class="more">Study plan</a></h4><span>Purchase a personalized study plan for your student. We guarantee a higher GPA or your money back.</span></li>
        </ol>
    </div>
</div>
<div id="student_home" class="students_only">
    <h2>Getting started is easy</h2>
    <ol>
        <li><h4><span>1</span> Key dates</h4><span>Enter your important deadlines and we will send you reminders to stay on track.</span></li>
        <li><h4><span>2</span> Check in</h4><span>Check in when you study and we will guide you through the best study techniques.</span></li>
        <li><h4><span>3</span> Set goals</h4><span>Set up study goals and rewards.  Parents are often happy to sponsor them.</span></li>
    </ol>
</div>
<p style="clear:both;margin:0;font-size:1px;height:0px;">&nbsp;</p>