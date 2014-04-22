<?php
global $user;
if(isset($user->field_parent_student['und'][0]['value']) && $user->field_parent_student['und'][0]['value'] == 'parent'): ?>
    <h1>Choose your student's plan</h1>
<?php else: ?>
    <h1>Choose your plan</h1>
<?php endif; ?>
<div class="grid_8 money-back">
    <div>
        <p>All plans have a <span>money-back guarantee</span>, so there is&nbsp;no risk to you</p>
    </div>
</div>
<div class="grid_9 product-wrapper">
    <div class="grid_3"><span class="price-is-right"><span>$10</span>/ term</span>
        <h3>Single term</h3>
        <ul>
            <li>Term study schedule</li>
            <li>Unlimited support</li>
            <li>Study tips</li>
        </ul>
        <p class="and-more"><img src="/sites/studysauce.com/themes/successinc/fancy-amp.png"> More...</p>
        <p class="wrap-button"><a class="more" href="/cart/add/e-p13_q1_a2o5_s?destination=cart/checkout" draggable="false">Choose plan</a></p>
    </div>
    <div class="grid_3 highlighted-link"><span class="price-is-right"><span>$18</span> / year</span>
        <h3>School year</h3>
        <p class="recommended">Recommended</p>
        <ul>
            <li>Study schedule for the year</li>
            <li>Unlimited support</li>
            <li>Study tips</li>
        </ul>
        <p class="and-more"><img src="/sites/studysauce.com/themes/successinc/fancy-amp.png"> More...</p>
        <p class="wrap-button"><a class="more" href="cart/add/e-p13_q2_a2o6_s?destination=cart/checkout" draggable="false">Choose plan</a></p>
    </div>
    <div class="grid_3"><span class="price-is-right"><span>35%</span> off</span>
        <h3>Multiple terms</h3>
        <ul>
            <li>Study schedule for &nbsp;<select name="terms" id="terms" onchange="jQuery('#multiple-link').attr('href', 'cart/add/'+jQuery(this).val()+'?destination=cart/checkout');"><option value="e-p13_q3_a2o7_s">3 terms&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;$27.00 (10% off)</option> <option value="e-p13_q4_a2o8_s">4 terms&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;$34.00 (15% off)</option> <option value="e-p13_q5_a2o9_s">5 terms&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;$40.00 (20% off)</option> <option value="e-p13_q6_a2o10_s">6 terms&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;$45.00 (25% off)</option> <option value="e-p13_q7_a2o11_s">7 terms&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;$49.00 (30% off)</option> <option value="e-p13_q8_a2o12_s">8 terms&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;$52.00 (35% off)</option> <option value="e-p13_q9_a2o12_s">9 terms&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;$58.50 (35% off)</option> <option value="e-p13_q10_a2o12_s">10 terms&nbsp;&nbsp;&nbsp;&nbsp;$65.00 (35% off)</option> <option value="e-p13_q11_a2o12_s">11 terms&nbsp;&nbsp;&nbsp;&nbsp;$71.50 (35% off)</option> <option value="e-p13_q12_a2o12_s">12 terms&nbsp;&nbsp;&nbsp;&nbsp;$78.00 (35% off)</option> </select></li>
            <li>Unlimited support</li>
            <li>Study tips</li>
        </ul>
        <p class="and-more"><img src="/sites/studysauce.com/themes/successinc/fancy-amp.png"> More...</p>
        <p class="wrap-button"><a class="more" id="multiple-link" onclick="window.location='cart/add/'+jQuery('#terms').val()+'?destination=cart/checkout';return false;" href="cart/add/e-p13_q3_a2o7_s?destination=cart/checkout" draggable="false">Choose plan</a></p>
    </div>
</div>


