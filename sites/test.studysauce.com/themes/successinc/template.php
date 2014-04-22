<?php
/**
 * Return a themed breadcrumb trail.
 *
 * @param $breadcrumb
 *   An array containing the breadcrumb links.
 * @return
 *   A string containing the breadcrumb output.
 */
function successinc_breadcrumb($variables){

  $breadcrumb = $variables['breadcrumb'];
  $breadcrumb_separator=theme_get_setting('breadcrumb_separator','successinc');
  
  if (!empty($breadcrumb)) {
    $breadcrumb[] = drupal_get_title();
	return '<div class="breadcrumb">' . implode(' <span class="breadcrumb-separator">' . $breadcrumb_separator . '</span>', $breadcrumb) . '</div>';
  }
}

function successinc_preprocess_page(&$variables)
{
    // if this is a panel page, add template suggestions
    if(($panel_page = page_manager_get_current_page())) {
        $layout = explode(':', $panel_page['handler']->conf['display']->layout);

        // add a generic suggestion for all panel pages
        $variables['theme_hook_suggestions'][] = 'page__panel';

        // add the panel page machine name to the template suggestions
        $variables['theme_hook_suggestions'][] = 'page__' . $panel_page['name'];
        if(isset($layout[1]))
            $variables['theme_hook_suggestions'][] = 'page__panel_' . $layout[1];

        //add a body class for good measure
        $body_classes[] = 'page-panel';
    }
}

function successinc_preprocess_block(&$variables) {
	
  $variables['classes_array'][]='clearfix';

}

function _studysauce_reminders_date_sort($a, $b)
{
    $a_weight = (is_array($a) && isset($a['field_due_date']['und'][0]['#default_value']['value'])
    && !isset($a['is_new']) && !isset($a['is_hidden'])
        ? strtotime($a['field_due_date']['und'][0]['#default_value']['value'])
        : (is_array($a) && isset($a['is_hidden']) ? 10000000000000 : -10000000000000));
    $b_weight = (is_array($b) && isset($b['field_due_date']['und'][0]['#default_value']['value'])
    && !isset($b['is_new']) && !isset($b['is_hidden'])
        ? strtotime($b['field_due_date']['und'][0]['#default_value']['value'])
        : (is_array($b) && isset($b['is_hidden']) ? 10000000000000 : -10000000000000));
    return $a_weight - $b_weight;
}

/**
 * Override or insert variables into the html template.
 */
function successinc_preprocess_html(&$variables) {

	if (empty($variables['page']['banner'])) {
		$variables['classes_array'][] = 'no-banner';
	}
	
	$color_scheme = theme_get_setting('color_scheme');
	
	if ($color_scheme != 'default') {
		drupal_add_css(drupal_get_path('theme', 'successinc') . '/style-' .$color_scheme. '.css', array('group' => CSS_THEME, 'type' => 'file'));
	}
	
	$protocol = theme_get_setting('protocol');

	if (theme_get_setting('sitename_font_family')=='sff-1'  ||
		theme_get_setting('headings_font_family')=='hff-1') {
		drupal_add_css(drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/merriweather-font.css', array('group' => CSS_THEME, 'type' => 'file'));
	}
	
	if (theme_get_setting('sitename_font_family')=='sff-2'  ||
		theme_get_setting('headings_font_family')=='hff-2' || 
		theme_get_setting('slogan_font_family')=='slff-2' ||
		theme_get_setting('paragraph_font_family')=='pff-2') {
		drupal_add_css(drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/sourcesanspro-font.css', array('group' => CSS_THEME, 'type' => 'file'));
	}
	
	if (theme_get_setting('sitename_font_family')=='sff-3'  ||
		theme_get_setting('headings_font_family')=='hff-3' ||
		theme_get_setting('slogan_font_family')=='slff-4' ||
		theme_get_setting('paragraph_font_family')=='pff-4') {
		drupal_add_css(drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/exo-font.css', array('group' => CSS_THEME, 'type' => 'file'));
	}
	
	if (theme_get_setting('sitename_font_family')=='sff-4'  ||
		theme_get_setting('headings_font_family')=='hff-4' ||
		theme_get_setting('slogan_font_family')=='slff-5' ||
		theme_get_setting('paragraph_font_family')=='pff-5') {
		drupal_add_css(drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/titilliumweb-font.css', array('group' => CSS_THEME, 'type' => 'file'));
	}
	
	if (theme_get_setting('sitename_font_family')=='sff-5'  ||
		theme_get_setting('headings_font_family')=='hff-5') {
		drupal_add_css(drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/adventpro-font.css', array('group' => CSS_THEME, 'type' => 'file'));
	}
	
	if (theme_get_setting('sitename_font_family')=='sff-6'  ||
		theme_get_setting('headings_font_family')=='hff-6' ||
		theme_get_setting('slogan_font_family')=='slff-7' ||
		theme_get_setting('paragraph_font_family')=='pff-7') {
		drupal_add_css(drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/ubuntu-font.css', array('group' => CSS_THEME, 'type' => 'file'));
	}
	
	if (theme_get_setting('sitename_font_family')=='sff-7'  ||
		theme_get_setting('headings_font_family')=='hff-7') {
		drupal_add_css(drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/playfairdisplaysc-font.css', array('group' => CSS_THEME, 'type' => 'file'));
	}
	
	if (theme_get_setting('sitename_font_family')=='sff-10'  ||
		theme_get_setting('headings_font_family')=='hff-10' ||
		theme_get_setting('slogan_font_family')=='slff-11' ||
		theme_get_setting('paragraph_font_family')=='pff-11') {
		drupal_add_css(drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/gentiumbookbasic-font.css', array('group' => CSS_THEME, 'type' => 'file'));
	}
	
	if (theme_get_setting('sitename_font_family')=='sff-11'  ||
		theme_get_setting('headings_font_family')=='hff-11') {
		drupal_add_css(drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/noticiatext-font.css', array('group' => CSS_THEME, 'type' => 'file'));
	}
	
	if (theme_get_setting('slogan_font_family')=='slff-1'  ||
		theme_get_setting('paragraph_font_family')=='pff-1') {
		drupal_add_css(drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/lato-font.css', array('group' => CSS_THEME, 'type' => 'file'));
	}
	
	if (theme_get_setting('slogan_font_family')=='slff-3'  ||
		theme_get_setting('paragraph_font_family')=='pff-3') {
		drupal_add_css(drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/opensans-font.css', array('group' => CSS_THEME, 'type' => 'file'));
	}
	
	if (theme_get_setting('slogan_font_family')=='slff-6'  ||
		theme_get_setting('paragraph_font_family')=='pff-6') {
		drupal_add_css(drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/ptsans-font.css', array('group' => CSS_THEME, 'type' => 'file'));
	}

	if (theme_get_setting('slogan_font_family')=='slff-8'  ||
		theme_get_setting('paragraph_font_family')=='pff-8') {
		drupal_add_css(drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/amaranth-font.css', array('group' => CSS_THEME, 'type' => 'file'));
	}
	
	if (theme_get_setting('slogan_font_family')=='slff-12'  ||
		theme_get_setting('paragraph_font_family')=='pff-12') {
		drupal_add_css(drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/alegreya-font.css', array('group' => CSS_THEME, 'type' => 'file'));
	}
	
	if (theme_get_setting('slogan_font_family')=='slff-13'  ||
		theme_get_setting('paragraph_font_family')=='pff-13') {
		drupal_add_css(drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/josefinslab-font.css', array('group' => CSS_THEME, 'type' => 'file'));
	}
	
	drupal_add_css(drupal_get_path('theme', 'successinc') . '/fonts/' . $protocol . '/ptserif-font.css', array('group' => CSS_THEME, 'type' => 'file'));
	
	drupal_add_css(path_to_theme() . '/ie9.css', array('group' => CSS_THEME, 'browsers' => array('IE' => '(IE 9)&(!IEMobile)', '!IE' => FALSE), 'preprocess' => FALSE));
	drupal_add_css(path_to_theme() . '/css/layout-ie.css', array('group' => CSS_THEME, 'browsers' => array('IE' => '(lt IE 9)&(!IEMobile)', '!IE' => FALSE), 'preprocess' => FALSE));
	drupal_add_css(path_to_theme() . '/ie.css', array('group' => CSS_THEME, 'browsers' => array('IE' => '(lt IE 9)&(!IEMobile)', '!IE' => FALSE), 'preprocess' => FALSE));
	
	// Adding local.css file for CSS overrides
	drupal_add_css(drupal_get_path('theme', 'successinc') . '/local.css', array('group' => CSS_THEME, 'type' => 'file'));
	
}

function successinc_preprocess_maintenance_page(&$variables) {
	$color_scheme = theme_get_setting('color_scheme');
	
	if ($color_scheme != 'default') {
		drupal_add_css(drupal_get_path('theme', 'successinc') . '/style-' .$color_scheme. '.css', array('group' => CSS_THEME, 'type' => 'file'));
	}
}

/**
 * Override or insert variables into the html template.
 */
function successinc_process_html(&$vars) {

  global $user;
  $user = user_load($user->uid);
  $classes = explode(' ', $vars['classes']);
  $classes[] = theme_get_setting('sitename_font_family');
  $classes[] = theme_get_setting('slogan_font_family');
  $classes[] = theme_get_setting('headings_font_family');
  $classes[] = theme_get_setting('paragraph_font_family');
  if(arg(1) == '67' || arg(1) == '90' || arg(1) == '103' || arg(1) == '104')
      $classes[] = 'page-node-landing';
  if(!empty($user->field_parent_student) && $user->field_parent_student['und'][0]['value'] == 'student')
      $classes[] = 'student';
  if(!empty($user->field_parent_student) && $user->field_parent_student['und'][0]['value'] == 'parent')
      $classes[] = 'parent';
  $vars['classes'] = trim(implode(' ', $classes));
 
}

function successinc_page_alter($page) {
	// <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1"/>
	$viewport = array(
		'#type' => 'html_tag',
		'#tag' => 'meta',
		'#attributes' => array(
		'name' =>  'viewport',
		'content' =>  'width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no'
		)
	);
	drupal_add_html_head($viewport, 'viewport');
}

function successinc_form_alter(&$form, &$form_state, $form_id) {
    $color_scheme = theme_get_setting('color_scheme');
    $color_folder = '';
    if ($color_scheme != 'default') { $color_folder = '/' . theme_get_setting('color_scheme'); }

	if ($form_id == 'search_block_form') {

    unset($form['search_block_form']['#title']);

    $form['search_block_form']['#type'] = 'textfield';
    $form['search_block_form']['#title_display'] = 'invisible';
	$form_default = t('Search        ');
    $form['search_block_form']['#attributes'] = array('placeholder' => $form_default );

    $form['actions']['submit'] = array('#type' => 'image_button', '#src' => base_path() . path_to_theme() . '/images'. $color_folder . '/search-button.png');
  }

    if($form_id == 'user_login_block') {
        $form['#attributes'] = array('class' => 'container-inline');
        unset($form['name']['#title']);
        $form['name']['#title_display'] = 'invisible';
        unset($form['pass']['#title']);
        $form['pass']['#title_display'] = 'invisible';

        $username_default = t('Email      ');
        $form['name']['#attributes'] = array('placeholder' => $username_default );

        $password_default = t('Password       ');
        $form['pass']['#attributes'] = array('placeholder' => $password_default );

        $form['actions']['submit'] = array('#type' => 'image_button', '#src' => base_path() . path_to_theme() . '/images'. $color_folder . '/login-right.png');
    }

    if($form_id == 'user_login')
    {
        $form['actions']['#attributes']['class'][] = 'highlighted-link';
        $form['actions']['submit']['#attributes']['class'][] = 'more';
    }

    if($form_id == 'user_register_form')
    {
        drupal_set_title(t('Create new account'));
        $form['actions']['#attributes']['class'][] = 'highlighted-link';
        $form['actions']['submit']['#attributes']['class'][] = 'more';
        $form['actions']['submit']['#value'] = 'Get started';
    }

    if($form_id == 'schedule_node_form')
    {
        $form['actions']['#attributes']['class'][] = 'highlighted-link';
        $form['actions']['submit']['#attributes']['class'][] = 'more';
        $form['actions']['submit']['#value'] = 'Save';
    }

    if($form_id == 'user_profile_form')
    {
        $form['actions']['#attributes']['class'][] = 'highlighted-link';
        $form['actions']['submit']['#attributes']['class'][] = 'more';
    }
}

function successinc_panels_flexible($vars)
{
    $css_id = $vars['css_id'];
    $content = $vars['content'];
    $settings = $vars['settings'];
    $display = $vars['display'];
    $layout = $vars['layout'];
    $handler = $vars['renderer'];

    panels_flexible_convert_settings($settings, $layout);

    $renderer = panels_flexible_create_renderer(FALSE, $css_id, $content, $settings, $display, $layout, $handler);

    if($layout['name'] == 'flexible:clone_of_full' ||
        $layout['name'] == 'flexible:full');
        // skip css
    else
    {
        // CSS must be generated because it reports back left/middle/right
        // positions.
        $css = panels_flexible_render_css($renderer);

        if (!empty($renderer->css_cache_name) && empty($display->editing_layout)) {
            ctools_include('css');
            // Generate an id based upon rows + columns:
            $filename = ctools_css_retrieve($renderer->css_cache_name);
            if (!$filename) {
                $filename = ctools_css_store($renderer->css_cache_name, $css, FALSE);
            }

            // Give the CSS to the renderer to put where it wants.
            if ($handler) {
                $handler->add_css($filename, 'module', 'all', FALSE);
            }
            else {
                drupal_add_css($filename);
            }
        }
        else {
            // If the id is 'new' we can't reliably cache the CSS in the filesystem
            // because the display does not truly exist, so we'll stick it in the
            // head tag. We also do this if we've been told we're in the layout
            // editor so that it always gets fresh CSS.
            drupal_add_css($css, array('type' => 'inline', 'preprocess' => FALSE));
        }

        // Also store the CSS on the display in case the live preview or something
        // needs it
        $display->add_css = $css;

    }

    $output = "<div class=\"panel-flexible " . $renderer->base['canvas'] . " clearfix\" $renderer->id_str>\n";
    $output .= "<div class=\"panel-flexible-inside " . $renderer->base['canvas'] . "-inside\">\n";

    $output .= panels_flexible_render_items($renderer, $settings['items']['canvas']['children'], $renderer->base['canvas']);

    // Wrap the whole thing up nice and snug
    $output .= "</div>\n</div>\n";

    return $output;

}

/**
 * Add Javascript for enable/disable scrollTop action
 */
if (theme_get_setting('scrolltop_display')) {

	drupal_add_js('jQuery(document).ready(function($) { 
	$(window).scroll(function() {
		if($(this).scrollTop() != 0) {
			$("#toTop").fadeIn();	
		} else {
			$("#toTop").fadeOut();
		}
	});
	
	$("#toTop").click(function() {
		$("body,html").animate({scrollTop:0},800);
	});	
	
	});',
	array('type' => 'inline', 'scope' => 'header'));

}

global $user;
drupal_add_css(drupal_get_path('theme', 'successinc') .'/header.css');

if(drupal_is_front_page() && $user->uid != 0)
{
//    drupal_add_css(drupal_get_path('module', 'date') .'/date_api/date.css');
//    drupal_add_js(drupal_get_path('module', 'date') .'/date_popup/date_popup.js');
    drupal_add_js(drupal_get_path('module', 'date') .'/date_popup/jquery.timeentry.pack.js');
    drupal_add_library('system', 'ui.datepicker');
    drupal_add_library('system', 'ui.draggable');
    drupal_add_library('system', 'ui.resizable');
    drupal_add_library('system', 'ui.droppable');
    drupal_add_library('system', 'ui.sortable');
    drupal_add_library('system', 'ui.progressbar');

    drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/skrollr.js');
    drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/flipclock/libs/prefixfree.min.js');
    drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/jquery.scrollintoview.js');
    drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/sauce.js');
    drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/d3.v3.js');
    drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/jquery.tipsy.js');
    drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/metrics.js');
    drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/fullcalendar/fullcalendar.js');
    drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/checkin.js');
    drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/plans.js');
    drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/profile.js');
    drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/schedule.js');
    drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/goals.js');
    drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/dates.js');
    drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/user.js');
    drupal_add_js(drupal_get_path('theme', 'successinc') .'/js/partner.js');
    drupal_add_js(libraries_get_path('plupload') . '/js/plupload.full.js');

    drupal_add_css(drupal_get_path('theme', 'successinc') .'/js/fullcalendar/fullcalendar.css');
    drupal_add_css(drupal_get_path('theme', 'successinc') .'/css/flipclock.css');
    drupal_add_css(drupal_get_path('theme', 'successinc') .'/css/tipsy.css');
    drupal_add_css(drupal_get_path('theme', 'successinc') .'/menu.css');
    drupal_add_css(drupal_get_path('theme', 'successinc') .'/home.css');
    drupal_add_css(drupal_get_path('theme', 'successinc') .'/checkin.css');
    drupal_add_css(drupal_get_path('theme', 'successinc') .'/goals.css');
    drupal_add_css(drupal_get_path('theme', 'successinc') .'/awards.css');
    drupal_add_css(drupal_get_path('theme', 'successinc') .'/deadlines.css');
    drupal_add_css(drupal_get_path('theme', 'successinc') .'/schedule.css');
    drupal_add_css(drupal_get_path('theme', 'successinc') .'/profile.css');
    drupal_add_css(drupal_get_path('theme', 'successinc') .'/metrics.css');
    drupal_add_css(drupal_get_path('theme', 'successinc') .'/plans.css');
    drupal_add_css(drupal_get_path('theme', 'successinc') .'/partner.css');
    drupal_add_css(drupal_get_path('theme', 'successinc') .'/invite.css');
    drupal_add_css(drupal_get_path('theme', 'successinc') .'/user-parent-student.css');
}
elseif(drupal_is_front_page() || arg(0) == 'parents' || arg(0) == 'students' ||
    arg(0) == 'parents2' || arg(0) == 'students2')
{
    drupal_add_css(drupal_get_path('theme', 'successinc') .'/front.css');

    $parallax =<<<EOJS
jQuery(document).ready(function () {
    jQuery(window).scroll(function () {
        var distance = jQuery('.header-wrapper').height(),
            offset = Math.min(jQuery(window).scrollTop() / distance, 1);
        jQuery('.header-wrapper').stop().animate({'background-position-y': -(100 - (offset * 100))}, 50, 'linear');

/*
        var pageDistance = jQuery(window).height() + jQuery('.page').outerHeight(),
            pageOffset = Math.max(Math.min((jQuery(window).scrollTop() - jQuery('.page').offset().top + jQuery(window).height()) / pageDistance, 1), 0);
        jQuery('.page').stop().animate({'background-position-y' : -200 - (pageOffset * 100)}, 50, 'linear');
        */
    });
});
EOJS;


    drupal_add_js($parallax, array('type' => 'inline', 'scope' => 'header'));

    // experiments code
    if(arg(0) == 'parents')
    {
        $exp = <<<EOJS
// <!-- Google Analytics Content Experiment code -->
function utmx_section(){}function utmx(){}(function(){var
k='76175055-0',d=document,l=d.location,c=d.cookie;
if(l.search.indexOf('utm_expid='+k)>0)return;
function f(n){if(c){var i=c.indexOf(n+'=');if(i>-1){var j=c.
indexOf(';',i);return escape(c.substring(i+n.length+1,j<0?c.
length:j))}}}var x=f('__utmx'),xx=f('__utmxx'),h=l.hash;d.write(
'<sc'+'ript src="'+'http'+(l.protocol=='https:'?'s://ssl':
'://www')+'.google-analytics.com/ga_exp.js?'+'utmxkey='+k+
'&utmx='+(x?x:'')+'&utmxx='+(xx?xx:'')+'&utmxtime='+new Date().
valueOf()+(h?'&utmxhash='+escape(h.substr(1)):'')+
'" type="text/javascript" charset="utf-8"><\/sc'+'ript>')})();
// <!-- End of Google Analytics Content Experiment code -->

EOJS;

        drupal_add_js($exp, array('type' => 'inline', 'scope' => 'header'));
        drupal_add_js("utmx('url','A/B');", array('type' => 'inline', 'scope' => 'header'));
    }

    if(arg(0) == 'students')
    {
        $exp = <<<EOJS
// <!-- Google Analytics Content Experiment code -->
function utmx_section(){}function utmx(){}(function(){var
k='76175055-1',d=document,l=d.location,c=d.cookie;
if(l.search.indexOf('utm_expid='+k)>0)return;
function f(n){if(c){var i=c.indexOf(n+'=');if(i>-1){var j=c.
indexOf(';',i);return escape(c.substring(i+n.length+1,j<0?c.
length:j))}}}var x=f('__utmx'),xx=f('__utmxx'),h=l.hash;d.write(
'<sc'+'ript src="'+'http'+(l.protocol=='https:'?'s://ssl':
'://www')+'.google-analytics.com/ga_exp.js?'+'utmxkey='+k+
'&utmx='+(x?x:'')+'&utmxx='+(xx?xx:'')+'&utmxtime='+new Date().
valueOf()+(h?'&utmxhash='+escape(h.substr(1)):'')+
'" type="text/javascript" charset="utf-8"><\/sc'+'ript>')})();
// <!-- End of Google Analytics Content Experiment code -->

EOJS;

        drupal_add_js($exp, array('type' => 'inline', 'scope' => 'header'));
        drupal_add_js("utmx('url','A/B');", array('type' => 'inline', 'scope' => 'header'));
    }
}

//EOF:Javascript

drupal_add_html_head_link(array(
    'rel' => 'image_src',
    'href' => url(drupal_get_path('theme', 'successinc') . '/images/studysauce-google-page.png', array('absolute' => true))
));

drupal_add_html_head(array(
    '#type' => 'html_tag',
    '#tag' => 'meta',
    '#attributes' => array(
        'name' =>  'description',
        'content' => 'Study Sauce teaches you the most effective study methods by automatically detecting your good and bad study behaviors.',
    )), 'facebook_description_meta');

drupal_add_html_head(array(
    '#type' => 'html_tag',
    '#tag' => 'meta',
    '#attributes' => array(
        'property' =>  'og:image',
        'content' => url(drupal_get_path('theme', 'successinc') . '/images/studysauce-google-page.png', array('absolute' => true)),
    )), 'facebook_image_meta');



