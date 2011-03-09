<?php

function tzuser_drush_command() {
  $items['tzuser-overview'] = array(
    'callback' => 'tzuser_drush_overview',
    'description' => dt('Fetch current user status overview'),
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

