<?php

if(!isset($account))
{
    global $user;
    $account = user_load($user->uid);
}
$strategies = studysauce_get_strategies($account);
// sort strategies by modified date
if(!isset($account))
{
    global $user;
    $account = user_load($user->uid);
}

?><h2>Uploaded content</h2><?php

$result = array();
$dates = array();
$query = new EntityFieldQuery();
$nodes = $query->entityCondition('entity_type', 'node')
    ->propertyCondition('type', 'strategies')
    ->propertyCondition('title', isset($account->mail) ? $account->mail : '')
    ->propertyCondition('status', 1)
    ->range(0, 1)
    ->execute();
if (!empty($nodes['node']))
{
    $nodes = array_keys($nodes['node']);
    $nid = array_shift($nodes);
    $node = node_load($nid);
    foreach(array('active', 'other', 'spaced', 'teach', 'prework') as $i => $strategy)
    {
        $field = 'field_' . $strategy . '_strategies';
        $strategies = $node->$field;
        if(isset($strategies['und'][0]['value']))
        {
            $strategies = entity_load('field_collection_item', array_map(function ($x) {return $x['value'];}, $strategies['und']));
            foreach($strategies as $eid => $entity)
            {
                if(!isset($entity->field_class_name['und'][0]['value']))
                    continue;

                $name = $entity->field_class_name['und'][0]['value'];

                // get date of last revision
                $rev = db_select('field_revision_field_' . $strategy . '_strategies', 's')
                    ->fields('s', array('revision_id'))
                    ->condition('field_' . $strategy . '_strategies_revision_id', $entity->revision_id, '=')
                    ->execute()->fetchAssoc();
                // get latest node revision date
                if(isset($rev['revision_id']))
                {
                    $nodeTime = db_select('node_revision', 's')
                        ->fields('s', array('timestamp'))
                        ->condition('vid', $rev['revision_id'], '=')
                        ->execute()->fetchAssoc();
                    while(isset($dates[$nodeTime['timestamp']]))
                        $nodeTime['timestamp']++;
                    $dates[$nodeTime['timestamp']] = array($strategy, $name);
                }
                if(!isset($nodeTime['timestamp']))
                {
                    // use node created time
                    while(isset($dates[$node->created]))
                        $node->created++;
                    $dates[$node->created] = array($strategy, $name);
                }

                if($strategy == 'active')
                {
                    $result[$name][$strategy]['skim'] = isset($entity->field_skim['und'][0]['value'])
                        ? $entity->field_skim['und'][0]['value']
                        : '';
                    $result[$name][$strategy]['why'] = isset($entity->field_why['und'][0]['value'])
                        ? $entity->field_why['und'][0]['value']
                        : '';
                    $result[$name][$strategy]['questions'] = isset($entity->field_questions['und'][0]['value'])
                        ? $entity->field_questions['und'][0]['value']
                        : '';
                    $result[$name][$strategy]['summarize'] = isset($entity->field_summarize['und'][0]['value'])
                        ? $entity->field_summarize['und'][0]['value']
                        : '';
                    $result[$name][$strategy]['exam'] = isset($entity->field_exam['und'][0]['value'])
                        ? $entity->field_exam['und'][0]['value']
                        : '';
                    $result[$name][$strategy]['default'] = isset($entity->field_default['und'][0]['value'])
                        ? $entity->field_default['und'][0]['value']
                        : false;
                }
                if($strategy == 'other')
                {
                    $result[$name][$strategy]['notes'] = isset($entity->field_notes['und'][0]['value'])
                        ? $entity->field_notes['und'][0]['value']
                        : '';
                }
                elseif($strategy == 'teach')
                {
                    $result[$name][$strategy]['title'] = isset($entity->field_title['und'][0]['value'])
                        ? $entity->field_title['und'][0]['value']
                        : '';
                    $result[$name][$strategy]['notes'] = isset($entity->field_notes['und'][0]['value'])
                        ? $entity->field_notes['und'][0]['value']
                        : '';
                    if(isset($entity->field_teaching['und'][0]['fid']))
                    {
                        $result[$name][$strategy]['uploads'][0]['fid'] = $entity->field_teaching['und'][0]['fid'];
                        $result[$name][$strategy]['uploads'][0]['thumbnail'] = $entity->field_teaching['und'][0]['thumbnail'];
                        $result[$name][$strategy]['uploads'][0]['uri'] = image_style_url('achievement', $entity->field_teaching['und'][0]['thumbnailfile']->uri);

                        // Load the derived files
                        $outputs = db_select('video_output', 'vo')
                            ->fields('vo')
                            ->condition('vo.original_fid', array($entity->field_teaching['und'][0]['fid']), 'IN')
                            ->execute()->fetchAllAssoc('output_fid');
                        foreach ($outputs as $outputfid => $output) {
                            if ($output->original_fid == $entity->field_teaching['und'][0]['fid']) {
                                $file = file_load($output->output_fid);
                                if($file->filesize > 0)
                                    $result[$name][$strategy]['uploads'][0]['play'] = check_plain(file_create_url($file->uri));
                            }
                        }
                    }
                }
                elseif($strategy == 'spaced')
                {
                    $result[$name][$strategy]['notes'] = isset($entity->field_notes['und'][0]['value'])
                        ? $entity->field_notes['und'][0]['value']
                        : '';
                    $result[$name][$strategy]['review'] = isset($entity->field_review['und'][0]['value'])
                        ? implode(',', array_map(function ($x) { return $x['value']; }, $entity->field_review['und']))
                        : '';
                }
                elseif($strategy == 'prework')
                {
                    $result[$name][$strategy]['notes'] = isset($entity->field_notes['und'][0]['value'])
                        ? $entity->field_notes['und'][0]['value']
                        : '';
                    $result[$name][$strategy]['prepared'] = isset($entity->field_prepared['und'][0]['value'])
                        ? implode(',', array_map(function ($x) { return $x['value']; }, $entity->field_prepared['und']))
                        : '';
                }
            }
        }
    }

    ksort($dates, SORT_DESC);

}

if(!empty($dates))
{
    // sort strategies by date and display strategies read-only
    foreach($dates as $t => $s)
    {
        list($strategy, $name) = $s;
        if($strategy == 'active')
        {
            ?>
            <h3>Active reading - Follow the guide below to better retain what you are reading.</h3>
            <h4>Before reading:</h4>
            <label>Take no more than 2 minutes to skim the reading. What is the topic?</label>
            <textarea name="strategy-skim"><?php print htmlspecialchars($result[$name][$strategy]['skim'], ENT_QUOTES); ?></textarea>
            <label>Why am I being asked to read this at this point in the class?</label>
            <textarea name="strategy-why"><?php print htmlspecialchars($result[$name][$strategy]['why'], ENT_QUOTES); ?></textarea>
            <h4>During reading:</h4>
            <label>What questions do I have as I am reading?</label>
            <textarea name="strategy-questions"><?php print htmlspecialchars($result[$name][$strategy]['questions'], ENT_QUOTES); ?></textarea>
            <h4>After reading:</h4>
            <label>Please summarize the reading in a few paragraphs (less than 1 page).  What are the 1 or 2 most important ideas from the reading?</label>
            <textarea name="strategy-summarize"><?php print htmlspecialchars($result[$name][$strategy]['summarize'], ENT_QUOTES); ?></textarea>
            <label>What possible exam questions will result from this reading?</label>
            <textarea name="strategy-exam"><?php print htmlspecialchars($result[$name][$strategy]['exam'], ENT_QUOTES); ?></textarea>
            <?php
        }
        elseif($strategy == 'teach')
        {
            ?>
            <h3>Teach - Upload a 1 min video explaining your assignment</h3>
            <div class="plupload">
                <div class="plup-list-wrapper">
                    <ul class="plup-list clearfix ui-sortable">
                        <?php if(isset($result[$name][$strategy]['uploads'][0]['uri'])): ?>
                            <li>
                                <div class="plup-thumb-wrapper">
                                    <?php if(isset($result[$name][$strategy]['uploads'][0]['play'])): ?>
                                    <video width="184" height="184" preload="auto" controls="controls" poster="<?php print $result[$name][$strategy]['uploads'][0]['uri']; ?>">
                                        <source src="<?php print $result[$name][$strategy]['uploads'][0]['play']; ?>" type="video/webm" />
                                        <object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" width="184" height="184" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,40,0">
                                            <param name="movie" value="<?php print $result[$name][$strategy]['uploads'][0]['play']; ?>" />
                                            <param name="autoplay" value="false" />
                                            <param name="wmode" value="transparent" />
                                            <object class="video-object" type="application/x-shockwave-flash" data="<?php print $result[$name][$strategy]['uploads'][0]['play']; ?>" width="184" height="184">
                                                <?php print t('No video? Get the !plugin', array('!plugin' => l(t('Adobe Flash plugin'), url('http://get.adobe.com/flashplayer/')))); ?>
                                            </object>
                                        </object>
                                    </video>
                                    <?php endif;
                                    if(!isset($result[$name][$strategy]['uploads'][0]['play'])): ?>
                                    <img src="<?php print $result[$name][$strategy]['uploads'][0]['uri']; ?>" title="">
                                    <?php endif; ?>
                                </div>
                            </li>
                        <?php else: ?>
                            <img src="/<?php print drupal_get_path('theme', 'successinc'); ?>/images/empty-play.png" alt="Upload" />
                        <?php endif; ?>
                    </ul>
                </div>
            </div>
            <div class="strategy-notes">
                <label>Title:</label>
                <input type="text" class="form-text" name="strategy-title" value="<?php print htmlspecialchars($result[$name][$strategy]['title'], ENT_QUOTES); ?>" />
                <label>Notes:</label>
                <textarea type="text" name="strategy-notes"><?php print htmlspecialchars($result[$name][$strategy]['notes'], ENT_QUOTES); ?></textarea>
            </div>
            <?php
        }
        elseif($strategy == 'other')
        {
            ?>
            <h3>Notes:</h3>
            <textarea name="strategy-notes"><?php print htmlspecialchars($result[$name][$strategy]['notes'], ENT_QUOTES); ?></textarea>
            <?php
        }
        elseif($strategy == 'spaced')
        {
            ?>
            <h3>Spaced repetition - Commit information to your long term memory by revisiting past work.</h3>
            <h4>Instructions - We highly recommend flashcards.  Online flashcard maker Quizlet is our favorite.  Read more about spaced repetition here.</h4>
            <div class="strategy-review">
                <label>Review material from:</label>
            </div>
            <div class="strategy-notes">
                <label>Write down any notes below:</label>
                <textarea type="text" name="strategy-notes"><?php print htmlspecialchars($result[$name][$strategy]['notes'], ENT_QUOTES); ?></textarea>
            </div>
            <?php
        }
    }
}
else
{
    ?><h3>Your student has not completed this section yet.</h3><?php
}
?>
<hr style="margin-bottom:30px;" />