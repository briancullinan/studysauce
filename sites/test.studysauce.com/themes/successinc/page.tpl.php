<?php if (theme_get_setting('scrolltop_display')): ?>
    <div id="toTop"><?php print t('Back to Top'); ?></div>
<?php endif; ?>

<?php if ($page['header_top_left'] || $page['header_top_right']) : ?>
    <!-- #header-top -->
    <div id="header-top">
        <div class="container_12">

            <!-- #header-top-inside -->
            <div id="header-top-inside" class="clearfix">

                <div class="grid_8">
                    <div class="mt-grid-fix">
                        <?php if ($page['header_top_left']) : ?>
                            <!-- #header-top-left -->
                            <div id="header-top-left" class="clearfix">
                                <?php print render($page['header_top_left']); ?>
                            </div>
                            <!-- EOF:#header-top-left -->
                        <?php endif; ?>
                    </div>
                </div>

                <div class="grid_4">
                    <div class="mt-grid-fix">
                        <?php if ($page['header_top_right']) : ?>
                            <!-- #header-top-right -->
                            <div id="header-top-right" class="clearfix">
                                <?php print render($page['header_top_right']); ?>
                            </div>
                            <!-- EOF:#header-top-right -->
                        <?php endif; ?>
                    </div>
                </div>

            </div>
            <!-- EOF: #header-top-inside -->

        </div>
    </div>
    <!-- EOF: #header-top -->
<?php endif; ?>

<!-- #header-wrapper -->
<div class="header-wrapper header">

    <?php if ($site_name): ?>
        <div id="site-name">
            <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>">
                <img src="/sites/studysauce.com/themes/successinc/logo 4 trans 2.png" height="48"
                    width="48" alt=""/><b>Study</b> Sauce</a>

            <?php if ($site_slogan): ?>
                <div id="site-slogan">
                    <?php print $site_slogan; ?>
                </div>
            <?php endif; ?>

        </div>
    <?php endif; ?>

    <?php if ($page['navigation']) : ?>
        <div class="grid_7">
            <div class="mt-grid-fix">

                <!-- #header-inside-right -->
                <div id="header-inside-right">
                    <div id="main-navigation" class="clearfix">
                        <?php print drupal_render($page['navigation']); ?>
                    </div>
                </div>
                <!-- EOF: #header-inside-right -->

            </div>
        </div>
    <?php endif; ?>

</div>
    <!-- EOF: #header-inside -->

    <!-- EOF: #header -->

<!-- #banner -->
<?php if ($page['banner']): ?>
    <div id="banner" class="clearfix">
        <div class="container_12">
            <div id="banner-inside" class="clearfix">
                <div class="grid_12">
                    <div class="mt-grid-fix">
                        <?php print render($page['banner']); ?>
                    </div>
                </div>
            </div>
        </div>
    </div>
<?php endif; ?>
<!-- EOF: #banner -->

<!-- EOF: #header-wrapper -->

<!-- #page-top -->
<?php if (($breadcrumb && theme_get_setting('breadcrumb_display')) || $title || $page['highlighted'] || $page['help']): ?>
<div id="page-top">

    <!-- #breadcrumb -->
    <?php if ($breadcrumb && theme_get_setting('breadcrumb_display')): ?>
        <div id="breadcrumb">
            <div class="container_12">

                <!-- #breadcrumb-inside -->
                <div id="breadcrumb-inside" class="clearfix">
                    <div class="grid_12">
                        <div class="mt-grid-fix">

                            <?php print $breadcrumb; ?>

                        </div>
                    </div>
                </div>
                <!-- EOF: #breadcrumb-inside -->

            </div>
        </div>
    <?php endif; ?>
    <!-- EOF: #breadcrumb-wrapper -->

    <!-- #intro -->
    <div id="intro">
        <div class="container_12">

            <!-- #intro-inside -->
            <div id="intro-inside" class="clearfix">
                <div class="grid_12">
                    <div class="mt-grid-fix">
                        <!-- #highlighted -->
                        <div id="highlighted-wrapper" class="clearfix">

                            <?php if ($page['highlighted']): ?>
                                <div id="highlighted" class="clearfix">
                                    <?php print render($page['highlighted']); ?>
                                </div>
                            <?php endif; ?>

                            <?php print render($title_prefix); ?>

                            <?php print render($page['help']); ?>

                        </div>
                        <!-- EOF: #highlighted -->
                    </div>
                </div>
            </div>
            <!-- EOF: #intro-inside -->

        </div>
    </div>
    <!-- EOF: #intro -->

</div>
<?php endif; ?>
<!-- EOF: #page-top -->

<!-- #page -->
<div id="page" class="clearfix">
<div class="container_12">

<!-- #messages-console -->
<?php if ($messages): ?>
    <div id="messages-console" class="clearfix">
        <div class="grid_12">
            <div class="mt-grid-fix">
                <?php print $messages; ?>
            </div>
        </div>
    </div>
<?php endif; ?>
<!-- EOF: #messages-console -->

<!-- #promoted -->
<?php if ($page['promoted']): ?>
    <div id="promoted" class="clearfix">
        <div class="grid_12">
            <div class="mt-grid-fix">
                <?php print render($page['promoted']); ?>
            </div>
        </div>
    </div>
<?php endif; ?>
<!-- EOF: #promoted -->

<?php if (theme_get_setting('frontpage_emulate') || !drupal_is_front_page()): ?>

<?php if ($page['sidebar_first']) { ?>
<div class="">
    <div class="mt-grid-fix">
        <?php } else { ?>
        <div class="grid_12">
            <div class="mt-grid-fix">
                <?php } ?>

                <div id="main" class="clearfix">

                    <!-- #tabs -->
                    <?php if ($tabs): ?>
                        <div class="tabs">
                            <?php print render($tabs); ?>
                        </div>
                    <?php endif; ?>
                    <!-- EOF: #tabs -->

                    <!-- #action links -->
                    <?php if ($action_links): ?>
                        <ul class="action-links">
                            <?php print render($action_links); ?>
                        </ul>
                    <?php endif; ?>
                    <!-- EOF: #action links -->

                    <?php print render($title_suffix); ?>

                    <?php if (theme_get_setting('frontpage_content_print') || !drupal_is_front_page()): ?>
                        <?php print render($page['content']); ?>
                    <?php endif; ?>

                    <?php print $feed_icons; ?>

                </div>
                <!-- EOF:#main -->
            </div>
        </div>

        <!-- #sidebar -->
        <?php if ($page['sidebar_first']): ?>
            <div class="grid_4">
                <div class="mt-grid-fix">
                    <div id="sidebar" class="clearfix">
                        <?php print render($page['sidebar_first']); ?>
                    </div>
                </div>
            </div>
        <?php endif; ?>
        <!-- EOF: #sidebar -->

        <?php endif; ?>
    </div>
</div>
<!-- EOF: #page -->

<?php if ($page['footer_first'] || $page['footer_second'] || $page['footer_third']): ?>
    <!-- #footer -->
    <div id="footer" class="clearfix">
        <div class="container_12">

            <!-- #footer-inside -->
            <div id="footer-inside" class="clearfix">
                <div class="grid_4">
                    <div class="mt-grid-fix">
                        <!-- #footer-first -->
                        <?php if ($page['footer_first']): ?>
                            <div class="footer-area">
                                <?php print render($page['footer_first']); ?>
                            </div>
                        <?php endif; ?>
                        <!-- EOF: #footer-first -->
                    </div>
                </div>
                <div class="grid_4">
                    <div class="mt-grid-fix">
                        <!-- #footer-second -->
                        <?php if ($page['footer_second']): ?>
                            <div class="footer-area">
                                <?php print render($page['footer_second']); ?>
                            </div>
                        <?php endif; ?>
                        <!-- EOF: #footer-second -->
                    </div>
                </div>
                <div class="grid_4">
                    <div class="mt-grid-fix">
                        <!-- #footer-third -->
                        <?php if ($page['footer_third']): ?>
                            <div class="footer-area">
                                <?php print render($page['footer_third']); ?>
                            </div>
                        <?php endif; ?>
                        <!-- EOF: #footer-third -->
                    </div>
                </div>
            </div>
            <!-- #footer-inside -->

        </div>
    </div><!-- EOF:#footer -->
<?php endif; ?>

<div id="subfooter">
<?php print theme('studysauce-footer'); ?>
</div>

<!-- Google Code for Remarketing Tag -->
<!--------------------------------------------------
Remarketing tags may not be associated with personally identifiable information or placed on pages related to sensitive categories. See more information and instructions on how to setup the tag on: http://google.com/ads/remarketingsetup
--------------------------------------------------->
<script type="text/javascript">
    /* <![CDATA[ */
    var google_conversion_id = 990070454;
    var google_custom_params = window.google_tag_params;
    var google_remarketing_only = true;
    /* ]]> */
</script>
<script type="text/javascript"
        src="<?php print ($_SERVER['SERVER_PORT'] == 443 ? 'https' : 'http'); ?>://www.googleadservices.com/pagead/conversion.js">
</script>
<noscript>
    <div style="display:inline;">
        <img height="1" width="1" style="border-style:none;" alt=""
             src="<?php print ($_SERVER['SERVER_PORT'] == 443 ? 'https' : 'http'); ?>://googleads.g.doubleclick.net/pagead/viewthroughconversion/990070454/?value=0&amp;guid=ON&amp;script=0"/>
    </div>
</noscript>
<?php if (arg(0) == 'node' && arg(1) == 'add' && arg(2) == 'schedule'): ?>
    <!-- Google Code for Sale Conversion Page -->
    <script type="text/javascript">
        /* <![CDATA[ */
        var google_conversion_id = 990070454;
        var google_conversion_language = "en";
        var google_conversion_format = "3";
        var google_conversion_color = "ffffff";
        var google_conversion_label = "h6z4CKLN9gcQto2N2AM";
        var google_conversion_value = 0;
        var google_remarketing_only = false;
        /* ]]> */
    </script>
    <script type="text/javascript"
            src="<?php print ($_SERVER['SERVER_PORT'] == 443 ? 'https' : 'http'); ?>://www.googleadservices.com/pagead/conversion.js">
    </script>
    <noscript>
        <div style="display:inline;">
            <img height="1" width="1" style="border-style:none;" alt=""
                 src="<?php print ($_SERVER['SERVER_PORT'] == 443 ? 'https' : 'http'); ?>://www.googleadservices.com/pagead/conversion/990070454/?value=0&amp;label=h6z4CKLN9gcQto2N2AM&amp;guid=ON&amp;script=0"/>
        </div>
    </noscript>
<?php elseif (arg(1) == '13' || arg(1) == '89'): ?>
    <!-- Google Code for Lead Conversion Page -->
    <script type="text/javascript"> /* <![CDATA[ */
        var google_conversion_id = 990070454;
        var google_conversion_language = "en";
        var google_conversion_format = "3";
        var google_conversion_color = "ffffff";
        var google_conversion_label = "zcRCCKqagAgQto2N2AM";
        var google_conversion_value = 0;
        var google_remarketing_only = false;
        /* ]]> */ </script>
    <script type="text/javascript" src="<?php print ($_SERVER['SERVER_PORT'] == 443 ? 'https' : 'http'); ?>://www.googleadservices.com/pagead/conversion.js"></script>
    <noscript>
        <div style="display:inline;"><img height="1" width="1" style="border-style:none;" alt=""
                                          src="<?php print ($_SERVER['SERVER_PORT'] == 443 ? 'https' : 'http'); ?>://www.googleadservices.com/pagead/conversion/990070454/?value=0&amp;label=zcRCCKqagAgQto2N2AM&amp;guid=ON&amp;script=0"/>
        </div>
    </noscript>
    <!-- Facebook Code for Conversion Tracking -->
    <script type="text/javascript"> var fb_param = {};
        fb_param.pixel_id = '6008770260529';
        fb_param.value = '0.00';
        fb_param.currency = 'USD';
        (function () {
            var fpw = document.createElement('script');
            fpw.async = true;
            fpw.src = '<?php print ($_SERVER['SERVER_PORT'] == 443 ? 'https' : 'http'); ?>://connect.facebook.net/en_US/fp.js';
            var ref = document.getElementsByTagName('script')[0];
            ref.parentNode.insertBefore(fpw, ref);
        })(); </script>
    <noscript><img height="1" width="1" alt="" style="display:none"
                   src="<?php print ($_SERVER['SERVER_PORT'] == 443 ? 'https' : 'http'); ?>://www.facebook.com/offsite_event.php?id=6008770260529&amp;value=0&amp;currency=USD"/>
    </noscript>
<?php elseif (arg(0) == 'welcome'): ?>
    <!-- Facebook Code for Conversion Tracking -->
    <script type="text/javascript"> var fb_param = {};
        fb_param.pixel_id = '6008770262329'; // Conversion
        fb_param.value = '0.00';
        fb_param.currency = 'USD';
        (function () {
            var fpw = document.createElement('script');
            fpw.async = true;
            fpw.src = '<?php print ($_SERVER['SERVER_PORT'] == 443 ? 'https' : 'http'); ?>://connect.facebook.net/en_US/fp.js';
            var ref = document.getElementsByTagName('script')[0];
            ref.parentNode.insertBefore(fpw, ref);
        })(); </script>
    <noscript><img height="1" width="1" alt="" style="display:none"
                   src="<?php print ($_SERVER['SERVER_PORT'] == 443 ? 'https' : 'http'); ?>://www.facebook.com/offsite_event.php?id=6008770262329&amp;value=0&amp;currency=USD"/>
    </noscript>
<?php endif; ?>

