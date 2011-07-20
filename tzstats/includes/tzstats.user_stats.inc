<?php

function tzstats_user_stats_json() {
  $server_name = str_replace('.tzapp.com', '', $_SERVER['HTTP_HOST']);
  drupal_set_header('Content-Type: text/javascript; charset=utf-8');
  drupal_set_header('Content-Disposition: attachment; filename=' . $server_name . '-user-stats.json');
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