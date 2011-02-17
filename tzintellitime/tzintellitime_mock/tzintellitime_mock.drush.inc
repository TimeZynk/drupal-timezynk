<?php

function tzintellitime_mock_drush_command() {
  $items['intellitime-mock-schedule-all'] = array(
    'callback' => 'tzintellitime_mock_drush_schedule_all',
    'description' => 'Login and schedule all intellitime mock users for sync',
  );
  $items['intellitime-mock-reset-all'] = array(
    'callback' => 'tzintellitime_mock_drush_reset_all',
    'description' => 'Reset all mock tables. NULLify all data!',
  );
  $items['intellitime-mock-add-user'] = array(
    'callback' => 'tzintellitime_mock_drush_add_user',
    'description' => 'Add a new user to the mock',
    'arguments' => array(
      'username' => 'Username',
      'password' => 'Password',
    ),
  );
  $items['intellitime-mock-list-users'] = array(
    'callback' => 'tzintellitime_mock_drush_list_users',
    'description' => 'List all mock users',
  );
  $items['intellitime-mock-add-assignment'] = array(
    'callback' => 'tzintellitime_mock_drush_add_assignment',
    'description' => 'Add a new assignment to the mock',
    'arguments' => array(
      'title' => 'Title',
    ),
  );
  $items['intellitime-mock-list-assignments'] = array(
    'callback' => 'tzintellitime_mock_drush_list_assignments',
    'description' => 'List all mock assignments',
  );
  $items['intellitime-mock-add-report'] = array(
    'callback' => 'tzintellitime_mock_drush_add_report',
    'description' => 'Add a new report to the mock',
    'arguments' => array(
      'user_id' => 'User ID',
      'assignment_id' => 'Assignment ID',
      'year' => 'Year',
      'month' => 'Month',
      'day' => 'Day',
      'begintime' => 'Begintime like 08:00',
      'endtime' => 'Endtime like 16:30',
      'breakminutes' => 'Breaktime in minutes',
      'comment' => 'Optional comment',
    ),
  );
  $items['intellitime-mock-list-reports'] = array(
    'callback' => 'tzintellitime_mock_drush_list_reports',
    'description' => 'List all mock reports',
  );
  return $items;
}

function tzintellitime_mock_drush_schedule_all() {
  global $user;

  $old_delay = TZINTELLITIME_MOCK_DELAY_MILLIS;
  variable_set('tzintellitime_mock_delay_millis', 0);

  $result = db_query('SELECT * FROM {tzintellitime_mock_users}');
  while($mock_user = db_fetch_object($result)) {
    $account = user_load(array('name' => $mock_user->login, 'status' => 1));
    $account = tzintellitime_server_auth($account, $mock_user->login, $mock_user->password);
    if($account) {
      tzintellitime_sync_schedule_user($account->uid);
    }
  }

  variable_set('tzintellitime_mock_delay_millis', $old_delay);
}

function tzintellitime_mock_drush_reset_all() {
  db_query('DELETE FROM {tzintellitime_mock_users}');
  db_query('DELETE FROM {tzintellitime_mock_data');
}

function tzintellitime_mock_drush_add_user($username, $password) {
  $db_object = array(
    'login' => $username,
    'fullname' => $username,
    'password' => $password,
  );
  drupal_write_record('tzintellitime_mock_users', $db_object);
  $user_id = empty($db_object['user_id']) ? 0 : $db_object['user_id'];
  print $user_id . "\n";
}

function tzintellitime_mock_drush_list_users() {
  $result = db_query('SELECT * FROM {tzintellitime_mock_users} ORDER BY user_id ASC');
  while($mock_user = db_fetch_object($result)) {
    printf("%d '%s' '%s'\n", $mock_user->user_id, $mock_user->login, $mock_user->password);
  }
}

function tzintellitime_mock_drush_add_assignment($title) {
  $assignment = new TZIntellitimeAssignment();
  $assignment->title = $title;
  $assignment->type = TZIntellitimeAssignment::TYPE_ASSIGNMENT;

  $id = tzintellitime_mock_store_object($assignment);
  print $id . "\n";
}

function tzintellitime_mock_drush_list_assignments() {
  $result = db_query('SELECT * FROM {tzintellitime_mock_data} WHERE user_id = 0 ORDER BY id ASC');
  while($row = db_fetch_object($result)) {
    $obj = unserialize($row->data);
    if($obj instanceof TZIntellitimeAssignment) {
      printf("%d '%s' %d\n", $row->id, $obj->title, $obj->type);
    }
  }
}

function tzintellitime_mock_drush_add_report($user_id, $assignment, $year, $month, $day, $begin, $end, $break=0, $comment='') {
  $mock_user = tzintellitime_mock_load_user($user_id);
  if(empty($mock_user)) {
    drush_log("Illegal user id: " . $user_id, 'error');
    return;
  }

  $report = new TZIntellitimeReport();

  /* As the second argument we accept either a numeric assignment ID or a string to be used
   * as the assignment title for this report */
  if (ctype_digit($assignment)) {
    $intelli_assignment = tzintellitime_mock_load_object($assignment);
    if(empty($intelli_assignment)) {
      drush_log("Illegal assignment id: " . $assignment, 'error');
      return;
    }

    $report->title = $intelli_assignment->title;
    $report->assignment_id = $intelli_assignment->id;
  } else {
    $report->title = $assignment;
  }

  $report->user_id = $user_id;
  $report->year = intval($year, 10);
  $report->month = intval($month, 10);
  $report->day = intval($day, 10);
  $report->begin = $begin;
  $report->end = $end;
  $report->break_duration_minutes = $break;

  $comment_args = array_slice(func_get_args(), 8);
  $report->comment = implode(' ', $comment_args);

  $id = tzintellitime_mock_store_object($report);
  print $id . "\n";
}

function tzintellitime_mock_drush_list_reports() {
  $reports = tzintellitime_mock_get_reports();
  foreach($reports as $report) {
    if (empty($report->assignment_id)) {
      printf("%d %d '%s' %04d-%02d-%02d %s %s %d '%s'\n", $report->id, $report->user_id,
        $report->title, $report->year, $report->month, $report->day,
        $report->begin, $report->end, $report->break_duration_minutes, $report->comment);

    } else {
      printf("%d %d %d %04d-%02d-%02d %s %s %d '%s'\n", $report->id, $report->user_id,
        $report->assignment_id, $report->year, $report->month, $report->day,
        $report->begin, $report->end, $report->break_duration_minutes, $report->comment);
    }
  }
}
