SET @start_date = '2014-01-31';
SET @end_date = '2014-02-07';


DROP TEMPORARY TABLE studysauce.user_checkins;
CREATE TEMPORARY TABLE IF NOT EXISTS studysauce.user_checkins
  AS (SELECT
        u.uid,
        SUM(count) AS count
      FROM (
             SELECT
# Join with users to count the number of unique users checking in 
               n.uid,
               COUNT(ch.entity_id) AS count
             FROM (
                    SELECT
                      entity_id
                    FROM studysauce.field_revision_field_checkin
                    WHERE field_checkin_value IS NOT NULL
                          AND field_checkin_value > @start_date
                          AND field_checkin_value < @end_date
# Filter out accidental checkins that happened right next to each other, there must be at least 2 minutes difference in time
                      GROUP BY entity_id, FLOOR(field_checkin_value / 120)
                  ) AS ch
               LEFT JOIN studysauce.field_data_field_classes AS cl
                 ON ch.entity_id = cl.field_classes_value
               LEFT JOIN studysauce.node AS n
                 ON cl.entity_id = n.nid
             GROUP BY ch.entity_id) AS distinct_users
        LEFT JOIN studysauce.users AS u
          ON distinct_users.uid = u.uid
      GROUP BY u.uid);

# get the users that have created schedules
DROP TEMPORARY TABLE studysauce.user_schedule;
CREATE TEMPORARY TABLE IF NOT EXISTS studysauce.user_schedule
  AS (SELECT
        users.uid,
        COUNT(*)
      FROM studysauce.node, studysauce.users
      WHERE node.uid = users.uid
            AND node.type = 'schedule'
            AND node.created > UNIX_TIMESTAMP(@start_date)
            AND node.created < UNIX_TIMESTAMP(@end_date)
            AND users.created > UNIX_TIMESTAMP(@start_date)
            AND users.created < UNIX_TIMESTAMP(@end_date)
      GROUP BY users.uid);

# get the users that have created incentives
DROP TEMPORARY TABLE studysauce.user_incentive;
CREATE TEMPORARY TABLE IF NOT EXISTS studysauce.user_incentive
  AS (SELECT
        users.uid,
        COUNT(*)
      FROM studysauce.node, studysauce.users
      WHERE node.uid = users.uid
            AND node.type = 'incentive'
            AND node.created > UNIX_TIMESTAMP(@start_date)
            AND node.created < UNIX_TIMESTAMP(@end_date)
            AND users.created > UNIX_TIMESTAMP(@start_date)
            AND users.created < UNIX_TIMESTAMP(@end_date)
      GROUP BY users.uid);


# get the number of goals that have been created
DROP TEMPORARY TABLE studysauce.user_goal;
CREATE TEMPORARY TABLE IF NOT EXISTS studysauce.user_goal
  AS (SELECT
        nid,
        delta,
        COUNT(nid) AS count,
        MAX(vid)   AS vid,
        uid
      FROM (
             SELECT
               nid,
               MAX(delta)  AS delta,
               MAX(nr.vid) AS vid,
               uid
             FROM (
                    SELECT
                      entity_id,
                      MAX(revision_id) AS vid,
                      field_goals_value,
                      MAX(delta)       AS delta
                    FROM studysauce.field_revision_field_goals
                    GROUP BY revision_id, field_goals_value
                  ) AS gr
               LEFT JOIN studysauce.node_revision AS nr
                 ON gr.vid = nr.vid
             WHERE nr.timestamp > UNIX_TIMESTAMP(@start_date)
                   AND nr.timestamp < UNIX_TIMESTAMP(@end_date)
             GROUP BY field_goals_value
           ) AS gr
      GROUP BY nid);

SELECT
  *
FROM studysauce.user_goal;

# get a list of invites
DROP TEMPORARY TABLE studysauce.user_invite;
CREATE TEMPORARY TABLE IF NOT EXISTS studysauce.user_invite
  AS (SELECT
        uid,
        COUNT(uid) AS count
      FROM (
             SELECT
               s.entity_id AS eid,
               i.entity_id AS uid
             FROM studysauce.field_data_field_sent AS s
               LEFT JOIN studysauce.field_data_field_email AS e
                 ON s.entity_id = e.entity_id
               LEFT JOIN studysauce.field_data_field_invites AS i
                 ON s.entity_id = i.field_invites_value
             WHERE e.field_email_value IS NOT NULL
                   AND e.field_email_value != ''
                   AND field_sent_value > @start_date
                   AND field_sent_value < @end_date
             GROUP BY s.entity_id
           ) AS invites
      GROUP BY uid);

SELECT
  *
FROM studysauce.user_invite;

# get the number of reminders
DROP TEMPORARY TABLE studysauce.user_reminder;
CREATE TEMPORARY TABLE IF NOT EXISTS studysauce.user_reminder
  AS (SELECT
        nid,
        MAX(delta) AS delta,
        COUNT(nid) AS count,
        MAX(vid)   AS vid,
        uid
      FROM (
             SELECT
               nid,
               MAX(delta)  AS delta,
               MAX(nr.vid) AS vid,
               uid
             FROM (
                    SELECT
                      entity_id,
                      MAX(revision_id) AS vid,
                      field_reminders_value,
                      MAX(delta)       AS delta
                    FROM studysauce.field_revision_field_reminders
                    GROUP BY revision_id, field_reminders_value
                  ) AS gr
               LEFT JOIN studysauce.node_revision AS nr
                 ON gr.vid = nr.vid
             WHERE nr.timestamp > UNIX_TIMESTAMP(@start_date)
                   AND nr.timestamp < UNIX_TIMESTAMP(@end_date)
             GROUP BY field_reminders_value
           ) AS gr
      GROUP BY nid);

DROP TEMPORARY TABLE studysauce.user_parent;
CREATE TEMPORARY TABLE IF NOT EXISTS studysauce.user_parent
  AS (
    SELECT
      uid
    FROM studysauce.field_data_field_parent_student AS ps,
      studysauce.users AS u
    WHERE ps.entity_id = u.uid
          AND ps.field_parent_student_value = 'parent'
  );

DROP TEMPORARY TABLE studysauce.user_student;
CREATE TEMPORARY TABLE IF NOT EXISTS studysauce.user_student
  AS (
    SELECT
      uid
    FROM studysauce.field_data_field_parent_student AS ps,
      studysauce.users AS u
    WHERE ps.entity_id = u.uid
          AND ps.field_parent_student_value = 'student'
  );

SET @invites = (SELECT
                  SUM(count)
                FROM studysauce.user_invite);

SET @parent_to_student_invites = (SELECT
                                    SUM(count)
                                  FROM studysauce.user_invite AS ui
                                  WHERE ui.uid IN (SELECT
                                                     uid
                                                   FROM studysauce.user_parent));

SET @users_sent_invites = (SELECT
                             COUNT(*)
                           FROM studysauce.user_invite);

SET @parent_sent_invites = (SELECT
                              COUNT(*)
                            FROM studysauce.user_invite AS ui
                            WHERE ui.uid IN (SELECT
                                               uid
                                             FROM studysauce.user_parent));

SET @student_signups = (SELECT
                          COUNT(*)
                        FROM studysauce.users AS u
                        WHERE u.uid IN (SELECT
                                          uid
                                        FROM studysauce.user_student)
                              AND u.created > UNIX_TIMESTAMP(@start_date)
                              AND u.created < UNIX_TIMESTAMP(@end_date));

SET @parent_signups = (SELECT
                         COUNT(*)
                       FROM studysauce.users AS u
                       WHERE u.uid IN (SELECT
                                         uid
                                       FROM studysauce.user_parent)
                             AND u.created > UNIX_TIMESTAMP(@start_date)
                             AND u.created < UNIX_TIMESTAMP(@end_date));

SET @checkins = (SELECT
                   SUM(count)
                 FROM studysauce.user_checkins);

SET @users_checking_in = (SELECT
                            COUNT(uid)
                          FROM studysauce.user_checkins);
SET @schedules = (SELECT
                    COUNT(*)
                  FROM studysauce.user_schedule);

SET @incentives = (SELECT
                     COUNT(*)
                   FROM studysauce.user_incentive);

SET @goals = (SELECT
                SUM(count)
              FROM studysauce.user_goal);

SET @parent_goals = (SELECT
                       SUM(count)
                     FROM studysauce.user_goal AS ug
                     WHERE ug.uid IN (SELECT
                                        uid
                                      FROM studysauce.user_parent));

SET @reminders = (SELECT
                    IF(SUM(count) IS NULL, 0, SUM(count))
                  FROM studysauce.user_reminder);

SET @users_with_reminders = (SELECT
                               COUNT(*)
                             FROM studysauce.user_reminder);

SET @users_not_in_schedules = (SELECT
                                 COUNT(*)
                               FROM studysauce.user_incentive
                               WHERE uid NOT IN (SELECT
                                                   uid
                                                 FROM studysauce.user_schedule));

SET @parents_incentives = (SELECT
                             COUNT(*)
                           FROM studysauce.user_incentive AS ug
                           WHERE ug.uid IN (SELECT
                                              uid
                                            FROM studysauce.user_parent));

SET @users_not_in_incentives = (SELECT
                                  COUNT(*)
                                FROM studysauce.user_invite
                                WHERE uid NOT IN (SELECT
                                                    uid
                                                  FROM studysauce.user_incentive
                                                  UNION SELECT
                                                          uid
                                                        FROM studysauce.user_schedule));

SET @users_not_in_invites = (SELECT
                               COUNT(*)
                             FROM studysauce.user_reminder
                             WHERE uid NOT IN (SELECT
                                                 uid
                                               FROM studysauce.user_invite
                                               UNION SELECT
                                                       uid
                                                     FROM studysauce.user_incentive
                                               UNION SELECT
                                                       uid
                                                     FROM studysauce.user_schedule));

SET @active_parents = (SELECT
                         @parents_incentives + (SELECT
                                                  COUNT(*)
                                                FROM studysauce.user_invite AS ui
                                                WHERE ui.uid IN (SELECT
                                                                   uid
                                                                 FROM studysauce.user_parent)
                                                      AND ui.uid NOT IN (SELECT
                                                                           uid
                                                                         FROM studysauce.user_incentive)));

SET @active_students = (SELECT
                          DISTINCT COUNT(uid)
                        FROM (
                               SELECT
                                 uid
                               FROM studysauce.user_schedule
                               UNION SELECT
                                       uid
                                     FROM studysauce.user_incentive
                               UNION SELECT
                                       uid
                                     FROM studysauce.user_reminder
                               UNION SELECT
                                       uid
                                     FROM studysauce.user_invite
                             ) AS distinct_users
                        WHERE uid IN (SELECT
                                        uid
                                      FROM studysauce.user_student));


SET @active_users = (SELECT
                       @schedules + @users_not_in_schedules + @users_not_in_incentives +
                       @users_not_in_invites);


SELECT
  'New students'   AS label,
  @student_signups AS value
UNION
SELECT
  'New parents'   AS label,
  @parent_signups AS value
UNION
SELECT
  'Check-ins' AS label,
  @checkins   AS value
UNION
SELECT
  'Users who checked in' AS label,
  @users_checking_in     AS value
UNION
SELECT
  'Class schedules entered' AS label,
  @schedules                AS value
UNION
SELECT
  'Goals established' AS label,
  @goals              AS value
UNION
SELECT
  'Goals sponsored by parents' AS label,
  @parent_goals                AS value
UNION
SELECT
  'Class incentives entered' AS label,
  @incentives                AS value
UNION
SELECT
  'Incentives entered by parents' AS label,
  @parents_incentives             AS value
UNION
SELECT
  'Number of invites' AS label,
  @invites            AS value
UNION
SELECT
  'Parent to student invites' AS label,
  @parent_to_student_invites  AS value
UNION
SELECT
  'Users who sent invites' AS label,
  @users_sent_invites      AS value
UNION
SELECT
  'Parents who sent invites' AS label,
  @parent_sent_invites       AS value
UNION
SELECT
  'Number of reminders' AS label,
  @reminders            AS value
UNION
SELECT
  'Users with reminders' AS label,
  @users_with_reminders  AS value
UNION
SELECT
  'Active users' AS label,
  @active_users  AS value
UNION
SELECT
  'Active students' AS label,
  @active_students  AS value
UNION
SELECT
  'Active parents' AS label,
  @active_parents  AS value;






