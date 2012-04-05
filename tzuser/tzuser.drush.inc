<?php

function tzuser_drush_command() {
  $items['tzuser-overview'] = array(
    'callback' => 'tzuser_drush_overview',
    'description' => dt('Fetch current user status overview'),
  );

  $items['tzuser-redis'] = array(
    'callback' => 'tzuser_drush_prep_redis'
  );

  return $items;
}

function tzuser_drush_help($section) {
  switch($section) {
    case 'drush:tzuser-overview':
      return dt('Fetch current user status overview and return it in JSON format');
  }
}

function tzuser_drush_overview() {
  $user_status = array();
  $now = time();

  $result = db_query('SELECT * FROM {users} WHERE uid > 1 AND status = 1');
  while ($account = db_fetch_object($result)) {
    $status = tzuser_get_user_status($account, $now);
    $status_code = $status->getStatusCode($now);
    $user_status[] = array(
      'uid' => intval($account->uid, 10),
      'status' => $status_code
    );
  }

  print drupal_json($user_status) . "\n";
}

function tzuser_drush_prep_redis() {
  $redis = tzstats_open_redis();
  $host = tzbase_hostname();

  $result = db_query('SELECT * FROM {tzstats_user} ORDER BY id');
  while ($stat = db_fetch_object($result)) {
    $redis->sAdd('users:all', $host . ':' . $stat->uid);
    $redis->sAdd('users:' . $host . ':all', $stat->uid);
    $redis->setnx('users:' . $host . ':' . $stat->uid . ':created', $stat->timestamp);
  }

  $result = db_query('SELECT * FROM {tzstats_login} ORDER BY id');
  while ($stat = db_fetch_object($result)) {
    $redis->sAdd('users:all', $host . ':' . $stat->uid);
    $redis->sAdd('users:' . $host . ':all', $stat->uid);
    $redis->setnx('users:' . $host . ':' . $stat->uid . ':first_login', $stat->timestamp);
    $redis->set('users:' . $host . ':' . $stat->uid . ':last_login', $stat->timestamp);
    $redis->setnx('users:' . $host . ':' . $stat->uid . ':created', $stat->timestamp);
  }

  $result = db_query('SELECT * FROM {tzstats_report} ORDER BY id');
  while ($stat = db_fetch_object($result)) {
    tzstats_record_redis_report_stats(tzbase_make_date($stat->timestamp), false);
  }
}
