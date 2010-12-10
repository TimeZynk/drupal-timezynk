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
  $partitions = array();
  $sync_limit = time() - TZINTELLITIME_SYNC_INTERVAL_MINUTES*60;
  $result = db_query_range('SELECT u.uid FROM {tzintellitime_sync} tz INNER JOIN {users} u ON u.uid = tz.uid WHERE u.status = 1 AND tz.intellitime_active = 1 AND tz.intellitime_last_sync <= %d ORDER BY tz.intellitime_last_sync ASC', $sync_limit, 0, TZINTELLITIME_SYNC_USERS_PER_RUN);
  $i = 0;
  while($uid = db_result($result)) {
    $partitions[$i][] = $uid;
    $i++;
    if($i == TZINTELLITIME_SYNC_PARALLEL_THREADS) {
      $i = 0;
    }
  }

  $partition_string = array();
  foreach($partitions as $partition) {
    $partition_string[] = implode(',', $partition);
  }
  print(implode(';', $partition_string) . "\n");
}

function tzintellitime_sync_drush_sync($users) {
  $user_list = explode(',', $users);
  $before = microtime(TRUE);
  $successful = 0;
  $count = 0;
  foreach($user_list as $user) {
    $user = trim($user);
    if(ctype_digit($user)) {
      $uid = intval($user, 10);
      $count++;
      $result = tzintellitime_sync_synchronize_user($uid);
      if($result === TZIntellitimeSyncController::SYNC_OK) {
        $successful++;
      }
    }
  }
  $after = microtime(TRUE);

  module_invoke_all('synchronized_multiple_users', 'tzintellitime', $count, $successful, $before, $after);
}

function tzintellitime_sync_drush_set_all_active($activate) {
  $active = 0;
  if($activate) {
    $active = 1;
  }
  db_query('UPDATE {tzintellitime_sync} SET intellitime_active = %d', $active);
}