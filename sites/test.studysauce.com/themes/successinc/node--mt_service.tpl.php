<div id="node-<?php print $node->nid; ?>" class="<?php print $classes; ?> clearfix"<?php print $attributes; ?>>

  <?php print render($title_prefix); ?>
  <?php if (!$page): ?>
    <h2<?php print $title_attributes; ?>><a href="<?php print $node_url; ?>"><?php print $title; ?></a></h2>
  <?php endif; ?>
  <?php print render($title_suffix); ?>
  
  <?php print render($content['field_image']); ?>
  
  <?php $content_meta_status = "disabled-meta"; ?>
  <?php if ($user_picture || $display_submitted || !empty($content['field_tags']) || 
 (module_exists('comment') && user_access('post comments') && ($node->comment == COMMENT_NODE_OPEN || ($node->comment == COMMENT_NODE_CLOSED && $node->comment_count > 0)))): ?>
  <?php $content_meta_status = "enabled-meta"; ?>
  <?php endif; ?>
    
  <div class="content-wrapper <?php print $content_meta_status; ?> clearfix">
	<div class="content-meta">
		
		<?php print $user_picture; ?>
		
		<?php if ($display_submitted): ?>
			<div class="submitted">
			<?php
            $custom_date = format_date($node->created, 'custom', 'F d, Y');
            print t('by !username<br/>on !datetime', array('!username' => $name, '!datetime' => $custom_date));
            ?>
			</div>
		<?php endif; ?>
		
		<?php if (module_exists('comment') && user_access('post comments') && ($node->comment == COMMENT_NODE_OPEN || ($node->comment == COMMENT_NODE_CLOSED && $node->comment_count > 0))): ?>
			<div class="comment-count">
	        <?php print $node->comment_count; print t(' comments'); ?>
	        </div>
        <?php endif;?>
        
		<?php if (!empty($content['field_tags'])): ?><?php print render($content['field_tags']); ?><?php endif; ?>

	</div>
	<div class="content clearfix"<?php print $content_attributes; ?>>
		<?php
		  // We hide the comments and links now so that we can render them later.
		  hide($content['comments']);
		  hide($content['links']);
		  print render($content);
		?>
		<div class="links">
		<?php print render($content['links']); ?>
		</div>
		
	</div>
  
  </div>
  <?php print render($content['comments']); ?>

</div>
