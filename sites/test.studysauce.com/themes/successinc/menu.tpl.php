<?php
global $user;
$user = user_load($user->uid);
if(in_array('adviser', $user->roles)): ?>
    <ul>
        <li><a href="#userlist"><span>&nbsp;</span>Home</a></li>
        <li><a href="#import"><span>&nbsp;</span>Invite students</a>
        <li><a href="#settings"><span>&nbsp;</span>Settings</a>
        <ul>
            <li><a href="#account">Account information</a></li>
        </ul>
    </ul>
<?php else: ?>
<ul>
    <li><a href="#home"><span>&nbsp;</span>Home</a></li>
    <li><a href="#time"><span>&nbsp;</span>Time</a>
        <ul>
            <li><a href="#deadlines"><span>Deadlines</span></a></li>
            <li><a href="#plan"><span>Study plan <sup class="premium">Premium</sup></span></a></li>
            <li><a href="#metrics"><span>Study metrics</span></a></li>
            <li><a href="#tips-time"><span>Tips: Time Management</span></a></li>
        </ul>
    </li>
    <li><a href="#environment"><span>&nbsp;</span>Environment</a>
        <ul>
            <li><a href="#checkin"><span>Check in</span></a></li>
            <li><a href="#badges"><span>Badges</span></a></li>
            <li><a href="#tips-env"><span>Tips: Environment</span></a></li>
        </ul>
    </li>
    <li><a href="#strategies"><span>&nbsp;</span>Strategy</a>
        <ul>
            <li><a href="#profile"><span>Personal study profile <sup class="premium">Premium</sup></span></a></li>
            <li><a href="#goals"><span>Goals</span></a></li>
            <li><a href="#partner"><span>Accountability partner</span></a></li>
            <li><a href="#invite"><span>Study partners</span></a></li>
            <li><a href="#tips-strategy"><span>Tips: Strategies</span></a></li>
        </ul>
    </li>
    <li><a href="#premium"><span>&nbsp;</span>Premium</a></li>
    <li><a href="#settings"><span>&nbsp;</span>Settings</a>
        <ul>
            <li><a href="#schedule"><span>Class schedule</span></a></li>
            <li><a href="#account"><span>Account information</span></a></li>
        </ul>
    </li>
</ul>
<?php endif; ?>