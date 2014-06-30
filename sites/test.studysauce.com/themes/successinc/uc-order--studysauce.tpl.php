<?php
$email_base = url(drupal_get_path('theme', 'successinc'), array('absolute' => true, 'https' => false));
$email_base = 'https://www.studysauce.com/sites/studysauce.com/themes/successinc';
?>

<body
    style="padding:0; margin:0; background: url(<?php print $email_base; ?>/images/noise_white.png) #FFFFFF;">
<div style="margin: 0 auto; display:block; height: 40px; background-color:#555; color:#FF9900; padding: 5px 15px; width:100%; max-width:600px;">
    <a title="Home" href="<?php print url('<front>', array('absolute' => true, 'https' => false)); ?>"
       style="font-family: 'Ubuntu',Helvetica Neue,Arial,sans-serif;font-size: 32px; color:#FFFFFF; white-space: nowrap; text-decoration: none; display:inline-block;">
        <img width="40" height="40" alt="" style="margin: 0 5px 0 5px; float: left;"
             src="<?php print $email_base; ?>/images/Study_Sauce_Logo.png"><strong
            style="color:#FF9900;">Study</strong> Sauce</a>
</div>
<div
    style="margin: 0 auto; padding:15px; background: url(<?php print $email_base; ?>/images/noise_gray.png) #EEEEEE; width:100%; max-width:600px;">
    <p style="font-family: 'Ubuntu',Helvetica Neue,Arial,sans-serif; font-size: 16px; color: #555555; ">
        <strong><?php print t('Dear !order_first_name !order_last_name', array('!order_first_name' => $order_first_name, '!order_last_name' => $order_last_name)); ?>,</strong>
    </p>

    <p style="font-family: 'Ubuntu',Helvetica Neue,Arial,sans-serif; font-size: 16px; color: #555555; ">
        Thank you for your recent purchase.  You can find the details of your order below.  If you have any questions, please feel free to contact us at <a style="color:#FF9900;" href="mailto:admin@studysauce.com">admin@studysauce.com.</a>
    </p>

    <p style="font-family: 'Ubuntu',Helvetica Neue,Arial,sans-serif; font-size: 16px; color: #555555; ">
        <?php print t('Purchasing Information:'); ?><br />
        <?php print t('E-mail Address:'); ?><br />
        <?php print $order_email; ?><br />
        <?php print t('Billing Address:'); ?><br />
        <?php print $order_billing_address; ?><br />
        <br />
        <?php print t('Order Total:'); ?>
        <?php print $order_total; ?>
        <?php print t('Payment Method:'); ?>
        <?php print $order_payment_method; ?>
    </p>

    <p style="font-family: 'Ubuntu',Helvetica Neue,Arial,sans-serif; font-size: 16px; color: #555555; ">
        To access your account <a style="color:#FF9900;"
                                  href="<?php print url('user/login', array('absolute' => true, 'query' => array('destination' => ''))); ?>"
                                  target="_blank">click here.</a>
    </p>

    <p style="font-family: 'Ubuntu',Helvetica Neue,Arial,sans-serif; font-size: 16px; color: #555555; ">
        <br/><br/>
        Keep studying!<br/>
        The Study Sauce Team
    </p>
    <p style="text-align: center; font-family: 'Ubuntu',Helvetica Neue,Arial,sans-serif; font-size: 16px; color: #555555; ">
        <a href="https://www.facebook.com/pages/Study-Sauce/519825501425670?ref=stream"
           style="background:url(<?php print $email_base; ?>/images/social_sprites_v2.png) no-repeat 0 0 transparent; height: 45px; width: 45px; display: inline-block; color:transparent;">
            &nbsp;</a>
        <a href="https://plus.google.com/115129369224575413617/about"
           style="background:url(<?php print $email_base; ?>/images/social_sprites_v2.png) no-repeat 0 -95px transparent; height: 45px; width: 45px; display: inline-block; color:transparent;">
            &nbsp;</a>
        <a href="https://twitter.com/StudySauce"
           style="background:url(<?php print $email_base; ?>/images/social_sprites_v2.png) no-repeat 0 -190px transparent; height: 45px; width: 45px; display: inline-block; color:transparent;">
            &nbsp;</a>
    </p>
    <?php if (isset($footer)): ?>
        <p style="font-family: 'Ubuntu',Helvetica Neue,Arial,sans-serif; font-size: 16px; color: #555555; ">
            <?php print $footer; ?>
        </p>
    <?php endif; ?>
</div>
<div
    style="text-align: center; margin: 0 auto; font-family: 'Ubuntu',Helvetica Neue,Arial,sans-serif; font-size: 9px; color: #555555; width:100%; max-width:600px;">
    Copyright <?php print date('Y'); ?>. &nbsp;<a target="_blank"
                                                  href="<?php print url('privacy', array('absolute' => true)); ?>"
                                                  style="text-decoration: underline; color: #555555; font-family: 'Ubuntu',Helvetica Neue,Arial,sans-serif; font-size: 9px;">Privacy
        Policy</a>&nbsp;|&nbsp;<a target="_blank" href="%unsubscribe%"
                                  style="text-decoration: underline; color: #555555; font-family: 'Ubuntu',Helvetica Neue,Arial,sans-serif; font-size: 9px;">Unsubscribe</a>
</div>
</body>