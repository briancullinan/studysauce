
<div class="field-select-strategy">
    <div>
        <label>Recommended strategy:</label>
        <select name="strategy-select">
            <option value="_none" selected="selected">- Change strategy -</option>
            <option value="teach">Teach</option>
            <option value="spaced">Spaced repetition</option>
            <option value="active">Active reading</option>
            <option value="prework">Prework</option>
        </select>
    </div>
</div>
<div class="strategy-teach">
    <h3>Teach - Upload a 1 min video explaining your assignment</h3>
    <div class="plupload" id="plan-{eid}-plupload">
        <div class="plup-list-wrapper">
            <ul class="plup-list clearfix ui-sortable">
                <li<img src="/<?php print drupal_get_path('theme', 'successinc'); ?>/images/empty-play.png" alt="Upload" /></li>
            </ul>
        </div>
        <div class="plup-filelist" id="plan-{eid}-filelist">
            <table>
                <tbody>
                <tr class="plup-drag-info">
                    <td>
                        <div class="drag-main">Upload video</div>
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
            <a href="#plan-{eid}-select" class="plup-select" id="plan-{eid}-select">Add</a>
            <a hre="#plan-{eid}-upload" class="plup-upload" id="plan-{eid}-upload">Upload</a>
            <div class="plup-progress"></div>
        </div>
    </div>
    <div class="strategy-notes">
        <label>Title:</label>
        <input type="text" class="form-text" name="strategy-title" readonly="readonly" />
        <label>Notes:</label>
        <textarea type="text" name="strategy-notes" readonly="readonly"></textarea>
    </div>
</div>
<div class="strategy-active">
    <h3>Active reading - Follow the guide below to better retain what you are reading.</h3>
    <h4>Before reading:</h4>
    <label>Take no more than 2 minutes to skim the reading. What is the topic?</label>
    <textarea name="strategy-skim" readonly="readonly"></textarea>
    <label>Why am I being asked to read this at this point in the class?</label>
    <textarea name="strategy-why" readonly="readonly"></textarea>
    <h4>During reading:</h4>
    <label>What questions do I have as I am reading?</label>
    <textarea name="strategy-questions" readonly="readonly"></textarea>
    <h4>After reading:</h4>
    <label>Please summarize the reading in a few paragraphs (less than 1 page).  What are the 1 or 2 most important ideas from the reading?</label>
    <textarea name="strategy-summarize" readonly="readonly"></textarea>
    <label>What possible exam questions will result from this reading?</label>
    <textarea name="strategy-exam" readonly="readonly"></textarea>
</div>
<div class="strategy-spaced">
    <h3>Spaced repetition - Commit information to your long term memory by revisiting past work.</h3>
    <h4>Instructions - We highly recommend flashcards.  Online flashcard maker Quizlet is our favorite.  Read more about spaced repetition here.</h4>
    <div class="strategy-review">
        <label>Review material from:</label>
    </div>
    <div class="strategy-notes">
        <label>Write down any notes below:</label>
        <textarea type="text" name="strategy-notes" readonly="readonly"></textarea>
    </div>
</div>
<div class="strategy-other">
    <h3>Notes:</h3>
    <textarea name="strategy-notes" placeholder="Write any notes here." readonly="readonly"></textarea>
</div>
<div class="strategy-prework">
    <h3>Prework - Get prepared for your class tomorrow.</h3>
    <input type="checkbox" name="strategy-topics" id="strategy-topics" value="topics" readonly="readonly">
    <label for="strategy-topics">Look at your syllabus to see what topics will be covered.</label><br />
    <input type="checkbox" name="strategy-reading" id="strategy-reading" value="reading" readonly="readonly">
    <label for="strategy-reading">Ensure you have completed the assigned reading.</label><br />
    <input type="checkbox" name="strategy-confusion" id="strategy-confusion" value="confusion" readonly="readonly">
    <label for="strategy-confusion">Identify areas of confusion.  This will help you focus during the class on areas of need.</label><br />
    <input type="checkbox" name="strategy-questions" id="strategy-questions" value="questions" readonly="readonly">
    <label for="strategy-questions">Prepare questions that you would like answered during class.</label><br />
    <h3>Notes:</h3>
    <textarea name="strategy-notes" readonly="readonly"></textarea>
</div>


