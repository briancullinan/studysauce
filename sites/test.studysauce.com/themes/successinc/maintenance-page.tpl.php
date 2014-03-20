<?php

/**
 * @file
 * Override of the default maintenance page.
 *
 * This is an override of the default maintenance page. Used for Garland and
 * Minnelli, this file should not be moved or modified since the installation
 * and update pages depend on this file.
 *
 * This mirrors closely page.tpl.php for Garland in order to share the same
 * styles.
 */
?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
  "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
    <head>
    <title><?php print $head_title ?></title>
    <?php print $head ?>
    <?php $protocol = theme_get_setting('protocol'); ?>
    <link rel="stylesheet" type="text/css" href="<?php print drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/ubuntu-font.css' ?>" />
    <link rel="stylesheet" type="text/css" href="<?php print drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/lato-font.css' ?>" />
    <?php print $styles ?>
    <?php print $scripts ?>
    </head>
    
    <body class="<?php print $classes; ?> no-banner" <?php print $attributes;?>>
        <div id="header-top">
            <div class="container_12">
                
                <!-- #header-top-inside -->
                <div id="header-top-inside" class="clearfix">
                
                    <div class="grid_8">
                    </div>
                    
                    <div class="grid_4">
                    </div>
                    
                </div>
                <!-- EOF: #header-top-inside -->
                
            </div>
        </div>
        <!-- EOF: #header-top -->
        
        <!-- #header-wrapper -->
        <div id="header-wrapper" class="clearfix">
        
            <!-- #header -->
            <div id="header" class="clearfix">
                <div class="container_12">
                
                    <!-- #header-inside -->
                    <div id="header-inside" class="clearfix">
                    
                        <div class="grid_12">
							<?php if ($logo):?>
                            <div id="logo">
                            <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home"> <img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" /> </a>
                            </div>
                            <?php endif; ?>
                            
                            <?php if ($site_name):?>
                            <div id="site-name">
                            <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>"><?php print $site_name; ?></a>
                            </div>
                            <?php endif; ?>
                            
                            <?php if ($site_slogan):?>
                            <div id="site-slogan">
                            <?php print $site_slogan; ?>
                            </div>
                            <?php endif; ?>
                        </div>
                        
                    </div>
                    <!-- EOF: #header-inside -->
                
                </div>
            </div>
            <!-- EOF: #header -->
        
        </div>
        <!-- EOF: #header-wrapper -->
        
        <!-- #page-top -->
        <div id="page-top">
        
            <!-- #breadcrumb -->
            <div id="breadcrumb">
                <div class="container_12">
                
                    <!-- #breadcrumb-inside -->
                    <div id="breadcrumb-inside" class="clearfix">
                        <div class="grid_12">
                        </div>
                    </div>
                    <!-- EOF: #breadcrumb-inside -->
                
                </div>
            </div>
            <!-- EOF: #breadcrumb-wrapper -->
            
            <!-- #intro -->
            <div id="intro">
                <div class="container_12">
                
                    <!-- #intro-inside -->
                    <div id="intro-inside"  class="clearfix">
                        <div class="grid_12">
                        <?php if ($title): ?><h1 class="title"><?php print $title; ?></h1> <?php endif; ?>
                        </div>
                    </div>
                    <!-- EOF: #intro-inside -->
                
                </div>
            </div>
            <!-- EOF: #intro -->
        
        </div>
        <!-- EOF: #page-top -->
        
        <!-- #page -->
        <div id="page" class="clearfix">
            <div class="container_12">
                <div class="grid_12">
                <?php print $messages; ?>
                <?php print $content; ?>
                </div>
            </div>
        </div>
        <!-- EOF: #page -->
        
        <!-- #footer -->
        <div id="footer" class="clearfix">
            <div class="container_12">
                    
                <!-- #footer-inside -->
                <div id="footer-inside" class="clearfix">
                    <div class="grid_4">
                    </div>
                    <div class="grid_4">
                    </div>
                    <div class="grid_4">
                    </div>
                </div>
                <!-- #footer-inside -->
        
            </div> 
        </div><!-- EOF:#footer -->
        
        <div id="subfooter" class="clearfix">
            <div class="container_12">
                
                <!-- #subfooter-inside -->
                <div id="subfooter-inside" class="clearfix">
                    <div class="grid_4">
                    </div>
                    <div class="grid_4">
                    </div>
                    <div class="grid_4">
                    </div>
                </div>
                <!-- EOF: #subfooter-inside -->
            
            </div>
        </div><!-- EOF:#footer-bottom -->
    
      </body>
    </html>