<?php if(drupal_get_path_alias(current_path()) == 'cart/checkout' ||
    drupal_get_path_alias(current_path()) == 'profile' ||
    drupal_get_path_alias(current_path()) == 'schedule' ||
    drupal_get_path_alias(current_path()) == 'schedule2' ||
    drupal_get_path_alias(current_path()) == 'customization' ||
    drupal_get_path_alias(current_path()) == 'customization2') : ?>
    <?php
    $menu = menu_navigation_links('menu-landing-secondary-menu');
    print theme('links__menu_landing_secondary_menu', array('links' => $menu, 'attributes' => array('class'=> array('menu', 'landing-secondary-menu')) ));
    ?>
<?php else: ?>
<div style="display:inline-block; vertical-align:top;">
    <iframe class="facebook-like" src="https://www.facebook.com/plugins/like.php?href=https%3A%2F%2Fwww.facebook.com%2Fpages%2FStudy-Sauce%2F519825501425670%3Fref%3Dstream&amp;layout=button_count&amp;show_faces=false&amp;width=89&amp;action=like&amp;colorscheme=light&amp;height=35&amp;locale=en_US" scrolling="no" frameborder="0" style="border:none; overflow:hidden;" allowtransparency="true"></iframe>
    <a href="https://twitter.com/StudySauce" class="twitter-follow-button" data-show-count="false" data-show-screen-name="false">Follow @StudySauce</a>
    <script>!function(d,s,id){var js,fjs=d.getElementsByTagName(s)[0],p=/^http:/.test(d.location)?'http':'https';if(!d.getElementById(id)){js=d.createElement(s);js.id=id;js.src=p+'://platform.twitter.com/widgets.js';fjs.parentNode.insertBefore(js,fjs);}}(document, 'script', 'twitter-wjs');</script>
    <a target="_blank" href="https://www.facebook.com/pages/Study-Sauce/519825501425670?ref=stream">&nbsp;</a>
    <a href="https://plus.google.com/115129369224575413617/about">&nbsp;</a>
    <a href="https://twitter.com/StudySauce">&nbsp;</a>
    <!--<a target="_blank" href="https://www.facebook.com/sharer/sharer.php?u=https://www.studysauce.com">Like us</a>
    <a href="https://plus.google.com/share?url=https://www.studysauce.com">Like us</a>-->
</div>
<?php
$menu = menu_navigation_links('menu-secondary-menu');
print theme('links__menu_secondary_menu', array('links' => $menu, 'attributes' => array('class'=> array('menu', 'secondary-menu')) ));
?>
<span><?php print 'Copyright ' . date('Y'); ?></span>
<script type="text/javascript">
    var fb_param = {};
    fb_param.pixel_id = '6008770262329';
    fb_param.value = '0.00';
    fb_param.currency = 'USD';
    (function(){
        var fpw = document.createElement('script');
        fpw.async = true;
        fpw.src = '//connect.facebook.net/en_US/fp.js';
        var ref = document.getElementsByTagName('script')[0];
        ref.parentNode.insertBefore(fpw, ref);
    })();
</script>
<noscript><img height="1" width="1" alt="" style="display:none" src="https://www.facebook.com/offsite_event.php?id=6008770262329&amp;value=0&amp;currency=USD" /></noscript>
<?php endif; ?>