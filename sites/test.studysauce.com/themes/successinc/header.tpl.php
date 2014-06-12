<?php
global $user;
$user = user_load($user->uid);
if(isset($user->field_partners['und'][0]['value']))
{
    $partner = entity_load('field_collection_item', array($user->field_partners['und'][0]['value']));
    $partner = $partner[$user->field_partners['und'][0]['value']];
    if(!isset($partner->field_permissions['und']) || !is_array($partner->field_permissions['und']))
        $partner->field_permissions['und'] = array();
    $permissions = array_map(function ($x) { return $x['value']; }, $partner->field_permissions['und']);
}

?>
<div class="header-wrapper header">
    <div id="site-name">
        <a title="Home" href="/">
            <img width="48" height="48" alt="" src="/<?php print drupal_get_path('theme', 'successinc'); ?>/logo 4 trans 2.png"><strong>Study</strong> Sauce</a>
        <div id="site-slogan">Discover the secret sauce to studying</div>
    </div>
    <div id="partner-message">
        <?php if(isset($partner->field_partner_photo['und'][0]['fid'])):
            $file = file_load($partner->field_partner_photo['und'][0]['fid']); ?>
            <img src="<?php print image_style_url('achievement', $file->uri); ?>" height="48" width="48" alt="Partner" />
        <?php else: ?>
            <img src="/<?php print drupal_get_path('theme', 'successinc'); ?>/images/empty-photo.png" height="48" width="48" alt="Partner" />
        <?php endif; ?>
        <div style="display:inline-block;">
            <?php if(isset($partner->field_first_name['und'][0]['value'])): ?>
                I am accountable to: <br /><span><?php print $partner->field_first_name['und'][0]['value']; ?> <?php print $partner->field_last_name['und'][0]['value']; ?></span>
            <?php else: ?>
                I am accountable to: <br /><a href="#partner">Click to set up</a>
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
        <!--<a href="#invite" class="<?php print (empty($partner) ? 'not-connected' : 'connected'); ?>">&nbsp;</a>&nbsp;-->
        <!--<a href="#mail">Mail</a>&nbsp;-->
        <!--<?php print l('account', 'user/' . $GLOBALS['user']->uid . '/edit', array('attributes' => array('class' => 'user-account'))); ?>&nbsp;-->
        <?php print l('logout', 'user/logout'); ?>
    </div>
</div>