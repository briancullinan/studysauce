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
