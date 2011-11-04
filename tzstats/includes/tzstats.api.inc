<?php

function tzstats_api_user_history_index() {
  drupal_set_header('Content-Type: application/json; charset=utf-8');
  echo "[ ";
  $result = db_query('SELECT tzu.* FROM {tzstats_user} tzu INNER JOIN {users} u ON u.uid = tzu.uid WHERE u.status = 1 AND u.uid > 1 ORDER BY id');
  $first = TRUE;
  while ($row = db_fetch_object($result)) {
    if (!$first) {
      echo ', ';
    }
    echo json_encode($row);
    $first = FALSE;
  }
  echo " ]";
}

function tzstats_api_logins_index() {
  $timestamp = time();
  if (!empty($_GET['timestamp'])) {
    $timestamp = intval($_GET['timestamp'], 10);
  }

  $interval = 24*3600;
  if (!empty($_GET['interval'])) {
    $interval = intval($_GET['interval'], 10);
  }

  $query = 'SELECT COUNT(DISTINCT(uid)) FROM {tzstats_login} WHERE timestamp > %d AND timestamp < %d';
  $result = db_result(db_query($query, $timestamp - $interval, $timestamp));
  drupal_set_header('Content-Type: application/json; charset=utf-8');
  print json_encode(array(
    'from' => $timestamp - $interval,
    'to' => $timestamp,
    'logins' => intval($result),
  ));
}
