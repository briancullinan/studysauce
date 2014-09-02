<?php
global $user;
$user = user_load($user->uid);
list($awards, $lastAward) = studysauce_get_awards();
?>
<div id="new-award">
    <div>
        <div>
            <span class="badge">&nbsp;</span>
            <div class="description">
                <h3>You have been awarded the <strong>{badge_name}</strong> badge.</h3>
                <p><a href="#badges">Read about your accomplishment <span>here</span>.</a></p>
            </div>
        </div>
        <a href="#" class="fancy-close">&nbsp;</a>
    </div>
</div>
<h2>Your Study Badges <small>(click on badges for detail)</small></h2>
<div class="awards">
    <a href="#setup-pulse" id="setup-pulse" class="<?php print ($awards['setup-pulse'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Pulse Detected</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Pulse Detected</h3>
        <p>Our Study Detection Software will automatically guide you to become a great studier.
        <a href="#schedule">Enter your class information <span>here</span> to unlock this badge.</a></p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['setup-pulse']; ?></p>
        <h3>Pulse Detected</h3>
        <p>Yep, you are alive.  Now let's get started.  Check in and begin a study session.</p>
        <h4>Badge description</h4>
        <p>Our Study Detection Software will automatically guide you to become a great studier.  Enter your class information <span>here</span> to unlock this badge.</p>
    </div>
    <a href="#setup-hours" id="setup-hours" class="<?php print ($awards['setup-hours'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Study Hours</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Study Hours</h3>
        <p>This is your most important goal.  It will not only allow you to start using our Study Detection Software to learn the best study methods, but it will also help you change your ingrained study behavior.  <a href="#goals" onclick="if(jQuery(window).outerWidth(true) <= 963) { jQuery('#goals .field-name-field-goals tr:nth-child(1) .field-name-field-type').scrollintoview({padding: {top:80,bottom:jQuery(window).height(),left:0,right:0}}); return false; }">Set up your study hours goal <span>here</span> to unlock.</a></p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['setup-hours']; ?></p>
        <h3>Study Hours</h3>
        <p>Good work.  Use the check-in tab every time you study to reach your weekly goal.  Our software will guide you alone the way.</p>
        <h4>Badge description</h4>
        <p>This is your most important goal.  It will not only allow you to start using our Study Detection Software to learn the best study methods, but it will also help you change your ingrained study behavior.  <a href="#goals" onclick="if(jQuery(window).outerWidth(true) <= 963) { jQuery('#goals .field-name-field-goals tr:nth-child(1) .field-name-field-type').scrollintoview({padding: {top:80,bottom:jQuery(window).height(),left:0,right:0}}); return false; }">Set up your study hours goal <span>here</span> to unlock.</a></p>
    </div>
    <a href="#setup-milestone" id="setup-milestone" class="<?php print ($awards['setup-milestone'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Study Milestone</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Study Milestone</h3>
        <p>Milestones will help you feel good about your progress as the term goes on.  <a href="#goals" onclick="if(jQuery(window).outerWidth(true) <= 963) { jQuery('#goals .field-name-field-goals tr:nth-child(2) .field-name-field-type').scrollintoview({padding: {top:80,bottom:jQuery(window).height(),left:0,right:0}}); return false; }">Set up your study milestones goal <span>here</span> to unlock.</a></p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['setup-milestone']; ?></p>
        <h3>Study Milestone</h3>
        <p>Great job.  Once you get your goal grade on an exam or paper, record the achievement.  You can also claim a reward (if sponsored) or send a brag to your parents to let them know how hard you are working.</p>
        <h4>Badge description</h4>
        <p>Milestones will help you feel good about your progress as the term goes on.  <a href="#goals" onclick="if(jQuery(window).outerWidth(true) <= 963) { jQuery('#goals .field-name-field-goals tr:nth-child(2) .field-name-field-type').scrollintoview({padding: {top:80,bottom:jQuery(window).height(),left:0,right:0}}); return false; }">Set up your study milestones goal <span>here</span> to unlock.</a></p>
    </div>
    <a href="#setup-outcome" id="setup-outcome" class="<?php print ($awards['setup-outcome'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Study Outcome</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Study Outcome</h3>
        <p><a href="#goals" onclick="if(jQuery(window).outerWidth(true) <= 963) { jQuery('#goals .field-name-field-goals tr:nth-child(3) .field-name-field-type').scrollintoview({padding: {top:80,bottom:jQuery(window).height(),left:0,right:0}}); return false; }">Set up your study outcome goal <span>here</span> to unlock.</a></p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['setup-outcome']; ?></p>
        <h3>Study Outcome</h3>
        <p>Well done.  Remember, it is great to have GPA goals, but don't get overwhelmed by them.  Changing your underlying study behavior is paramount, the GPA success will come... we promise.</p>
        <h4>Badge description</h4>
        <p><a href="#goals" onclick="if(jQuery(window).outerWidth(true) <= 963) { jQuery('#goals .field-name-field-goals tr:nth-child(3) .field-name-field-type').scrollintoview({padding: {top:80,bottom:jQuery(window).height(),left:0,right:0}}); return false; }">Set up your study outcome goal <span>here</span> to unlock.</a></p>
    </div>
    <a href="#setup-linked" id="setup-linked" class="<?php print ($awards['setup-linked'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Linked</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Linked</h3>
        <p><a href="#invite">Connect your account with your parent or other sponsor on Study Sauce <span>here</span>.  Once connected, they can sponsor your study goals and you can earn rewards.</a></p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['setup-linked']; ?></p>
        <h3>Linked</h3>
        <p>Nice work, you are linked!  The next step is to get a goal sponsored.  Earn rewards for the study hours you are putting in already.</p>
        <h4>Badge description</h4>
        <p><a href="#invite">Connect your account with your parent or other sponsor on Study Sauce <span>here</span>.  Once connected, they can sponsor your study goals and you can earn rewards.</a></p>
    </div>
    <a href="#beginner-checkin" id="beginner-checkin" class="<?php print ($awards['beginner-checkin'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Baby Steps</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Baby Steps</h3>
        <p><a href="#checkin">To unlock, complete your first study session by checking in <span>here</span>.</a></p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['beginner-checkin']; ?></p>
        <h3>Baby Steps</h3>
        <p>Congratulations, you are on the road to recovery.  By now, you should have an idea of how the software works.  Keep checking in and we will continue guiding you through the most effective study methods.</p>
        <h4>Badge description</h4>
        <p><a href="#checkin">To unlock, complete your first study session by checking in <span>here</span>.</a></p>
    </div>
    <a href="#beginner-checklist" id="beginner-checklist" class="<?php print ($awards['beginner-checklist'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Flight Student</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Flight Student</h3>
        <p>Complete one full study flight checklist (must click all the checkmarks) before a study session.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['beginner-checklist']; ?></p>
        <h3>Flight Student</h3>
        <p>The study flight checklist message will continue to pop up when you check in to help you get in the right mindset when you study.</p>
        <h4>Badge description</h4>
        <p>Complete one full study flight checklist (must click all the checkmarks) before a study session.</p>
    </div>
    <a href="#beginner-commuter" id="beginner-commuter" class="<?php print ($awards['beginner-commuter'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Commuter</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Commuter</h3>
        <p>Check in from two different study locations to unlock this badge.  Changing study locations is proven to increase retention of your study materials and is a key method to improve your study performance.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['beginner-commuter']; ?></p>
        <h3>Commuter</h3>
        <p>Read more about the benefits of changing study location on the study tips page.  It goes without saying that finding locations conducive to studying is very important.  Work as many locations as possible into your study rotation.</p>
        <h4>Badge description</h4>
        <p>Check in from two different study locations to unlock this badge.  Changing study locations is proven to increase retention of your study materials and is a key method to improve your study performance.</p>
    </div>
    <a href="#beginner-mix" id="beginner-mix" class="<?php print ($awards['beginner-mix'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Mix It Up</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Mix It Up</h3>
        <p>Complete two check-ins of different class subjects in a single setting to unlock.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['beginner-mix']; ?></p>
        <h3>Mix It Up</h3>
        <p>Studying different topics sequentially is an important tool in maximizing your brain's effectiveness to retain information.  For maximum benefit, select very different subjects to give parts of your brain a break.</p>
        <h4>Badge description</h4>
        <p>Complete two check-ins of different class subjects in a single setting to unlock.</p>
    </div>
    <a href="#beginner-breaks" id="beginner-breaks" class="<?php print ($awards['beginner-breaks'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Breather</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Breather</h3>
        <p>Breaks are important for your performance.  Take a 10-15 minute break between study sessions to unlock.  If your break is too long, or too short it won't count...</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['beginner-breaks']; ?></p>
        <h3>Breather</h3>
        <p>This is the break length that you should strive to achieve.  A well-timed break has amazing benefits and are scientifically proven to help you.</p>
        <h4>Badge description</h4>
        <p>Breaks are important for your performance.  Take a 10-15 minute break between study sessions to unlock.  If your break is too long, or too short it won't count...</p>
    </div>
    <a href="#beginner-cram" id="beginner-cram" class="<?php print ($awards['beginner-cram'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Rehabbing Crammer</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Rehabbing Crammer</h3>
        <p>Check in five days in a row to unlock.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['beginner-cram']; ?></p>
        <h3>Rehabbing Crammer</h3>
        <p>You are on fire.  The key to a less hectic finals period is spacing out your study sessions.  Do a little bit each day and periodically review to avoid having to memorize everything just before an exam.</p>
        <h4>Badge description</h4>
        <p>Check in five days in a row to unlock.</p>
    </div>
    <a href="#beginner-brain" id="beginner-brain" class="<?php print ($awards['beginner-brain'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Big Brain</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Big Brain</h3>
        <p><a href="#study-quiz">Take the study quiz <span>here</span> and score 100% to unlock this badge.</a></p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['beginner-brain']; ?></p>
        <h3>Check out the big brain on Brad</h3>
        <p>Now you know some of the study behaviors that Study Sauce will help you develop when you check in during your study sessions.</p>
        <h4>Badge description</h4>
        <p><a href="#study-quiz">Take the study quiz <span>here</span> and score 100% to unlock this badge.</a></p>
    </div>
    <a href="#beginner-sponsored" id="beginner-sponsored" class="<?php print ($awards['beginner-sponsored'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Get Sponsored</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Get Sponsored</h3>
        <p>Invite your parents to Study Sauce and have them sponsor one of your goals.  Many of our students find that their parents are happy to pay money in exchange for hard work studying.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['beginner-sponsored']; ?></p>
        <h3>Get Sponsored</h3>
        <p>Now you have a target to shoot for...check in during the week or claim your study milestone reward here.</p>
        <h4>Badge description</h4>
        <p>Invite your parents to Study Sauce and have them sponsor one of your goals.  Many of our students find that their parents are happy to pay money in exchange for hard work studying.</p>
    </div>
    <a href="#beginner-chicken" id="beginner-chicken" class="<?php print ($awards['beginner-chicken'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Chicken Dinner</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Chicken Dinner</h3>
        <p>To unlock, check in during your study sessions over the week to reach your weekly study hours goal.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['beginner-chicken']; ?></p>
        <h3>Chicken Dinner</h3>
        <p>Winner, winner, chicken dinner.  If your study hours goal is sponsored, claim it here.  If not, revel in the glory of achieving your study goal anyway!</p>
        <h4>Badge description</h4>
        <p>To unlock, check in during your study sessions over the week to reach your weekly study hours goal.</p>
    </div>
    <a href="#beginner-apples" id="beginner-apples" class="<?php print ($awards['beginner-apples'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Them Apples</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Them Apples</h3>
        <p><a href="#goals">Claim or brag about your study milestone <span>here</span>.</a>  We recommend uploading a selfie holding your exam or paper.  If you are linked, your parent/sponsor will get a copy and will love it.  Either way, you can record your achievement for posterity.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['beginner-apples']; ?></p>
        <h3>Them Apples</h3>
        <p>How do you like 'them apples?  Well done, your hard work is paying off.</p>
        <h4>Badge description</h4>
        <p><a href="#goals">Claim or brag about your study milestone <span>here</span>.</a>  We recommend uploading a selfie holding your exam or paper.  If you are linked, your parent/sponsor will get a copy and will love it.  Either way, you can record your achievement for posterity.</p>
    </div>
    <a href="#beginner-jackpot" id="beginner-jackpot" class="<?php print ($awards['beginner-jackpot'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Jackpot</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Jackpot</h3>
        <p>Get a sponsor for a study goal, complete the goal, and claim the reward.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['beginner-jackpot']; ?></p>
        <h3>Jackpot</h3>
        <p>By now, you understand the reward system.  Keep after it!</p>
        <h4>Badge description</h4>
        <p>Get a sponsor for a study goal, complete the goal, and claim the reward.</p>
    </div>
    <a href="#intermediate-flier" id="intermediate-flier" class="<?php print ($awards['intermediate-flier'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Frequent Flier</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Frequent Flier</h3>
        <p>Check in from five different study locations to unlock this badge.  Changing study locations is proven to increase retention of your study materials and is a key method to improve your study performance.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['intermediate-flier']; ?></p>
        <h3>Frequent Flier</h3>
        <p>You are starting to solidify a great study habit!  Keep taking alternating study locations to reach peak study performance.</p>
        <h4>Badge description</h4>
        <p>Check in from five different study locations to unlock this badge.  Changing study locations is proven to increase retention of your study materials and is a key method to improve your study performance.</p>
    </div>
    <a href="#intermediate-cross" id="intermediate-cross" class="<?php print ($awards['intermediate-cross'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Crosstrainer</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Crosstrainer</h3>
        <p>Study different classes in a single study session on five different occasions.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['intermediate-cross']; ?></p>
        <h3>Crosstrainer</h3>
        <p>By now you are starting to solidify this important concept as a positive study habit.  Keep going!</p>
        <h4>Badge description</h4>
        <p>Study different classes in a single study session on five different occasions.</p>
    </div>
    <a href="#intermediate-breaker" id="intermediate-breaker" class="<?php print ($awards['intermediate-breaker'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Breaker, Breaker</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Breaker, Breaker</h3>
        <p>Take 5 study breaks of 10-15 minutes between study sessions to unlock.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['intermediate-breaker']; ?></p>
        <h3>Breaker, Breaker</h3>
        <p>You are starting to solidify a great study habit!  Keep taking breaks of this length to study at optimal levels over longer durations.</p>
        <h4>Badge description</h4>
        <p>Take 5 study breaks of 10-15 minutes between study sessions to unlock.</p>
    </div>
    <a href="#intermediate-cured" id="intermediate-cured" class="<?php print ($awards['intermediate-cured'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Cured Crammer</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Cured Crammer</h3>
        <p>Check in ten days in a row to unlock.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['intermediate-cured']; ?></p>
        <h3>Cured Crammer</h3>
        <p>Now the gears are moving.  Hopefully, you are now experiencing the benefits of not cramming.</p>
        <h4>Badge description</h4>
        <p>Check in ten days in a row to unlock.</p>
    </div>
    <a href="#intermediate-disco" id="intermediate-disco" class="<?php print ($awards['intermediate-disco'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Disco Stu</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Disco Stu</h3>
        <p>Complete any combination of five goals to unlock.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['intermediate-disco']; ?></p>
        <h3>Disco Stu</h3>
        <p>You should take a night off and celebrate your accomplishments.  We recommend your friendly, neighborhood discotheque...</p>
        <h4>Badge description</h4>
        <p>Complete any combination of five goals to unlock.</p>
    </div>
    <a href="#intermediate-high" id="intermediate-high" class="<?php print ($awards['intermediate-high'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>High Life</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>High Life</h3>
        <p>Claim any combination of five rewards to unlock.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['intermediate-high']; ?></p>
        <h3>High Life</h3>
        <p>Outstanding.  At this point you should be experiencing the benefits of the Incentive Theory of Motivation.</p>
        <h4>Badge description</h4>
        <p>Claim any combination of five rewards to unlock.</p>
    </div>
    <a href="#advanced-magellan" id="advanced-magellan" class="<?php print ($awards['advanced-magellan'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Magellan</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Magellan</h3>
        <p>Check in from ten different study locations to unlock this badge.  Changing study locations is proven to increase retention of your study materials and is a key method to improve your study performance.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['advanced-magellan']; ?></p>
        <h3>Magellan</h3>
        <p>Well done, by now you should have solidified a great study habit!  Keep alternating study locations to reach peak study performance.</p>
        <h4>Badge description</h4>
        <p>Check in from ten different study locations to unlock this badge.  Changing study locations is proven to increase retention of your study materials and is a key method to improve your study performance.</p>
    </div>
    <a href="#advanced-bo" id="advanced-bo" class="<?php print ($awards['advanced-bo'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Bo Jackson</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Bo Jackson</h3>
        <p>Complete check-ins of different class subjects in a single setting on 10 separate occasions to unlock.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['advanced-bo']; ?></p>
        <h3>Bo Jackson</h3>
        <p>Consider yourself the Bo Jackson of multi-discipline studying.  You are getting pretty good at cross training your brain.</p>
        <h4>Badge description</h4>
        <p>Complete check-ins of different class subjects in a single setting on 10 separate occasions to unlock.</p>
    </div>
    <a href="#advanced-comber" id="advanced-comber" class="<?php print ($awards['advanced-comber'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Beach Comber</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Beach Comber</h3>
        <p>Take 5 study breaks of 10-15 minutes between study sessions to unlock.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['advanced-comber']; ?></p>
        <h3>Beach Comber</h3>
        <p>Fantastic job adopting this great study habit!  Keep taking breaks of this length to study at optimal levels over longer durations.</p>
        <h4>Badge description</h4>
        <p>Take 5 study breaks of 10-15 minutes between study sessions to unlock.</p>
    </div>
    <a href="#advanced-nocram" id="advanced-nocram" class="<?php print ($awards['advanced-nocram'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span><div class="mobile-only">Thunder-struck</div><div class="full-only">Thunderstruck</div></a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Thunderstruck</h3>
        <p>Check in twenty-five days in a row to unlock.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['advanced-nocram']; ?></p>
        <h3>Thunderstruck</h3>
        <p>You've been thunderstruck by the benefits of not cramming.  Well, hopefully you have anyway.  By this time you might have cruised past midterms.  By the time finals roll around, you will be much better prepared and won't have to study as much.</p>
        <h4>Badge description</h4>
        <p>Check in twenty-five days in a row to unlock.</p>
    </div>
    <a href="#advanced-magneto" id="advanced-magneto" class="<?php print ($awards['advanced-magneto'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Magneto</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Magneto</h3>
        <p>Complete any combination of ten goals to unlock.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['advanced-magneto']; ?></p>
        <h3>Magneto</h3>
        <p>You are a goal magnet.  Well done completing so many!</p>
        <h4>Badge description</h4>
        <p>Complete any combination of ten goals to unlock.</p>
    </div>
    <a href="#advanced-wall" id="advanced-wall" class="<?php print ($awards['advanced-wall'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Wall Street</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Wall Street</h3>
        <p>Claim any combination of ten rewards to unlock.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['advanced-wall']; ?></p>
        <h3>Wall Street</h3>
        <p>You are getting quite good at this...  Keep on keeping on!</p>
        <h4>Badge description</h4>
        <p>Claim any combination of ten rewards to unlock.</p>
    </div>
    <a href="#advanced-veni" id="advanced-veni" class="<?php print ($awards['advanced-veni'] ? 'awarded' : 'not-awarded'); ?>"><span>&nbsp;</span>Veni, Vidi, Vici</a>
    <div class="description before-only">
        <span class="awardArrow">&nbsp;</span>
        <h3>Veni, Vidi, Vici</h3>
        <p><a href="#goals">Claim or brag about your study outcome goal <span>here</span>.</a>  We recommend a photo to commemorate the achievement.  If you are linked, your parent/sponsor will get a copy and will love it.</p>
    </div>
    <div class="description after-only">
        <span class="awardArrow">&nbsp;</span>
        <p class="full-only"><strong>Date achieved:</strong> <?php print $awards['advanced-veni']; ?></p>
        <h3>Veni, Vidi, Vici</h3>
        <p>Congratulations!  Your hard work has paid off.  Hopefully, by now you have transformed your study behaviors and are utilizing the best known study methods.</p>
        <h4>Badge description</h4>
        <p><a href="#goals">Claim or brag about your study outcome goal <span>here</span>.</a>  We recommend a photo to commemorate the achievement.  If you are linked, your parent/sponsor will get a copy and will love it.</p>
    </div>
</div>
<?php
if(arg(1) != 'ajax' && $lastAward != null && arg(0) != 'checkin')
{

    ?>
<script type="text/javascript">
    window.initialAward = '<?php print $lastAward; ?>';
</script>
<?php
}
?>
<p style="clear:both;margin-bottom:0;line-height:0;">&nbsp;</p>