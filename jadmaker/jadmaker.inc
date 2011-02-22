<?php
/**
 * @file Contains the jad generation routines
 */

/**
 * Generate a JAD file
 */
function jadmaker_create_jad() {
  $ua = $_SERVER['HTTP_USER_AGENT'];

  jadmaker_store_download_stats();

  // Try to serve up APK-file if available
  if(false !== strpos($ua, 'Android')) {
    $apkfile = 'tz-Generic-android-sv.apk';
    $apkpath = file_create_path($apkfile);
    if(file_exists($apkpath)) {
      file_transfer($apkfile, array(
        'Content-Type: application/vnd.android.package-archive',
        'Content-Disposition: attachment; filename="' . $apkfile . '";',
        'Content-Length: ' . filesize(file_create_path($apkfile)),
      ));
    } else {
      drupal_goto('/mobile');
    }
  }

  if(false !== strpos($ua, 'iPhone') ||
     false !== strpos($ua, 'iPad')) {
    // Check for customer-specific overrride
    $mobile_path = file_directory_path() . '/mobile';
    if(file_exists($mobile_path)) {
      drupal_goto(file_create_url($mobile_path));
    } else {
      drupal_goto('mobile');
    }
  }

  $jarfile = 'tz-Generic-AnyPhone-sv.jar';

  if(false !== strpos($ua, 'SEMC') ||
     false !== strpos($ua, 'SonyEricsson')) {
    $jarfile = 'tz-Sony-Ericsson-JavaPlatform8-sv.jar';
  }

  $jarpath = file_create_path($jarfile);
  if(!file_exists($jarpath)) {
    drupal_set_message('JAR file "' . $jarpath . '" not found!');
    return drupal_not_found();
  }

  // check jar statistics
  $jarstat = stat($jarpath);

  $cid = 'jadmaker:' . $jarfile;

  $jaddata = cache_get($cid);
  if(!$jaddata || ($jaddata->created < $jarstat['mtime'])) {
    // Cached data did not exist or is older than jar file

    // Read manifest
    $jzip = zip_open($jarpath);
    if(!is_resource($jzip)) {
      drupal_set_message('Could not open jar file "' . $jarpath . '" ' . $jzip);
      return drupal_not_found();
    }
    while($entry = zip_read($jzip)) {
      if('META-INF/MANIFEST.MF' == zip_entry_name($entry)) {
        $manifest = zip_entry_read($entry, zip_entry_filesize($entry));
        break;
      }
    }
    zip_close($jzip);

    if(!$manifest) {
      drupal_set_message('Could not read jar manifest from "' . $jarpath . '"');
      return drupal_not_found();
    }

    // We only want the MIDlet- items from the jar manifest
    $manifest = explode("\n", $manifest);
    $manifest = preg_grep('/^[^:]*MIDlet-[^:]*:/', $manifest);

    // build jad data
    $jaddata = array_slice($manifest, 0, 3);
    // Insert jar file and size as row 4-5
    $jaddata[] = 'MIDlet-Jar-URL: ' . url($jarpath, array('absolute' => TRUE));
    $jaddata[] = 'MIDlet-Jar-Size: ' . $jarstat['size'];
    $jaddata = array_merge($jaddata, array_slice($manifest, 3));
    $jaddata = implode("\n", $jaddata) . "\n";

    // store in cache
    cache_set($cid, $jaddata);
  } else {
    $jaddata = $jaddata->data;
  }

  drupal_set_header('Content-Type: text/vnd.sun.j2me.app-descriptor');
  print($jaddata);
}

function jadmaker_store_download_stats() {
  $vars = array();
  foreach($_SERVER as $key => $value) {
    if(substr($key, 0, 5) === 'HTTP_') {
      $vars[$key] = $value;
    }
  }
  $entry = array(
    'timestamp' => time(),
    'http_data' => gzcompress(serialize($vars)),
  );
  drupal_write_record('jadmaker', $entry);
}