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
  $interval = _tzstats_interval_params();

  $query = 'SELECT * FROM {tzstats_login} WHERE timestamp >= %d AND timestamp < %d';
  $result = db_query($query, $interval[0], $interval[1]);
  $entries = array();
  while ($e = db_fetch_object($result)) {
    $entries[] = array(
      'timestamp' => intval($e->timestamp),
      'user' => array(
        'id' => $e->uid,
        'url' => url('api/users/' . $e->uid, array('absolute' => TRUE)),
      ),
    );
  }
  drupal_set_header('Content-Type: application/json; charset=utf-8');
  print json_encode($entries);
}

function tzstats_api_fetched_reports() {
  $interval = _tzstats_interval_params();

  $query = 'SELECT * FROM {tzstats_fetched_reports} WHERE timestamp >= %d AND timestamp < %d';
  $result = db_query($query, $interval[0], $interval[1]);
  $entries = array();
  while ($e = db_fetch_object($result)) {
    $entries[] = array(
      'count' => intval($e->count),
      'timestamp' => intval($e->timestamp),
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
  $interval = _tzstats_interval_params();

  $result = db_query('SELECT * FROM {tzstats_report} tzstats_report LEFT JOIN {tzreport} tzreport ON tzreport.vid = tzstats_report.vid ' .
                     'WHERE tzstats_report.timestamp >= %d AND tzstats_report.timestamp < %d', $interval[0], $interval[1]);
  $entries = array();
  while ($entry = db_fetch_object($result)) {
    $e = array(
      'timestamp' => intval($entry->timestamp),
      'user' => array(
        'id' => $entry->uid,
        'url' => url('api/users/' . $entry->uid, array('absolute' => TRUE)),
      ),
    );
    if (!empty($entry->vid)) {
      $e['report'] = array(
        'start' => intval($entry->begintime),
        'end' => intval($entry->endtime),
        'url' => url('api/reports/' . $entry->nid . '/' . $entry->vid, array('absolute' => TRUE)),
      );
    }
    $entries[] = $e;
  }

  drupal_set_header('Content-Type: application/json; charset=utf-8');
  print json_encode($entries);
}

function tzstats_api_sms() {
  $interval = _tzstats_interval_params();

  $query = 'SELECT * FROM {tzstats_sms} WHERE timestamp >= %d AND timestamp < %d';
  $result = db_query($query, $interval[0], $interval[1]);
  $entries = array();
  while ($e = db_fetch_object($result)) {
    $entries[] = array(
      'type' => tzsms_format_type($e->sms_type),
      'length' => intval($e->length),
      'destination' => $e->destination,
      'timestamp' => intval($e->timestamp),
      'user' => array(
        'id' => $e->uid,
        'url' => url('api/users/' . $e->uid, array('absolute' => TRUE)),
      ),
    );
  }
  drupal_set_header('Content-Type: application/json; charset=utf-8');
  print json_encode($entries);
}

function _tzstats_interval_params() {
  $now = time();
  $default_length = 24*3600;
  $interval = array($now - $default_length, $now);

  if (!empty($_GET['timestamp'])) {
    $interval[1] = intval($_GET['timestamp'], 10);
  }
  if (!empty($_GET['to'])) {
    $interval[1] = intval($_GET['to'], 10);
  }
  $interval[0] = $interval[1] - $default_length;

  if (isset($_GET['from'])) {
    $interval[0] = intval($_GET['from'], 10);
  }

  if (!empty($_GET['interval'])) {
    $interval[0] = $interval[1] - intval($_GET['interval'], 10);
  }

  return $interval;
}
