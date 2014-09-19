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
    if(isset($partner->field_email['und'][0]['value']))
    {
        $partnerUser = user_load_by_mail($partner->field_email['und'][0]['value']);
        if($partnerUser)
            $partner = $partnerUser;
    }
}


// check if we are being advised by another user in a group
$groups = og_get_groups_by_user();
$readonly = false;
if(isset($groups['node']))
{
    // get group adviser
    $query = db_select('og_membership', 'ogm');
    $query->condition('ogm.gid', array_keys($groups['node']), 'IN');
    $query->fields('ogm', array('entity_type', 'etid'));
    $result = $query->execute();
    $members = $result->fetchAll();
    foreach($members as $i => $member)
    {
        $m = user_load($member->etid);
        if(in_array('adviser', $m->roles) || in_array('master adviser', $m->roles))
        {
            $partner = $m;
            if(in_array('adviser', $m->roles))
                break;
        }
    }
}

?>
<div class="header-wrapper header">
    <div id="site-name">
        <a title="Home" href="/">
            <img width="48" height="48" alt="" src="/<?php print drupal_get_path('theme', 'successinc'); ?>/logo 4 trans 2.png"><strong>Study</strong> Sauce</a>
        <div id="site-slogan">Discover the secret sauce to studying</div>
    </div>
    <div id="partner-message">
        <?php if(isset($partner->field_partner_photo['und'][0]['fid']) ||
            isset($partner->picture)):
            $file = isset($partner->field_partner_photo['und'][0]['fid']) ? file_load($partner->field_partner_photo['und'][0]['fid']) : $partner->picture; ?>
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
    <?php
    $view = views_get_view('music_player');
    $result = views_get_view_result('music_player', 'block_1', array());
    $links = array_map(function ($x) {return $x->field_field_media_url[0]['rendered']['#markup'];}, $result);
    ?>
    <div id="jquery_jplayer"></div>
    <script type="text/javascript">
        window.musicLinks = <?php print json_encode($links); ?>;
        window.musicIndex = 0;
        jQuery(document).ready(function () {
            jQuery('.minplayer-default-play').on('click', function () {
                var index = window.musicIndex++;
                jQuery('#jquery_jplayer').jPlayer("setMedia", {
                    mp3: window.musicLinks[index],
                    m4a: window.musicLinks[index].substr(0, window.musicLinks[index].length - 4) + '.mp4',
                    oga: window.musicLinks[index].substr(0, window.musicLinks[index].length - 4) + '.ogg'
                });
            });

            window.currentAudio = jQuery('#jquery_jplayer').jPlayer({
                swfPath: '/sites/test.studysauce.com/themes/successinc/js',
                solution: 'html, flash',
                supplied: 'mp3, m4a, oga',
                preload: 'metadata',
                volume: 0.8,
                muted: false,
                cssSelectorAncestor: '.page-dashboard #checkin',
                cssSelector: {
                    play: '.minplayer-default-play',
                    pause: '.minplayer-default-pause'
                }
            });
            jQuery("#jquery_jplayer").bind(jQuery.jPlayer.event.ended, function(event) {
                var index = window.musicIndex++;
                jQuery('#jquery_jplayer').jPlayer("setMedia", {
                    mp3: window.musicLinks[index],
                    m4a: window.musicLinks[index].substr(0, window.musicLinks[index].length - 4) + '.mp4',
                    oga: window.musicLinks[index].substr(0, window.musicLinks[index].length - 4) + '.ogg'
                });
                jQuery(this).jPlayer("play");
            });
        });
    </script>
    <div id="welcome-message"><span>Welcome </span><strong><?php
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
        <?php print l('logout', 'user/logout', array('attributes' => array('title' => 'Log out'))); ?>
    </div>
</div>
