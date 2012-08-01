<?php

function tzstats_api_user_history_index() {
  $result = db_query('SELECT tzu.* FROM {tzstats_user} tzu INNER JOIN {users} u ON u.uid = tzu.uid WHERE u.status = 1 AND u.uid > 1 ORDER BY id');
  tzapi_db_print_json($result);
}

function tzstats_api_logins_index() {
  $interval = tzapi_interval_params();

  $query = 'SELECT * FROM {tzstats_login} WHERE timestamp >= %d AND timestamp < %d';
  $result = db_query($query, $interval[0], $interval[1]);
  tzapi_db_print_json($result, function ($e) {
    return array(
      'timestamp' => intval($e->timestamp),
      'user' => array(
        'id' => $e->uid,
        'url' => url('api/users/' . $e->uid),
      ),
    );
  });
}

function tzstats_api_fetched_reports() {
  $interval = tzapi_interval_params();

  $query = 'SELECT * FROM {tzstats_fetched_reports} WHERE timestamp >= %d AND timestamp < %d';
  $result = db_query($query, $interval[0], $interval[1]);
  tzapi_db_print_json($result, function ($e) {
    return array(
      'count' => intval($e->count),
      'timestamp' => intval($e->timestamp),
      'user' => array(
        'id' => $e->uid,
        'url' => url('api/users/' . $e->uid, array('absolute' => TRUE)),
      ),
    );
  });
}

function tzstats_api_reports() {
  $interval = tzapi_interval_params();

  $result = db_query('SELECT * FROM {tzstats_report} tzstats_report LEFT JOIN {tzreport} tzreport ON tzreport.vid = tzstats_report.vid ' .
                     'WHERE tzstats_report.timestamp >= %d AND tzstats_report.timestamp < %d', $interval[0], $interval[1]);

  tzapi_db_print_json($result, function ($entry) {
    $e = array(
      'timestamp' => intval($entry->timestamp),
      'user' => array(
        'id' => $entry->uid,
        'url' => url('api/users/' . $entry->uid),
      ),
    );
    if (!empty($entry->vid)) {
      $e['report'] = array(
        'start' => intval($entry->begintime),
        'end' => intval($entry->endtime),
        'url' => url('api/reports/' . $entry->nid . '/' . $entry->vid),
      );
    }
    return $e;
  });
}

function tzstats_api_sms() {
  $interval = tzapi_interval_params();

  $query = 'SELECT * FROM {tzstats_sms} WHERE timestamp >= %d AND timestamp < %d';
  $result = db_query($query, $interval[0], $interval[1]);
  tzapi_db_print_json($result, function ($e) {
    return array(
      'type' => tzsms_format_type($e->sms_type),
      'length' => intval($e->length),
      'destination' => $e->destination,
      'timestamp' => intval($e->timestamp),
      'user' => array(
        'id' => $e->uid,
        'url' => url('api/users/' . $e->uid),
      ),
    );
  });
}

function tzstats_api_availability() {
  $interval = tzapi_interval_params();

  $query = 'SELECT * FROM {tzstats_availability} WHERE timestamp >= %d AND timestamp < %d';
  $result = db_query($query, $interval[0], $interval[1]);

  tzapi_db_print_json($result, function ($e) {
    return array(
      'timestamp' => intval($e->timestamp),
      'user' => array(
        'id' => $e->uid,
        'url' => url('api/users/' . $e->uid),
      ),
    );
  });
}


