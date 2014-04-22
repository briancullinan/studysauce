<?php
// check if user has purchased a plan
global $user;
if(!isset($orders))
{
    $orders = _studysauce_orders_by_uid($user->uid);
    $conn = studysauce_get_connections();
    foreach ($conn as $i => $c)
        $orders = array_merge($orders, _studysauce_orders_by_uid($c->uid));
}
// get event schedule
$o = end($orders);
list($events, $node, $classes, $entities) = studysauce_get_events($o ? $o->created : null);

if (count($orders) && isset($user->field_parent_student['und'][0]['value']) && $user->field_parent_student['und'][0]['value'] == 'student'):

    // on mobile only show event between this week, hide everything else unless the user clicks View historic
    $startWeek = strtotime(date("Y-m-d", strtotime('this week', time()))) - 86400;
    $endWeek = $startWeek + 604800 - 86400;
    //$dotwStr = date('l', strtotime($event['start']));
    ?>

    <h2><?php print (isset($user->field_first_name['und'][0]['value']) ? ('Personalized study plan for ' . $user->field_first_name['und'][0]['value']) : 'Your personalized study plan'); ?></h2>
    <div id="calendar" class="full-only"></div>
    <script type="text/javascript"> window.planEvents = <?php print json_encode($events); ?>; </script>
    <div class="sort-by">
        <label>Sort by: </label>
        <input type="radio" id="schedule-by-date" name="schedule-by" checked="checked"><label for="schedule-by-date">Date</label>&nbsp;
        <input type="radio" id="schedule-by-class" name="schedule-by"><label for="schedule-by-class">Class</label>
        <input type="checkbox" id="schedule-historic"><label for="schedule-historic">View historical</label>
    </div>
    <?php
    $first = true;
    $headStr = '';
    $classes[''] = 'Nonacademic';
    foreach ($events as $eid => $event)
    {
        // TODO: should we allow notes for class events?
        if(strpos($event['className'], 'class-event') !== false ||
            strpos($event['className'], 'holiday-event') !== false)
            continue;

        $time = strtotime($event['start']);
        if($headStr != date('j F', $time))
        {
            $headStr = date('j F', $time);
            ?><div class="head <?php print ($time < strtotime(date('Y/m/d')) - 86400 ? 'hide' : ''); ?>"><?php print $headStr; ?></div><?
        }
        $classI = '';
        $cid = '';
        if(preg_match('/class([0-9]+)(\s|$)/i', $event['className'], $matches))
        {
            $classI = $matches[1];
            $cid = array_keys($classes)[$classI];
        }
        $title = (strpos($event['className'], 'deadline-event') !== false ? 'Deadline' : '') . preg_replace(
            array('/^\(P\)\s*/', '/^\(SR\)\s*/', '/' . preg_quote($classes[$cid]) . '\s*/', '/<\/?small>/'),
            array('Pre-work: ', 'Spaced repetition: ', '', ''),
            $event['title']);
        ?>
        <div class="row <?php
        print ($first && !($first = false) ? 'first' : ''); ?> <?php
        print (strpos($event['className'], 'deadline-event') !== false ? 'deadline' : ''); ?> <?php
        print ($time < strtotime(date('Y/m/d')) - 86400 ? 'hide' : ''); ?> <?php
        print (strtotime($event['start']) >= $startWeek && strtotime($event['start']) <= $endWeek ? 'mobile' : ''); ?>" id="eid-<?php print $eid; ?>">
            <div class="field-type-text field-name-field-class-name field-widget-text-textfield form-wrapper">
                <div class="read-only"><span class="class<?php print $classI; ?>">&nbsp;</span><?php print htmlspecialchars($classes[$cid], ENT_QUOTES); ?></div>
            </div>
            <div class="field-type-text field-name-field-assignment field-widget-text-textfield form-wrapper">
                <div class="read-only"><?php print htmlspecialchars($title, ENT_QUOTES); ?></div>
            </div>
            <div class="field-type-number-integer field-name-field-percent field-widget-number form-wrapper">
                <div class="read-only"><?php if(isset($event['percent'])): print $event['percent']; ?>% of grade<?php else: ?>&nbsp;<?php endif; ?></div>
            </div>
            <div class="field-type-list-boolean field-name-field-completed field-widget-options-onoff form-wrapper">
                <div class="read-only"><input type="checkbox" id="schedule-completed-<?php print $eid; ?>" />
                    <label for="schedule-completed-<?php print $eid; ?>">&nbsp;</label></div>
            </div>
        <?php if($classI !== ''): ?>
            <div class="mini-checkin">
                <a href="#class<?php print $classI; ?>" class="class<?php print $classI; ?> eid<?php print $cid; ?>"><span>&nbsp;</span><?php print htmlspecialchars($classes[$cid], ENT_QUOTES); ?></a>
                <div class="flip-counter clock flip-clock-wrapper">
                    <ul class="flip">
                        <li data-digit="0"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">0</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">0</div>
                                </div>
                            </a></li>
                        <li data-digit="1"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">1</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">1</div>
                                </div>
                            </a></li>
                        <li data-digit="2"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">2</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">2</div>
                                </div>
                            </a></li>
                        <li data-digit="3"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">3</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">3</div>
                                </div>
                            </a></li>
                        <li data-digit="4"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">4</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">4</div>
                                </div>
                            </a></li>
                        <li data-digit="5"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">5</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">5</div>
                                </div>
                            </a></li>
                        <li data-digit="6"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">6</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">6</div>
                                </div>
                            </a></li>
                        <li data-digit="7"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">7</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">7</div>
                                </div>
                            </a></li>
                        <li data-digit="8"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">8</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">8</div>
                                </div>
                            </a></li>
                        <li data-digit="9"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">9</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">9</div>
                                </div>
                            </a></li>
                    </ul>
                    <ul class="flip">
                        <li data-digit="0"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">0</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">0</div>
                                </div>
                            </a></li>
                        <li data-digit="1"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">1</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">1</div>
                                </div>
                            </a></li>
                        <li data-digit="2"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">2</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">2</div>
                                </div>
                            </a></li>
                        <li data-digit="3"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">3</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">3</div>
                                </div>
                            </a></li>
                        <li data-digit="4"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">4</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">4</div>
                                </div>
                            </a></li>
                        <li data-digit="5"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">5</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">5</div>
                                </div>
                            </a></li>
                        <li data-digit="6"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">6</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">6</div>
                                </div>
                            </a></li>
                        <li data-digit="7"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">7</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">7</div>
                                </div>
                            </a></li>
                        <li data-digit="8"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">8</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">8</div>
                                </div>
                            </a></li>
                        <li data-digit="9"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">9</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">9</div>
                                </div>
                            </a></li>
                    </ul>
                    <span class="flip-clock-divider minutes"><span class="flip-clock-dot top"></span><span class="flip-clock-dot bottom"></span></span>
                    <ul class="flip">
                        <li data-digit="0"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">0</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">0</div>
                                </div>
                            </a></li>
                        <li data-digit="1"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">1</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">1</div>
                                </div>
                            </a></li>
                        <li data-digit="2"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">2</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">2</div>
                                </div>
                            </a></li>
                        <li data-digit="3"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">3</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">3</div>
                                </div>
                            </a></li>
                        <li data-digit="4"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">4</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">4</div>
                                </div>
                            </a></li>
                        <li data-digit="5"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">5</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">5</div>
                                </div>
                            </a></li>
                    </ul>
                    <ul class="flip">
                        <li data-digit="0"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">0</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">0</div>
                                </div>
                            </a></li>
                        <li data-digit="1"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">1</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">1</div>
                                </div>
                            </a></li>
                        <li data-digit="2"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">2</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">2</div>
                                </div>
                            </a></li>
                        <li data-digit="3"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">3</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">3</div>
                                </div>
                            </a></li>
                        <li data-digit="4"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">4</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">4</div>
                                </div>
                            </a></li>
                        <li data-digit="5"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">5</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">5</div>
                                </div>
                            </a></li>
                        <li data-digit="6"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">6</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">6</div>
                                </div>
                            </a></li>
                        <li data-digit="7"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">7</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">7</div>
                                </div>
                            </a></li>
                        <li data-digit="8"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">8</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">8</div>
                                </div>
                            </a></li>
                        <li data-digit="9"><a href="#">
                                <div class="up">
                                    <div class="shadow"></div>
                                    <div class="inn">9</div>
                                </div>
                                <div class="down">
                                    <div class="shadow"></div>
                                    <div class="inn">9</div>
                                </div>
                            </a></li>
                    </ul>
                </div>
            </div>
            <div class="field-select-strategy">
                <div>
                    <label for="strategy-select">Strategy:</label>
                    <select name="strategy-select" id="strategy-select">
                        <option value="teach" selected="selected">- Change strategy -</option>
                        <option value="teach">Teach</option>
                        <option value="spaced">Spaced repetition</option>
                        <option value="active">Active reading</option>
                    </select>
                </div>
            </div>
            <div class="strategy-teach">
                <h3>Teach - Upload a 1 min video explaining your assignment</h3>
                <div class="strategy-upload"><a href="#strategy-upload"><img src="/<?php print drupal_get_path('theme', 'successinc'); ?>/images/empty-play.png" /><span>Upload video</span></a></div>
                <div class="strategy-notes">
                    <label for="strategy-title">Title:</label>
                    <input type="text" id="strategy-title" name="strategy-title" />
                    <label for="strategy-notes">Notes:</label>
                    <textarea type="text" id="strategy-notes" name="strategy-notes"></textarea>
                </div>
            </div>
            <div class="strategy-active">
                <h3>Active reading - Follow the guide below to better retain what you are reading.</h3>
                <h4>Before reading:</h4>
                <label for="strategy-topic">Take no more than 2 minutes to skim the reading. What is the topic?</label>
                <textarea name="strategy-topic" id="strategy-topic"></textarea>
                <label for="strategy-why">Why am I being asked to read this at this point in the class?</label>
                <textarea name="strategy-why" id="strategy-why"></textarea>
                <h4>During reading:</h4>
                <label for="strategy-questions">What questions do I have as I am reading?</label>
                <textarea name="strategy-questions" id="strategy-questions"></textarea>
                <h4>After reading:</h4>
                <label for="strategy-">Please summarize the reading in a few paragraphs (less than 1 page).  What are the 1 or 2 most important ideas from the reading?</label>
                <textarea name="strategy-" id="strategy-"></textarea>
                <label for="strategy-">What possible exam questions will result from this reading?</label>
                <textarea name="strategy-" id="strategy-"></textarea>
            </div>
            <div class="strategy-spaced">
                <h3>Spaced repetition - Commit information to your long term memory by revisiting past work.</h3>
                <h4>Instructions - We highly recommend flashcards.  Online flashcard maker Quizlet is our favorite.  Read more about spaced repetition here.</h4>
                <div class="strategy-review">
                    <label>Review material from:</label>
                    <input type="checkbox" name="strategy-from-1" id="strategy-from-1-<?php print $eid; ?>"><label for="strategy-from-1-<?php print $eid; ?>">4/26</label><br />
                    <input type="checkbox" name="strategy-from-2" id="strategy-from-2-<?php print $eid; ?>"><label for="strategy-from-2-<?php print $eid; ?>">4/19</label><br />
                    <input type="checkbox" name="strategy-from-3" id="strategy-from-3-<?php print $eid; ?>"><label for="strategy-from-3-<?php print $eid; ?>">4/12</label><br />
                    <input type="checkbox" name="strategy-from-4" id="strategy-from-4-<?php print $eid; ?>"><label for="strategy-from-4-<?php print $eid; ?>">3/26</label>
                </div>
                <div class="strategy-spaced-notes">
                    <label for="strategy-notes">Write down any notes below:</label>
                    <textarea type="text" id="strategy-notes" name="strategy-notes"></textarea>
                </div>
            </div>
        <?php endif;
        if($classI === ''): ?>
            <div class="strategy-other-notes">
                <h3>Notes:</h3>
                <label for="strategy-other-notes">Feel free to record any notes associated with this non-academic deadline.</label>
                <textarea name="strategy-other-notes" id="strategy-other-notes"></textarea>
            </div>
        <?php endif; ?>
        </div>
    <?php
    }
    ?>
    <p style="clear: both; margin-bottom:0; line-height:1px;">&nbsp;</p>

<?php else: ?>
    <h2 class="students_only">
        <a style="color: inherit; text-decoration: none;"
           onclick="jQuery('.lightbox').first().trigger('click'); return false;" href="#">Get a study plan</a></h2>
    <h2 class="parents_only">
        <a style="color: inherit; text-decoration: none;"
           onclick="jQuery('.lightbox').first().trigger('click'); return false;" href="#">Help your student excel with a
            custom study plan</a></h2>

    <p><a style="padding: 0px 20px; width: 70%; float: right; position: relative;"
          onclick="jQuery('.lightbox').first().trigger('click'); return false;"
          href="/[custom:files-path]/Product%2520Bigger_1.png">
            <img width="664" height="472" style="width: 100%; height: auto;"
                 src="/[custom:files-path]/Product%2520Bigger_1.png">
        </a>
        <a class="lightbox" href="/<?php print drupal_get_path('theme', 'successinc'); ?>/tour/1-Title.png" rel="tour"></a>
        <a class="lightbox" href="/<?php print drupal_get_path('theme', 'successinc'); ?>/tour/2-Tips.png" rel="tour"></a>
        <a class="lightbox" href="/<?php print drupal_get_path('theme', 'successinc'); ?>/tour/3-Science2.png" rel="tour"></a>
        <a class="lightbox" href="/<?php print drupal_get_path('theme', 'successinc'); ?>/tour/4-Science.png" rel="tour"></a>
        <a class="lightbox" href="/<?php print drupal_get_path('theme', 'successinc'); ?>/tour/5-Schedule.png" rel="tour"></a>
    </p>
    <p><a class="support" onclick="jQuery('.lightbox').first().trigger('click'); return false;" href="#">Chances are no
            one
            ever taught you how to study. Our study plans are based on research and help students prioritize which
            subjects
            to study and when.<br><br>Take a look through a few pages of a <span>sample plan</span> and see why other
            students already using Study Sauce are improving their GPA.</a>
        <a class="take-the-tour" onclick="jQuery('.lightbox').first().trigger('click'); return false;" href="#">
            <span>Take the tour</span></a>
    </p>

    <div class="highlighted-link buy-links">
        <p class="price-is-right"><a href="#" onclick="jQuery('.lightbox').first().trigger('click'); return false;">
                <span style="font-size: 24px;">$10</span> / term</a></p>
        <p><a class="more-parents students_only" href="/student/1">Bill my parents</a> &nbsp;
            <a class="more students_only" href="/buy">Buy study plan</a>
            <a class="more parents_only" href="/parent/3">Buy study plan</a></p>
    </div>
    <p style="margin: 0px; clear: both;">&nbsp;</p>
    <hr>
    <h2 class="students_only">Your study plan features</h2>
    <h2 class="parents_only">Study plan features</h2>

    <div class="grid_6">
        <div><img width="48" height="48" src="/[custom:files-path]/Grey%2520Icons%2520Calender.png">

            <h3>Detailed study schedule</h3>

            <p class="students_only">We plan out your entire semester and tell you what to study and when. Be confident
                knowing that you are making the most of your study time.</p>

            <p class="parents_only">We plan out your student's entire semester and tell him/her what to study and when.
                Be
                confident knowing that your student is making the most of study time.</p>
        </div>
        <div><img width="48" height="48" src="/[custom:files-path]/Grey%2520Icons%2520Science.png">

            <h3>Proven science</h3>

            <p class="students_only">Your study plan incorporates the leading science in memory retention. Improve your
                study skills and stop cramming for exams only to forget all of the information a few days later.</p>

            <p class="parents_only">Your student's study plan incorporates the leading science in memory retention.
                Improve
                your student's study skills to stop cramming for exams only to forget all of the information a few days
                later.</p>
        </div>
    </div>
    <div class="grid_6">
        <div><img width="48" height="48" src="/[custom:files-path]/Grey%2520Icons%2520Tips.png">

            <h3>Study tips</h3>

            <p class="students_only">We also give you invaluable tips on how to study…and more importantly how not to
                study.
                We have compiled the leading research on good and bad study habits and you will be surprised by the
                results.</p>

            <p class="parents_only">We also give your student invaluable tips on how to study…and more importantly how
                not
                to study. We have compiled the leading research on good and bad study habits and you will be surprised
                by
                the results.</p>
        </div>
        <div><img width="48" height="48" src="/[custom:files-path]/Grey%2520Icons%2520Money%2520back%2520q.png">

            <h3>Money back guarantee</h3>

            <p class="students_only">If your GPA doesn’t go up, we will refund your money. No hassle!</p>

            <p class="parents_only">If your student's GPA doesn’t go up, we will refund your money. No hassle!</p>
        </div>
    </div>
    <p style="clear: both; margin-bottom:0; line-height:1px;">&nbsp;</p>
<?php endif; ?>

