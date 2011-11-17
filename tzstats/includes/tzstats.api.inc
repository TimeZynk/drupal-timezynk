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

  $query = 'SELECT * FROM {tzstats_login} WHERE timestamp > %d AND timestamp < %d';
  $result = db_query($query, $timestamp - $interval, $timestamp);
  $entries = array();
  while ($e = db_fetch_object($result)) {
    $entries[] = array(
      'timestamp' => $e->timestamp,
      'user' => array(
        'id' => $e->uid,
        'url' => url('api/users/' . $e->uid, array('absolute' => TRUE)),
      ),
    );
  }
  drupal_set_header('Content-Type: application/json; charset=utf-8');
  print json_encode($entries);
}

function tzstats_api_reports() {
  $timestamp = time();
  if (!empty($_GET['timestamp'])) {
    $timestamp = intval($_GET['timestamp'], 10);
  }

  $interval = 24*3600;
  if (!empty($_GET['interval'])) {
    $interval = intval($_GET['interval'], 10);
  }

  $result = db_query('SELECT * FROM {tzstats_report} tzstats_report LEFT JOIN {tzreport} tzreport ON tzreport.vid = tzstats_report.vid ' .
                     'WHERE tzstats_report.timestamp >= %d AND tzstats_report.timestamp <= %d', $timestamp - $interval, $timestamp);
  $entries = array();
  while ($entry = db_fetch_object($result)) {
    $e = array(
      'timestamp' => $entry->timestamp,
      'user' => array(
        'id' => $entry->uid,
        'url' => url('api/users/' . $entry->uid, array('absolute' => TRUE)),
      ),
    );
    if (!empty($entry->vid)) {
      $e['report'] = array(
        'start' => $entry->begintime,
        'end' => $entry->endtime,
        'url' => url('api/reports/' . $entry->nid . '/' . $entry->vid, array('absolute' => TRUE)),
      );
    }
    $entries[] = $e;
  }

  drupal_set_header('Content-Type: application/json; charset=utf-8');
  print json_encode($entries);
}
