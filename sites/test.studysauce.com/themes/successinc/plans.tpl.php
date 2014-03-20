<?php
// check if user has purchased a plan
global $user;
$orders = _studysauce_orders_by_uid($user->uid);
$conn = studysauce_get_connections();
foreach ($conn as $i => $c)
    $orders = array_merge($orders, _studysauce_orders_by_uid($c->uid));

if (count($orders)):

    // get event schedule
    $o = end($orders);
    list($events, $node, $classes) = studysauce_get_events($o->created);

    if(!isset($node->field_university['und'][0]['value']) || empty($node->field_university['und'][0]['value']))
    {
        // display study preferences dialog
        ?>
        <div id="study-preferences">
            <div>
                <h3>First, answer a few questions about how you study.</h3>
                <div class="field-type-text field-name-field-university field-widget-text-textfield form-wrapper">
                    <label for="schedule-university">School name</label>
                    <input class="text-full form-text required" type="text" id="schedule-university" name="schedule-university" size="60" maxlength="255">
                </div>
                <div class="field-type-list-text field-name-field-grades field-widget-options-buttons form-wrapper">
                    <label>What kind of grades do you want?</label>
                    <input type="radio" id="schedule-grades-as-only" name="field_grades" value="as_only" class="form-radio">
                    <label class="option" for="schedule-grades-as-only">Nothing but As </label>
                    <input type="radio" id="schedule-grades-has-life" name="field_grades" value="has_life" class="form-radio">
                    <label class="option" for="schedule-grades-has-life">I want to do well, but don't want to live in the library </label>
                </div>
                <div class="field-type-list-text field-name-field-weekends field-widget-options-buttons form-wrapper">
                    <label>How do your manage your weekends?</label>
                    <input type="radio" id="schedule-weekends-hit-hard" name="field_weekends" value="hit_hard" class="form-radio">
                    <label class="option" for="schedule-weekends-hit-hard">Hit hard, keep weeks open </label>
                    <input type="radio" id="schedule-weekends-light-work" name="field_weekends" value="light_work" class="form-radio">
                    <label class="option" for="schedule-weekends-light-work">Light work, focus during the week </label>
                </div>
                <div class="field-name-field-time-preference">
                    <label>On a scale of 0-5 (5 being the best), rate how mentally sharp you feel during the following time periods:</label>
                    <div class="field-type-list-integer field-name-field-6-am-11-am field-widget-options-buttons form-wrapper">
                        <label>6 AM - 11 AM</label>
                        <div class="form-radios">
                            <input type="radio" id="schedule-6-am-11-am-0" name="field_6_am_11_am" value="0" class="form-radio">
                            <label class="option" for="schedule-6-am-11-am-0">0 </label>
                            <input type="radio" id="schedule-6-am-11-am-1" name="field_6_am_11_am" value="1" class="form-radio">
                            <label class="option" for="schedule-6-am-11-am-1">1 </label>
                            <input type="radio" id="schedule-6-am-11-am-2" name="field_6_am_11_am" value="2" class="form-radio">
                            <label class="option" for="schedule-6-am-11-am-2">2 </label>
                            <input type="radio" id="schedule-6-am-11-am-3" name="field_6_am_11_am" value="3" class="form-radio">
                            <label class="option" for="schedule-6-am-11-am-3">3 </label>
                            <input type="radio" id="schedule-6-am-11-am-4" name="field_6_am_11_am" value="4" class="form-radio">
                            <label class="option" for="schedule-6-am-11-am-4">4 </label>
                            <input type="radio" id="schedule-6-am-11-am-5" name="field_6_am_11_am" value="5" class="form-radio">
                            <label class="option" for="schedule-6-am-11-am-5">5 </label>
                        </div>
                    </div>
                    <div class="field-type-list-integer field-name-field-11-am-4-pm field-widget-options-buttons form-wrapper">
                        <label for="schedule-11-am-4-pm">11 AM - 4 PM</label>
                        <div class="form-radios">
                            <input type="radio" id="schedule-11-am-4-pm-0" name="field_11_am_4_pm" value="0" class="form-radio">
                            <label class="option" for="schedule-11-am-4-pm-0">0 </label>
                            <input type="radio" id="schedule-11-am-4-pm-1" name="field_11_am_4_pm" value="1" class="form-radio">
                            <label class="option" for="schedule-11-am-4-pm-1">1 </label>
                            <input type="radio" id="schedule-11-am-4-pm-2" name="field_11_am_4_pm" value="2" class="form-radio">
                            <label class="option" for="schedule-11-am-4-pm-2">2 </label>
                            <input type="radio" id="schedule-11-am-4-pm-3" name="field_11_am_4_pm" value="3" class="form-radio">
                            <label class="option" for="schedule-11-am-4-pm-3">3 </label>
                            <input type="radio" id="schedule-11-am-4-pm-4" name="field_11_am_4_pm" value="4" class="form-radio">
                            <label class="option" for="schedule-11-am-4-pm-4">4 </label>
                            <input type="radio" id="schedule-11-am-4-pm-5" name="field_11_am_4_pm" value="5" class="form-radio">
                            <label class="option" for="schedule-11-am-4-pm-5">5 </label>
                        </div>
                    </div>
                    <div class="field-type-list-integer field-name-field-4-pm-9-pm field-widget-options-buttons form-wrapper">
                        <label for="schedule-4-pm-9-pm">4 PM - 9 PM</label>
                        <div class="form-radios">
                            <input type="radio" id="schedule-4-pm-9-pm-0" name="field_4_pm_9_pm" value="0" class="form-radio">
                            <label class="option" for="schedule-4-pm-9-pm-0">0 </label>
                            <input type="radio" id="schedule-4-pm-9-pm-1" name="field_4_pm_9_pm" value="1" class="form-radio">
                            <label class="option" for="schedule-4-pm-9-pm-1">1 </label>
                            <input type="radio" id="schedule-4-pm-9-pm-2" name="field_4_pm_9_pm" value="2" class="form-radio">
                            <label class="option" for="schedule-4-pm-9-pm-2">2 </label>
                            <input type="radio" id="schedule-4-pm-9-pm-3" name="field_4_pm_9_pm" value="3" class="form-radio">
                            <label class="option" for="schedule-4-pm-9-pm-3">3 </label>
                            <input type="radio" id="schedule-4-pm-9-pm-4" name="field_4_pm_9_pm" value="4" class="form-radio">
                            <label class="option" for="schedule-4-pm-9-pm-4">4 </label>
                            <input type="radio" id="schedule-4-pm-9-pm-5" name="field_4_pm_9_pm" value="5" class="form-radio">
                            <label class="option" for="schedule-4-pm-9-pm-5">5 </label>
                        </div>
                    </div>
                    <div class="field-type-list-integer field-name-field-9-pm-2-am field-widget-options-buttons form-wrapper">
                        <label for="schedule-9-pm-2-am">9 PM - 2 AM</label>
                        <div class="form-radios">
                            <input type="radio" id="schedule-9-pm-2-am-0" name="field_9_pm_2_am" value="0" class="form-radio">
                            <label class="option" for="schedule-9-pm-2-am-0">0 </label>
                            <input type="radio" id="schedule-9-pm-2-am-1" name="field_9_pm_2_am" value="1" class="form-radio">
                            <label class="option" for="schedule-9-pm-2-am-1">1 </label>
                            <input type="radio" id="schedule-9-pm-2-am-2" name="field_9_pm_2_am" value="2" class="form-radio">
                            <label class="option" for="schedule-9-pm-2-am-2">2 </label>
                            <input type="radio" id="schedule-9-pm-2-am-3" name="field_9_pm_2_am" value="3" class="form-radio">
                            <label class="option" for="schedule-9-pm-2-am-3">3 </label>
                            <input type="radio" id="schedule-9-pm-2-am-4" name="field_9_pm_2_am" value="4" class="form-radio">
                            <label class="option" for="schedule-9-pm-2-am-4">4 </label>
                            <input type="radio" id="schedule-9-pm-2-am-5" name="field_9_pm_2_am" value="5" class="form-radio">
                            <label class="option" for="schedule-9-pm-2-am-5">5 </label>
                        </div>
                    </div>
                </div>
                <p style="clear:both; margin-bottom:0;" class="highlighted-link"><a href="#save-schedule" class="more">Save</a></p>
            </div>
        </div>

<?php } ?>

    <button class="field-add-more-submit ajax-processed" name="field_reminders_add_more" value="Add new" onclick="jQuery('#plan').toggleClass('edit-only'); jQuery('#schedule-class-name-label').show(); jQuery('#schedule-event-title-label, .field-name-field-day-of-the-week .form-checkboxes.form-type-checkbox *').hide(); jQuery('#plan .field-name-field-day-of-the-week .form-checkboxes .form-type-checkbox').show(); jQuery('#schedule-all-day, label[for=&quot;schedule-all-day&quot;]').css('visibility', 'hidden'); jQuery('#plan #schedule-type').val('c'); jQuery('#plan #schedule-weekly').prop('checked', true); return false;">
        Add <span>&nbsp;</span> class
    </button>
    <button class="field-add-more-submit ajax-processed" name="field_reminders_add_more" value="Add new" onclick="jQuery('#plan').toggleClass('edit-only'); jQuery('#schedule-class-name-label').hide(); jQuery('#schedule-event-title-label, .field-name-field-day-of-the-week .form-checkboxes.form-type-checkbox *').show(); jQuery('#schedule-all-day, label[for=&quot;schedule-all-day&quot;]').css('visibility', 'visible'); jQuery('#plan #schedule-type').val('o'); return false;">
        Add <span>&nbsp;</span> other event
    </button>
    <div id="class-dialog">
        <div class="edit row">
            <div class="field-type-text field-name-field-class-name field-widget-text-textfield form-wrapper">
                <div class="form-item form-type-textfield">
                    <label for="schedule-class-name" id="schedule-event-title-label">Event title</label>
                    <label for="schedule-class-name" id="schedule-class-name-label">Class name</label>
                    <input name="schedule-class-name"
                           class="text-full form-text jquery_placeholder-processed"
                           id="schedule-class-name"
                           onkeyup='var startDate = jQuery(this).parents(".field-type-text").parent().find(".start-date-wrapper .form-type-textfield:first-child input"); if(startDate.val() == "" &amp;&amp; startDate.attr("placeholder") != "Start") startDate.val(startDate.attr("placeholder"));var endDate = jQuery(this).parents(".field-type-text").parent().find(".end-date-wrapper .form-type-textfield:first-child input"); if(endDate.val() == "" &amp;&amp; endDate.attr("placeholder") != "End") endDate.val(endDate.attr("placeholder"));'
                           type="text" size="60" maxlength="255" placeholder="Hist 101" value="" autocomplete="off">
                    <div class="form-checkboxes form-type-checkbox">
                        <?php
                        foreach($classes as $i => $c)
                        {
                            $skip = false;
                            reset($events);
                            while (list($key, $val) = each($events))
                                if($val['title'] == $c)
                                {
                                    $skip = true;
                                    break;
                                }
                            if($skip)
                                continue;
                            ?><input type="radio" name="schedule-class-option" id="schedule-class<?php print $i; ?>" value="<?php print $c; ?>"
                                onchange="jQuery('#plan .field-name-field-class-name input.form-text').val(jQuery(this).val());"/>
                              <label for="schedule-class<?php print $i; ?>" class="class<?php print $i; ?>"><?php print $c; ?></label><?php
                        }
                        ?>
                    </div>
                </div>
            </div>
            <div class="field-type-list-text field-name-field-day-of-the-week field-widget-options-buttons form-wrapper">
                <div class="form-item form-type-checkboxes">
                    <label>Reoccurring</label>
                    <div class="form-checkboxes form-type-checkbox">
                        <input type="radio" name="schedule-reoccurring" id="schedule-daily" value="daily" class="form-checkbox" onchange="if(jQuery(this).is(':checked')) { jQuery('#plan input[name*=&quot;schedule-dotw-&quot;]').prop('checked', true); jQuery('#plan .field-name-field-day-of-the-week .form-checkboxes .form-type-checkbox').hide(); }" />
                        <label for="schedule-daily">Daily</label>
                        <input type="radio" name="schedule-reoccurring" id="schedule-weekly" value="weekly" class="form-checkbox" checked onchange="if(jQuery(this).is(':checked')) { jQuery('#plan input[name*=&quot;schedule-dotw-&quot;]').prop('checked', false); jQuery('#plan .field-name-field-day-of-the-week .form-checkboxes .form-type-checkbox').show(); }" />
                        <label for="schedule-weekly">Weekly</label>
                        <input type="radio" name="schedule-reoccurring" id="schedule-monthly" value="monthly" class="form-checkbox" onchange="if(jQuery(this).is(':checked')) { jQuery('#plan input[name*=&quot;schedule-dotw-&quot;]').prop('checked', false); jQuery('#plan .field-name-field-day-of-the-week .form-checkboxes .form-type-checkbox').hide(); }" />
                        <label for="schedule-monthly">Monthly</label>
                        <input type="radio" name="schedule-reoccurring" id="schedule-yearly" value="yearly" class="form-checkbox" onchange="if(jQuery(this).is(':checked')) { jQuery('#plan input[name*=&quot;schedule-dotw-&quot;]').prop('checked', false); jQuery('#plan .field-name-field-day-of-the-week .form-checkboxes .form-type-checkbox').hide(); }" />
                        <label for="schedule-yearly">Yearly</label>
                    </div>
                    <div class="form-checkboxes">
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-M" class="form-checkbox"
                                   id="schedule-dotw-M" type="checkbox" value="M"> <label
                                class="option" for="schedule-dotw-M">M </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-Tu" class="form-checkbox"
                                   id="schedule-dotw-Tu" type="checkbox" value="Tu"> <label
                                class="option" for="schedule-dotw-Tu">Tu </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-W" class="form-checkbox"
                                   id="schedule-dotw-W" type="checkbox" value="W"> <label
                                class="option" for="schedule-dotw-W">W </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-Th" class="form-checkbox"
                                   id="schedule-dotw-Th" type="checkbox" value="Th"> <label
                                class="option" for="schedule-dotw-Th">Th </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-F" class="form-checkbox"
                                   id="schedule-dotw-F" type="checkbox" value="F"> <label
                                class="option" for="schedule-dotw-F">F </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-Sa" class="form-checkbox"
                                   id="schedule-dotw-Sa" type="checkbox" value="Sa"> <label
                                class="option" for="schedule-dotw-Sa">Sa </label>

                        </div>
                        <div class="form-item form-type-checkbox">
                            <input name="schedule-dotw-Su" class="form-checkbox"
                                   id="schedule-dotw-Su" type="checkbox" value="Su"> <label
                                class="option" for="schedule-dotw-Su">Su </label>

                        </div>
                    </div>
                </div>
            </div>
            <div class="field-type-datetime field-name-field-time field-widget-date-popup form-wrapper">
                <div class="form-item form-type-textfield">
                    <label for="schedule-value-time">Time</label>
                    <input name="schedule-value-time"
                           class="date-clear form-text jquery_placeholder-processed"
                           id="schedule-value-time"
                           onchange='var myDate = jQuery(this).parents(".form-type-textfield").prev().find("input"); if(myDate.val() == "" &amp;&amp; myDate.attr("placeholder") != "Start") myDate.val(myDate.attr("placeholder"));'
                           type="text" size="15" maxlength="10" placeholder="Start" value="">
                </div>
                <div class="form-item form-type-textfield">
                    <input type="checkbox" name="schedule-all-day" id="schedule-all-day" /><label for="schedule-all-day">All day event</label>
                    <input name="schedule-value2-time"
                           class="date-clear form-text jquery_placeholder-processed"
                           id="schedule-value2-time"
                           onchange='var myDate = jQuery(this).parents(".form-type-textfield").prev().find("input"); if(myDate.val() == "" &amp;&amp; myDate.attr("placeholder") != "End") myDate.val(myDate.attr("placeholder"));'
                           type="text" size="15" maxlength="10" placeholder="End" value="">
                </div>
                <div class="form-item form-type-textfield">
                    <label
                        for="schedule-value-date">Date</label>
                    <input name="schedule-value-date"
                           class="date-clear form-text jquery_placeholder-processed"
                           id="schedule-value-date"
                           onchange='var that = jQuery(this).val(); jQuery(".field-name-field-time .start-date-wrapper .form-type-textfield:first-child input").each(function () { jQuery(this).attr("placeholder", that); });'
                           type="text" size="20" maxlength="30" placeholder="Start" value="">
                </div>
                <div class="form-item form-type-textfield">
                    &nbsp;
                    <input name="schedule-value2-date"
                           class="date-clear form-text jquery_placeholder-processed"
                           id="schedule-value2-date"
                           onchange='var that = jQuery(this).val(); jQuery(".field-name-field-time .end-date-wrapper .form-type-textfield:first-child input").each(function () { jQuery(this).attr("placeholder", that); });'
                           type="text" size="20" maxlength="30" placeholder="End" value="">
                </div>
            </div>
            <input type="hidden" class="field-type-hidden field-name-field-event-type form-wrapper" name="schedule-type" id="schedule-type" />
            <input class="form-submit ajax-processed" type="submit" value="Remove">
            <p style="margin-bottom:0;clear:both;position:static;top:auto;" class="highlighted-link">
                <a href="#save-class" class="more">Save</a>
            </p>
            <a href="#" onclick="jQuery('#schedule form')[0].reset(); jQuery('#plan').removeClass('edit-only').scrollintoview(); return false;" class="fancy-close">&nbsp;</a>
        </div>
    </div>
    <div class="mobile-only">
        <?php
        $startWeek = strtotime(date("Y-m-d", strtotime('this week', time()))) - 86400;
        $endWeek = $startWeek + 604800 - 86400;
        $dotwStr = '';
        if (count($events)) {
            // TODO: list classes on plan
            foreach ($events as $i => $event) {
                if (strtotime($event['start']) >= $startWeek && strtotime($event['start']) <= $endWeek) {
                    if (date('l', strtotime($event['start'])) != $dotwStr)
                        print '<h3>' . date('l', strtotime($event['start'])) . '</h3>';
                    $dotwStr = date('l', strtotime($event['start']));
                    print $event['rendered'];
                }
            }

            // TODO: you haven't entered any classes message
        }
        ?>
    </div>
    <div id="calendar" class="full-only"></div>
    <script type="text/javascript"> window.planEvents = <?php print json_encode($events); ?>; </script>
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
        <a class="lightbox" href="/[custom:theme-path]/tour/1-Title.png" rel="tour"></a>
        <a class="lightbox" href="/[custom:theme-path]/tour/2-Tips.png" rel="tour"></a>
        <a class="lightbox" href="/[custom:theme-path]/tour/3-Science2.png" rel="tour"></a>
        <a class="lightbox" href="/[custom:theme-path]/tour/4-Science.png" rel="tour"></a>
        <a class="lightbox" href="/[custom:theme-path]/tour/5-Schedule.png" rel="tour"></a>
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

    <div class="highlighted-link">
        <p class="price-is-right"><a href="user/register"><span style="font-size: 24px;">$10</span> / term</a></p>

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
<?php endif ?>