<?php

function tzstats_record_user_status() {
  $now = time();

  // Save current status
  $result = db_query('SELECT * FROM {users} WHERE uid > 1 AND status = 1');
  while ($account = db_fetch_object($result)) {
    $status = tzuser_get_user_status($account, $now);
    $status_code = $status->getStatusCode();
    $entry = array(
      'status' => $status_code,
      'timestamp' => $now,
      'uid' => $account->uid,
    );
    drupal_write_record('tzstats_user', $entry);
  }
}