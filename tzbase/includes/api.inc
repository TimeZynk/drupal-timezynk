<?php

/**
 * List all neighbouring site URLs
 */
function tzbase_api_sites_index() {
  $files = scandir(getcwd() . '/sites');
  $sites = array_filter($files, function($f) {
    return !in_array($f, array('.', '..', 'all', 'default'));
  });
  $prefix = (isset($_SERVER['HTTPS']) && $_SERVER['HTTPS'] == 'on') ? 'https' : 'http';
  $sites = array_map(function($s) use ($prefix) {
    return $prefix . '://' . $s . '/';
  }, $sites);
  $sites = array_values($sites);

  drupal_set_header('Content-Type: application/json; charset=utf-8');
  print json_encode($sites);
}
