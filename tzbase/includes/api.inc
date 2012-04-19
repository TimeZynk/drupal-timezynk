<?php

/**
 * List all neighbouring site URLs
 */
function tzbase_api_sites_index() {
  $sites_dir = getcwd() . '/sites';
  $files = scandir(getcwd() . '/sites');
  $sites = array_filter($files, function($f) use ($sites_dir) {
    if ($f[0] == '.' ||
        $f == 'all' ||
        $f == 'default') {
      return FALSE;
    }
    return is_file("$sites_dir/$f/settings.php");
  });
  $prefix = !empty($_SERVER['HTTPS']) ? 'https' : 'http';
  $sites = array_map(function($s) use ($prefix) {
    return $prefix . '://' . $s . '/';
  }, $sites);
  $sites = array_values($sites);

  drupal_set_header('Content-Type: application/json; charset=utf-8');
  print json_encode($sites);
}

function tzbase_api_info() {
  $info = array(
    'name' => variable_get('site_name', ''),
    'url' => url('/', array('absolute' => TRUE))
  );
  drupal_alter('api_info', $info);
  drupal_set_header('Content-Type: application/json; charset=utf-8');
  print json_encode($info);
}

function tzbase_availabilities_index() {
  $interval = tzapi_interval_params();
  $availability_store = tzbase_availability_store();

  $user_id = 0;
  if (!empty($_GET['user_id'])) {
    $user_id = $_GET['user_id'];
  }
  $avs = $availability_store->findBetween(
    $user_id,
    tzbase_make_date($interval[0]),
    tzbase_make_date($interval[1])
  );
  $avs = array_map(function($av) {
    return $av->getFields();
  }, $avs);
  drupal_set_header('Content-Type: application/json; charset=utf-8');
  print json_encode($avs);
}

function tzbase_reports_index() {
  $interval = tzapi_interval_params();

  $query = 'SELECT * FROM {node} n ' .
      'INNER JOIN {node_revisions} rev ON n.vid = rev.vid ' .
      'INNER JOIN {tzreport} tzr ON n.vid = tzr.vid ' .
      'WHERE tzr.flags != 255';
  $query_args = array();

  $query .= ' AND tzr.begintime >= %d';
  $query_args[] = $interval[0];

  $query .= ' AND tzr.begintime <= %d';
  $query_args[] = $interval[1];

  if (!empty($_GET['user_id'])) {
    $query .= ' AND tzr.assignedto = %d';
    $query_args[] = $_GET['user_id'];
  }

  $query .= ' ORDER BY tzr.begintime';
  $result = db_query($query, $query_args);

  drupal_set_header('Content-Type: application/json; charset=utf-8');
  print "[";
  $first = TRUE;
  while ($report = db_fetch_object($result)) {
    if (!$first) {
      print ",";
    }
    $first = FALSE;
    $report->assignment_path = theme('assignment_path', $report->jobid);
    print json_encode($report);
  }
  print "]";
}
