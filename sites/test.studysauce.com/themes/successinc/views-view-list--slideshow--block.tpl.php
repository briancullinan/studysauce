<div class="flexslider-wrapper">
	<div class="flexslider-shadow">
        <div class="flexslider">
            <ul class="slides">
            <?php foreach ($rows as $id => $row) { ?>
            <li>
            
            <?php $view = views_get_current_view();
            $nid = $view->result[$id]->nid; 
            $node = node_load($nid);
            $lang = 'und';
            
            if ($node->type=='mt_slideshow_entry') {
            
                if ($node->field_teaser_image) {
                
                    $image = image_style_url('slideshow', $node->field_teaser_image[$lang][0]['uri']); 
                    $title = $node->field_teaser_image[$lang][0]['title'];
                    $alt = $node->field_teaser_image[$lang][0]['alt']; ?>
                
                    <?php if ($node->field_slideshow_entry_path) {
                     
                    $path = $node->field_slideshow_entry_path[$lang][0]['value']; ?>
                    <div class="views-field views-field-field-teaser-image">
                        <div class="field-content">
                        <a href="<?php print url($path); ?>"><img  src="<?php print $image; ?>" title="<?php print $title; ?>" alt="<?php print $alt; ?>"/></a>
                        </div>
                    </div>
                    <?php } else { ?>
                    <div class="views-field views-field-field-teaser-image">
                        <div class="field-content">
                        <img  src="<?php print $image; ?>" title="<?php print $title; ?>" alt="<?php print $alt; ?>"/>
                        </div>
                    </div>
                    <?php } ?>
                
                <?php } ?> 
                
                <?php
                if ($node->field_teaser): ?>
                <?php $teaser = $view->style_plugin->rendered_fields[$id]['field_teaser'];
                print $teaser; ?>
                <?php endif; ?>
            
            <?php } else { print $row; } ?> 
            
            </li>
            <?php } ?>
            </ul>
        </div>
	</div> 
</div>

<?php
$effect=theme_get_setting('slideshow_effect');
$effect_time=theme_get_setting('slideshow_effect_time')*1000;
$effect_direction=theme_get_setting('slideshow_effect_direction');

drupal_add_js('jQuery(document).ready(function($) { 

	$(window).load(function() {
	
		$(".flexslider-shadow").fadeIn("slow");
	
		$(".flexslider").fadeIn("slow");
	
		$(".flexslider").flexslider({
			animation: "'.$effect.'", // Select your animation type, "fade" or "slide"
			slideDirection: "'.$effect_direction.'", // Select the sliding direction, "horizontal" or "vertical"
			slideshowSpeed: "'.$effect_time.'", // Set the speed of the slideshow cycling, in milliseconds
			pauseOnAction: false,
			controlsContainer : ".flexslider-wrapper",
			before: function($slider){
			$slider.find(".flex-caption").hide();            
			},
			after: function($slider){
			$slider.find(".flex-caption").fadeIn("slow");            
			}
		});

	});
});',
array('type' => 'inline', 'scope' => 'header'));

if (theme_get_setting('banner_background')) {

	drupal_add_js('jQuery(document).ready(function($) { 
	
		$(window).load(function() {
		var banner = document.getElementById("banner");
			if (banner != null) {
				document.getElementById("banner").className += " bg-shadow";
			}
		});
	
	});',
	array('type' => 'inline', 'scope' => 'footer'));

}
	
?>