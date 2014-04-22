<h2>Choosing an accountability partner can be invaluable to achieving your goals</h2>
<br />
<div class="partner-setup">
    <h3>I am accountable to:</h3>

    <div class="plupload" id="partner-plupload">
        <div class="plup-list-wrapper">
            <ul class="plup-list clearfix ui-sortable">
                <img src="/<?php print drupal_get_path('theme', 'successinc'); ?>/images/empty-photo.png" height="200" width="200" alt="Upload" />
            </ul>
        </div>
        <div class="plup-filelist" id="partner-plupload-filelist">
            <table>
                <tbody>
                <tr class="plup-drag-info">
                    <td>
                        <div class="drag-main">Upload photo of your partner</div>
                        <div class="drag-more">
                            <div>You can upload up to <strong>1</strong> files.</div>
                            <div>Allowed files types: <strong>png gif jpg jpeg</strong>.</div>
                        </div>
                    </td>
                </tr>
                </tbody>
            </table>
        </div>
        <div class="plup-bar clearfix">
            <a href="#partner-select" class="plup-select" id="partner-plupload-select">Add</a>
            <a hre="#partner-upload" class="plup-upload" id="partner-plupload-upload">Upload</a>
            <div class="plup-progress"></div>
        </div>
        <div class="plupload html5"
             style="position: absolute; background-color: transparent; width: 0px; height: 0px; overflow: hidden; z-index: -1; opacity: 0; top: 0px; left: 0px;">
            <input style="font-size: 999px; position: absolute; width: 100%; height: 100%;" type="file"
                   accept="image/png,image/gif,image/jpeg,image/*" multiple="multiple"></div>
    </div>

    <div class="partner-invite">
        <div
            class="form-item webform-component webform-component-textfield">
            <input type="text" id="partner-first" name="partner-first"
                   value="" size="60" maxlength="128" class="form-text required" placeholder="First name">
        </div>
        <div
            class="form-item webform-component webform-component-textfield">
            <input type="text" id="partner-last" name="partner-last" value=""
                   size="60" maxlength="128" class="form-text required" placeholder="Last name">
        </div>
        <div class="form-item webform-component webform-component-email">
            <input class="email form-text form-email required" type="email" id="partner-email"
                   name="partner-email" size="60" placeholder="Email address">
        </div>
        <div class="highlighted-link form-actions">
            <a href="#partner-save" class="webform-submit button-primary more form-submit ajax-processed">Save</a></div>
    </div>

    <h3>My partner is allowed to see:</h3>
    <ul class="partner-permissions">
        <li><input type="checkbox" value="goals" id="partner-goals" name="partner-goals" />
            <label for="partner-goals">My goals</label></li>
        <li><input type="checkbox" value="metrics" id="partner-metrics" name="partner-metrics" />
            <label for="partner-metrics">My study metrics</label></li>
        <li><input type="checkbox" value="deadlines" id="partner-deadlines" name="partner-deadlines" />
            <label for="partner-deadlines">My deadlines</label></li>
        <li><input type="checkbox" value="uploads" id="partner-uploads" name="partner-uploads" />
            <label for="partner-uploads">My uploaded content <sup class="premium">Premium</sup></label></li>
        <li><input type="checkbox" value="plan" id="partner-plan" name="partner-plan" />
            <label for="partner-plan">My study plan <sup class="premium">Premium</sup></label></li>
        <li><input type="checkbox" value="profile" id="partner-profile" name="partner-profile" />
            <label for="partner-profile">My study profiles <sup class="premium">Premium</sup></label></li>
    </ul>
</div>
<div class="partner-faqs">
    <h3>FAQs:</h3>
    <h4>Why do I need an accountability partner?</h4>
    <p>
        Research shows that simply writing down your goals makes you more likely to achieve them.  Having an accountability partner takes it to a new level.  We all have ups and downs in school and finding someone to help motivate and challenge you along the way can be invaluable.
    </p>
    <h4>How do I choose an accountability partner?</h4>
    <p>
        An accountability partner is someone that will keep you on track to achieve your goals.  Here are some attributes to consider as you decide.  Choose someone that:
    </p>
    <ul>
        <li>Will challenge you (you will need more than just encouragement)</li>
        <li>Will celebrate your successes with you</li>
        <li>Is invested in your education</li>
        <li>You trust</li>
    </ul>
    <p>Take a few minutes to think about who best fits this description.  Sometimes a parent or best friend are not your best options.  Maybe some other family member, classmate, or even a non-family mentor can be the ideal choice.</p>
    <h4>Now that I have chosen my accountability partner, what should I do?</h4>
    <p>Communication is the key!  Outline your expectations and ask to be held accountable.  Set up regular check-ins (try to talk at least once every week).  Be transparent about your struggles and your successes during the conversations.</p>
    <h4>Can I change my accountability partner in Study Sauce?</h4>
    <p>Sure you can.  You can change your accountability partner or what they can see at any time.  Just use the edit function next to the photograph on the Accountability partner tab.</p>
</div>