<?php
global $user;
$user = user_load($user->uid);
$lastOrder = _studysauce_orders_by_uid($user->uid);
?>
<div class="header-wrapper header">
    <div id="site-name">
        <a title="Home" href="/">
            <img width="48" height="48" alt="" src="/<?php print drupal_get_path('theme', 'successinc'); ?>/images/Study_Sauce_Logo.png"><strong>Study</strong> Sauce</a>
    </div>
    <?php if(!in_array('adviser', $user->roles) && !in_array('master adviser', $user->roles) &&
        empty($lastOrder)): ?>
        <div id="partner-upgrade-message" class="highlighted-link">
            <a class="more" href="/cart/add/e-p13_q1_a4o13_s?destination=cart/checkout">Sponsor student</a>
        </div>
    <?php endif; ?>
    <div id="welcome-message">Welcome <strong>
        <?php
        if ($user->uid > 0)
        {
            $user = user_load($user->uid);
            if(isset($user->field_first_name['und'][0]['value']))
                print $user->field_first_name['und'][0]['value'];
            elseif(substr($user->mail, -strlen('@internal.example.org ')) == '@internal.example.org')
                print 'student';
            else
                print $user->mail;
        }
        ?></strong>
    </div>
</div>
