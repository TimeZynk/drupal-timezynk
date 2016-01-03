<?php

function tzintellitime_sync_drush_command() {
  $items['intellitime-partition'] = array(
    'callback' => 'tzintellitime_sync_drush_partition',
    'description' => dt('Fetch the users needing sync partitioned to number of configured threads'),
  );
  $items['intellitime-sync'] = array(
    'callback' => 'tzintellitime_sync_drush_sync',
    'description' => dt('Run intellitime sync for a set of users'),
    'arguments' => array(
      'users' => 'Comma-separated list of users to synchronize',
    ),
  );
  $items['intellitime-set-all-active'] = array(
    'callback' => 'tzintellitime_sync_drush_set_all_active',
    'description' => dt('Setsintellitime sync active status for all users'),
    'arguments' => array(
      'active' => '0 to deactivate, 1 to activate',
    ),
  );
  return $items;
}

function tzintellitime_sync_drush_help($section) {
  switch($section) {
    case 'drush:intellitime-partition':
      return dt('Fetch the users needing sync partitioned to number of configured threads');
    case 'drush:intellitime-sync':
      return dt('Run intellitime sync for a set of users');
  }
}

function tzintellitime_sync_drush_partition() {
  $users = array();
  $sync_limit = time() - TZINTELLITIME_SYNC_INTERVAL_MINUTES*60;
  $result = db_query('SELECT u.uid FROM {tzintellitime_sync} tz INNER JOIN {users} u ON u.uid = tz.uid WHERE u.status = 1 AND tz.intellitime_active = 1 AND tz.intellitime_last_sync <= %d ORDER BY tz.intellitime_last_sync ASC', $sync_limit);
  while($uid = db_result($result)) {
    $users[] = $uid;
  }

  $partitions = array_chunk($users, TZINTELLITIME_SYNC_USERS_PER_THREAD);
  $partition_string = array();
  foreach($partitions as $partition) {
    $partition_string[] = implode(',', $partition);
  }
  print(implode(';', $partition_string) . "\n");
}

function tzintellitime_sync_drush_sync($users) {
  $user_list = explode(',', $users);
  $stats = array();
  foreach($user_list as $user) {
    $before = microtime(TRUE);
    $user = trim($user);
    if(ctype_digit($user)) {
      $uid = intval($user, 10);
      $result = tzintellitime_sync_synchronize_user($uid);
      $after = microtime(TRUE);

      $result = isset($result) ? $result : array();
      $result['uid'] = $uid;
      $result['time'] = $after - $before;
      $stats[] = $result;
    }
  }

  module_invoke_all('synchronized_multiple_users', $stats);
}

function tzintellitime_sync_drush_set_all_active($activate) {
  $active = 0;
  if($activate) {
    $active = 1;
  }
  db_query('UPDATE {tzintellitime_sync} SET intellitime_active = %d', $active);
}
