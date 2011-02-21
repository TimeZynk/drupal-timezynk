<?php

function tzstats_record_user_status() {
  $now = time();

  // Find old status
  $old_status = array();
  /* Query rows with the maximum row id for every user to get latest status
   * This query was adapted from the following stack overflow thread:
   * http://stackoverflow.com/questions/121387
   */
  $result = db_query('SELECT u1.* FROM {tzstats_user} AS u1 LEFT OUTER JOIN {tzstats_user} AS u2 ON (u1.uid = u2.uid AND u1.id < u2.id) WHERE u2.uid IS NULL');
  while ($stats = db_fetch_object($result)) {
    $old_status[$stats->uid] = $stats->status;
  }

  // Compare to current status
  $result = db_query('SELECT * FROM {users} WHERE uid > 1 AND status = 1');
  while ($account = db_fetch_object($result)) {
    $status = tzuser_get_user_status($account, $now);
    $status_code = $status->getStatusCode($now);
    if (!isset($old_status[$account->uid]) || $status_code != $old_status[$account->uid]) {
      $entry = array(
        'status' => $status_code,
        'timestamp' => $now,
        'uid' => $account->uid,
      );
      drupal_write_record('tzstats_user', $entry);
    }
  }
}