<?php

function tzintellitime_mock_drush_command() {
  $items['intellitime-mock-schedule-all'] = array(
    'callback' => 'tzintellitime_mock_drush_schedule_all',
    'description' => 'Login and schedule all intellitime mock users for sync',
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
    if(tzintellitime_server_auth($account, $mock_user->login, $mock_user->password)) {
      tzintellitime_sync_schedule_user($account->uid);
    }
  }

  variable_set('tzintellitime_mock_delay_millis', $old_delay);
}
