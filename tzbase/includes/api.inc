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

  drupal_set_header('Content-Type: application/json; charset=utf-8');
  print "[";
  $first = TRUE;
  foreach($avs as $av) {
    if (!$first) { print ","; } $first = FALSE;
    print json_encode($av->getFields());
  }
  print "]";
}

function tzbase_reports_index() {
  $interval = tzapi_interval_params();

  $query = 'SELECT n.nid,rev.title,rev.body,tzr.* FROM {node} n ' .
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

  $result = db_query($query, $query_args);

  tzapi_db_print_json($result);
}
