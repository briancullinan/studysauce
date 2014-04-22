<?php
if (empty($studyConnections)) {
    ?>
    <div class="students_only not-connected">
        <?php
        $node = node_load(157);
        webform_node_view($node, 'full');
        print theme_webform_view($node->content); ?>
        <h3 class="parents_only">Recommend to your friends</h3>

        <h3 class="students_only">Recommend to your classmates</h3>

        <p style="margin-bottom:0;" class="like-us"><a target="_blank"
                                                       href="https://www.facebook.com/sharer/sharer.php?u=https://www.studysauce.com">
                &nbsp;</a>
            <a href="https://plus.google.com/share?url=https://www.studysauce.com">&nbsp;</a>
            <a href="https://twitter.com/intent/tweet?source=webclient&text=Check+out+www.StudySauce.com+for+your+online+study+assistant.">
                &nbsp;</a></p>
    </div>
    <div class="parents_only not-connected">
        <?php
        $node = node_load(250);
        webform_node_view($node, 'full');
        print theme_webform_view($node->content); ?>
        <h3 class="parents_only">Recommend to your friends</h3>

        <h3 class="students_only">Recommend to your classmates</h3>

        <p style="margin-bottom:0;" class="like-us"><a target="_blank"
                                                       href="https://www.facebook.com/sharer/sharer.php?u=https://www.studysauce.com">
                &nbsp;</a>
            <a href="https://plus.google.com/share?url=https://www.studysauce.com">&nbsp;</a>
            <a href="https://twitter.com/intent/tweet?source=webclient&text=Check+out+www.StudySauce.com+for+your+online+study+assistant.">
                &nbsp;</a></p>
    </div>
<?php } else { ?>
    <div>
        <h2>Your account is connected to:</h2>
        <?php
        $studyConnections = studysauce_get_connections();
        foreach ($studyConnections as $i => $conn) {
            if (isset($conn->field_first_name['und'][0]['value']) && isset($conn->field_last_name['und'][0]['value']))
                $displayName = $conn->field_first_name['und'][0]['value'] . ' ' . $conn->field_last_name['und'][0]['value'];
            else
                $displayName = $conn->mail;

            print '<span class="' . (isset($conn->uid) ? 'connected' : 'not-connected') . '">' . $displayName . ' (' . $conn->mail . ')</span>';
        }
        ?>
        <h3 class="parents_only" style="margin-right:0;">Recommend to your friends</h3>
        <h3 class="students_only" style="margin-right:0;">Recommend to your classmates</h3>

        <p style="margin-bottom:0;margin-right:0;" class="like-us"><a target="_blank"
                                                                      href="https://www.facebook.com/sharer/sharer.php?u=https://www.studysauce.com">
                &nbsp;</a>
            <a href="https://plus.google.com/share?url=https://www.studysauce.com">&nbsp;</a>
            <a href="https://twitter.com/intent/tweet?source=webclient&text=Check+out+www.StudySauce.com+for+your+online+study+assistant.">
                &nbsp;</a></p>
    </div>
<?php } ?>