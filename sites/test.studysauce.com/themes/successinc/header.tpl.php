<?php
global $user;
$studyConnections = studysauce_get_connections();
?>
<div class="header-wrapper header">
    <div id="site-name">
        <a title="Home" href="/">
            <img width="48" height="48" alt="" src="/[custom:theme-path]/logo 4 trans 2.png"><strong>Study</strong> Sauce</a>
        <div id="site-slogan">Discover the secret sauce to studying</div>
    </div>
    <div id="partner-message">
        <img src="/<?php print drupal_get_path('theme', 'successinc'); ?>/images/empty-photo.png" height="48" width="48" alt="Partner" />
        <div style="display:inline-block;">
            <?php if(isset($studyConnections[0]->field_first_name['und'][0]['value'])): ?>
                I am accountable to <br /><?php print $studyConnections[0]->field_first_name['und'][0]['value']; ?>
            <?php else: ?>
                I am accountable to <br /><a href="#partner">Click to set up</a>
            <?php endif; ?>
        </div>
    </div>
    <div id="welcome-message">Welcome <strong><?php
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
            ?></strong><br />
        <a href="#badges" class="students_only">&nbsp;</a>&nbsp;
        <!--<a href="#invite" class="<?php print empty($studyConnections) ? 'not-connected' : 'connected'; ?>">&nbsp;</a>&nbsp;-->
        <!--<a href="#mail">Mail</a>&nbsp;-->
        <!--<?php print l('account', 'user/' . $GLOBALS['user']->uid . '/edit', array('attributes' => array('class' => 'user-account'))); ?>&nbsp;-->
        <?php print l('logout', 'user/logout'); ?>
    </div>
</div>
