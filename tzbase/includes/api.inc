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
