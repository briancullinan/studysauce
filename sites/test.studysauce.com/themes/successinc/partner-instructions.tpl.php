<div class="fixed-centered modal">
    <div id="first-time-message" class="dialog">
        <h2>Welcome to Study Sauce</h2>
        <p>Thank you for agreeing to help your student<?php print (isset($account->field_first_name['und'][0]['value']) ? (', ' . $account->field_first_name['und'][0]['value']) : ''); ?>.  Click the button below for a few tips on how to be a great accountability partner.</p>
        <p class="highlighted-link">
            <a class="more" href="#first-time-message-2">Next</a>
        </p>
        <a href="#close">&nbsp;</a>
    </div>
    <div id="first-time-message-2" class="dialog">
        <h2>Why an accountability partner?</h2>
        <p>Research shows that simply writing down your goals makes you more likely to achieve them.  Having an accountability partner takes this to a new level.  We all have ups and downs in school and finding someone to help motivate and challenge you along the way can be invaluable.</p>
        <p class="highlighted-link">
            <a class="more" href="#first-time-message-3">Next</a>
        </p>
        <a href="#close">&nbsp;</a>
    </div>
    <div id="first-time-message-3" class="dialog">
        <h2>Why me?</h2>
        <p>Your student chose you because he/she believes that you will:
            <ul>
                <li>Challenge him/her. This requires more than just encouragement.</li>
                <li>Will celebrate his/her successes.</li>
                <li>Be emotionally invested in his/her education.</li>
                <li>Continue to be someone that he/she trusts.</li>
            </ul></p>
        <p class="highlighted-link">
            <a class="more" href="#first-time-message-4">Next</a>
        </p>
        <a href="#close">&nbsp;</a>
    </div>
    <div id="first-time-message-4" class="dialog">
        <h2>Now what?</h2>
        <p>Communication is the key!  Outline and agree upon expectations for your student.  Remember that it is ok if it feels a little uncomfortable.  Then hold the student accountable for achieving the goals he/she create.</p>
        <p>Set up regular check-ins (try to talk at least once every week or two).  Let your student be transparent about his/her struggles and successes during the conversations.  They need the outlet.</p>
        <p>In general, just be there for your student.  There will undoubtedly be times throughout school that he/she will need you!</p>
        <p class="highlighted-link">
            <a class="more" href="#close">Finish</a>
        </p>
        <a href="#close">&nbsp;</a>
    </div>
</div>
<script type="text/javascript">
    jQuery(document).ready(function () {
        jQuery('#first-time-message').dialog();
    });
</script>





