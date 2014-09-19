<?php

?>
<div class="header-wrapper">
    <div class="banner container_12">
        <div class="flexslider">
            <h1>Help Your Student Succeed</h1>
            <ul class="slides">
                <li class="clone">
                    <div class="video-floater">
                        <div style="width:100%;padding-bottom:56%;position:relative;">
                            <!--<iframe id="ytplayer" width="560" height="315" src="https://www.youtube.com/embed/vJG9PDaXNaQ?rel=0&controls=0&modestbranding=1&showinfo=0&enablejsapi=1&playerapiid=ytplayer&origin=[site:url]" frameborder="0" style="width:100%;height:100%;position:absolute;top:0;left:0;" allowfullscreen></iframe>-->
                            <div id="ytplayer"></div>
                            <script type="text/javascript">
                                var tag = document.createElement('script');

                                tag.src = "https://www.youtube.com/iframe_api";
                                var firstScriptTag = document.getElementsByTagName('script')[0];
                                firstScriptTag.parentNode.insertBefore(tag, firstScriptTag);

                                function onYouTubeIframeAPIReady(playerId) {
                                    player = new YT.Player('ytplayer', {
                                        height: '315',
                                        width: '560',
                                        videoId: 'vJG9PDaXNaQ',
                                        playerVars: {
                                            rel: 0,
                                            controls: 0,
                                            modestbranding: 1,
                                            showinfo: 0,
                                            enablejsapi: 1,
                                            playerapiid: 'ytplayer',
//            origin:'[site:url]'
                                        },
                                        events: {
                                            'onStateChange': function (e) {
                                                _gaq.push(['_trackPageview', location.pathname + location.search + '#yt' + e.data]);
                                            }
                                        }
                                    });
                                }
                            </script>
                        </div>
                    </div>
                    <div class="highlighted-link">
                        <p><a class="more" href="/cart/add/e-p13_q1_a4o13_s?destination=cart/checkout">Sponsor student</a><br/>
                            <span style="color:#ffffff;">or </span><a href="#bill-my-parents" style="font-weight:bold;padding:8px 25px 8px 10px">Tell your student</a></p>
                    </div>
                </li>
            </ul>
        </div>
    </div>
</div>
<div class="did-you-know">
    <div class="container_12">
        <div class="buy-plan">
            <div class="fixed-centered modal">
                <div id="bill-my-parents" class="dialog">
                    <h2>Invite your student to start using Study Sauce.</h2>
                    <div
                        class="form-item webform-component webform-component-textfield webform-component--student-first-name">
                        <label>Student's first name</label>
                        <input type="text" name="invite-first" size="60" maxlength="128" class="form-text required"
                               value="">
                    </div>
                    <div
                        class="form-item webform-component webform-component-textfield webform-component--student-last-name">
                        <label>Student's last name</label>
                        <input type="text" name="invite-last" size="60" maxlength="128" class="form-text required"
                               value="">
                    </div>
                    <div class="form-item webform-component webform-component-email">
                        <label>Student's email</label>
                        <input class="email form-text form-email required" type="email" name="invite-email" size="60"
                               value="">
                    </div>
                    <div class="highlighted-link">
                        <a href="#bill-send" class="more">Send</a></div>
                    <a href="#close">&nbsp;</a>
                </div>
                <div id="parents-bill-2" class="dialog">
                    <h2>Thanks!</h2>
                    <h3>Your student has been notified.</h3>
                    <div class="highlighted-link">
                        <a href="#close" class="more">Close</a></div>
                    <a href="#close">&nbsp;</a>
                </div>
            </div>
        </div>
        <img width="165" src="/sites/test.studysauce.com/themes/successinc/did_you_know_620x310.png"/>

        <h1>Even if your student is away at school, you can still help.</h1>

        <div class="one"><h3><span>1</span>Upgrade your student's free account</h3></div>
        <div class="two"><h3><span>2</span>Spread the word</h3></div>
        <div class="one highlighted-link"><a class="more" href="/cart/add/e-p13_q1_a4o13_s?destination=cart/checkout">Sponsor
                student</a></div>
        <div class="two"><a class="more" href="#bill-my-parents">Tell your student</a></div>
    </div>
</div>

<div class="page-top">
    <div class="container_12" id="src">
        <h1>Give your student the advantage</h1>

        <div class="grid_6"><img width="200" height="200" alt=""
                                 src="/<?php print drupal_get_path('theme', 'successinc'); ?>/images/situation_compressed.png"></div>
        <div class="grid_6">
            <h3>Students learn to become great studiers</h3>

            <p>An incredible amount of time and effort is devoted to learning in the classroom. However, up to 75% of
                time in school is spent outside class. Considering how much time is spent studying, it is stunning that
                students are never taught effective study methods to employ once they leave the classroom.</p>
        </div>
        <div class="grid_6"><img width="200" height="200" alt=""
                                 src="/<?php print drupal_get_path('theme', 'successinc'); ?>/images/complication_compressed.png"></div>
        <div class="grid_6">
            <h3>Bad study habits are hurting performance</h3>

            <p>To make things worse, many of the methods students typically use are either ineffective or oftentimes
                counterproductive. For example, highlighting or underlining while studying offers no benefit and can
                even impede learning.</p>
        </div>
        <div class="grid_6"><img width="200" height="200" alt=""
                                 src="/<?php print drupal_get_path('theme', 'successinc'); ?>/images/resolution_compressed.png"></div>
        <div class="grid_6">
            <h3>We make becoming a great studier easy</h3>

            <p>We have studied the best scientific research and have incorporated the findings into our site. Study
                Sauce automatically detects good and bad study behaviors and teaches student by simply logging in during
                study sessions. Your student can become a great studier and improve retention, performance, and
                grades.</p>
        </div>
        <p class="highlighted-link"><a class="more" href="/cart/add/e-p13_q1_a4o13_s?destination=cart/checkout">Sponsor
                student</a></p>
    </div>
</div>

<div class="page">
    <div class="container_12">
        <h1 style="color:#FFFFFF;font-weight:bold;text-align:center;font-size:32px;margin:20px;">Help your student
            become a great studier</h1>
    </div>
</div>

<div class="footer">
    <div class="container_12 two-column-guide">
        <h1>Study Sauce features</h1>

        <div class="grid_6">
            <div><img width="48" height="48" src="/<?php print variable_get('file_public_path', conf_path() . '/files'); ?>/Grey_Icons_Science.png">

                <h3>Proven science</h3>

                <p>We incorporate the leading science in memory retention to ensure study time is maximized. Improve
                    study skills and stop cramming for exams only to forget all of the information a few days later.</p>
            </div>
            <div><span class="checkin">&nbsp;</span>

                <h3>Check in</h3>

                <p>Students log into study sessions and retrain themselves to use the most effective study methods. We
                    teach them as they go.</p>
            </div>
            <div><span class="incentives">&nbsp;</span>

                <h3>Create incentives</h3>

                <p>We incorporate the Incentive Theory of Motivation to help establish meaningful rewards. In fact, many
                    of our sponsors establish extra incentives for student achievements.</p>
            </div>
        </div>
        <div class="grid_6">
            <div><img width="48" height="48" src="/<?php print variable_get('file_public_path', conf_path() . '/files'); ?>/Grey_Icons_Calender.png">

                <h3>Study plans</h3>

                <p>Take studying to the next level with one of our custom study plans. We build personalized plans based
                    on the student's study preferences and individual needs.</p>
            </div>
            <div><img width="48" height="48" src="/<?php print variable_get('file_public_path', conf_path() . '/files'); ?>/Grey_Icons_Tips.png">

                <h3>Study tips</h3>

                <p>We provide invaluable tips on how to study, and more importantly how not to study. It is a safe bet
                    that you will be surprised by the insights.</p>
            </div>
            <div><span class="metrics">&nbsp;</span>

                <h3>Study metrics</h3>

                <p>Track study sessions over time. See all of the hard work aggregated in custom charts that we create
                    when students check in.</p>
            </div>
        </div>
    </div>
    <div class="container_12">
        <p id="support-box" style="padding: 40px 0px 0px; text-align: center; clear: both;">
            <a href="#parents-contact"><img width="48" height="48"
                              style="margin-top: -10px; margin-right: 10px; float: left; display: inline-block;"
                              src="/<?php print drupal_get_path('theme', 'successinc'); ?>/chat_icon.png">Still have questions? <span>Talk to a study tutor.</span></a>
        </p>
        <p class="highlighted-link"><a class="more" href="/cart/add/e-p13_q1_a4o13_s?destination=cart/checkout">Sponsor
                student</a></p>
    </div>
</div>
<div class="fixed-centered modal">
    <div id="parents-contact" class="dialog">
        <?php print theme('studysauce-contact'); ?>
        <a href="#close">&nbsp;</a>
    </div>
</div>

<div class="subfooter">
    <div class="pane-testimonials">
        <div class="inner pane-inner">
            <h1 class="pane-title">What our students are saying</h1>

            <div
                class="view view-testimonials view-id-testimonials view-display-id-block_1 view-dom-id-758a656a03f200241aa47343d723b353">
                <div class="view-content">
                    <div class="views-row views-row-1 views-row-odd views-row-first views-row-last">
                        <div class="views-field views-field-field-teaser-image">
                            <div class="field-content"><img typeof="foaf:Image"
                                                            src="https://test.studysauce.com/sites/test.studysauce.com/files/testimonial.png"
                                                            width="342" height="342" alt=""></div>
                        </div>
                        <div class="views-field views-field-body">
                            <div class="field-content">
                                <div class="testimonial clearfix">
                                    <div class="testimonial-inner">
                                        <p>“I never knew how to actually study until Study Sauce showed me. &nbsp;Now I'm
                                            organized and I don't cram for tests."</p>

                                    </div>

                                    <div class="testimonial-submitted clearfix">
                                        <p>- Justin C.</p>

                                        <p>Arizona State University</p>
                                    </div>

                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>

    <?php print theme('studysauce-footer'); ?>
</div>
