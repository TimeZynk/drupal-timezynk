<?php

/**
 * Generate a JAD file
 */
function jadmaker_create_jad() {
  $ua = $_SERVER['HTTP_USER_AGENT'];

  jadmaker_store_download_stats();

  // Redirect iPhone and iPad
  if(false !== strpos($ua, 'iPhone') ||
     false !== strpos($ua, 'iPad')) {
    drupal_goto('mobile');
  }

  $lang = jadmaker_detect_language();

  $file = jadmaker_find_file($lang);

  if (!file_exists($file->path)) {
    watchdog('jadmaker', 'File not found: "@path"', array('@path' => $file->path), WATCHDOG_ALERT);
    return drupal_not_found();
  }

  if (preg_match('/\.apk$/', $file->name)) {
    // Matched APK file, just transfer it raw
    file_transfer($file->path, array(
      'Content-Type: application/vnd.android.package-archive',
      'Content-Disposition: attachment; filename="' . $file->name . '";',
      'Content-Length: ' . filesize($file->path),
    ));
  } else {
    // Time to serve up JAD manifest for JAR-file
    $manifest = jadmaker_create_manifest($file->name, $file->path);
    drupal_set_header('Content-Type: text/vnd.sun.j2me.app-descriptor');
    drupal_set_header('Content-Length: ' . strlen($manifest));
    print($manifest);
    exit();
  }
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

function jadmaker_detect_language() {
  static $available_languages = array('sv', 'en');

  $choosen_language = NULL;

  if (!empty($_SERVER['HTTP_ACCEPT_LANGUAGE'])) {
    // Accept-Language found, match with available languages
    $accepted = explode(',', strtolower($_SERVER['HTTP_ACCEPT_LANGUAGE']));
    foreach ($accepted as $lang) {
      $lang = trim($lang);
      foreach ($available_languages as $available) {
        if (strpos($lang, $available) === 0) {
          $choosen_language = $available;
          break 2;
        }
      }
    }
  }

  if (!$choosen_language && !empty($_SERVER['HTTP_USER_AGENT'])) {
    // No language decided yet, check if User-Agent has a match instead
    foreach ($available_languages as $lang) {
      if (preg_match("/;\s*$lang/", $_SERVER['HTTP_USER_AGENT'])) {
        $choosen_language = $lang;
      }
    }
  }

  if (!$choosen_language) {
    // Still no match, use drupal language or fall back to first available
    global $language;
    $choosen_language = $available_languages[0];
    foreach ($available_languages as $lang) {
      if ($lang == $language->language) {
        $choosen_language = $lang;
      }
    }
  }

  return $choosen_language;
}

function jadmaker_find_file($lang) {
  $ua = $_SERVER['HTTP_USER_AGENT'];

  $file = new stdClass();
  $file->name = "tz-Generic-AnyPhone-$lang.jar";

  if(FALSE !== strpos($ua, 'Android')) {
    $file->name = "tz-Generic-android-$lang.apk";
  } else if (FALSE !== strpos($ua, 'SEMC') ||
     FALSE !== strpos($ua, 'SonyEricsson')) {
    $file->name = "tz-Sony-Ericsson-JavaPlatform8-$lang.jar";
  }

  $file->path = file_create_path($file->name);
  return $file;
}

function jadmaker_create_manifest($jarfile, $jarpath) {
  $cid = 'jadmaker:' . $jarfile;
  $jaddata = cache_get($cid);

  // check if cache is older than file
  $jarstat = stat($jarpath);
  if(!$jaddata || ($jaddata->created < $jarstat['mtime'])) {
    // Cached data did not exist or is older than jar file

    // Read manifest from JAR
    $jzip = zip_open($jarpath);
    if(!is_resource($jzip)) {
      watchdog('jadmaker', 'Error @error when opening "@path"',
          array('@path' => $jarpath, '@error' => $jzip), WATCHDOG_ALERT);
      drupal_not_found();
      exit();
    }

    while($entry = zip_read($jzip)) {
      if('META-INF/MANIFEST.MF' == zip_entry_name($entry)) {
        $manifest = zip_entry_read($entry, zip_entry_filesize($entry));
        break;
      }
    }
    zip_close($jzip);

    if(!$manifest) {
      watchdog('jadmaker', 'Error when reading manifest from "@path"',
          array('@path' => $jarpath), WATCHDOG_ALERT);
      drupal_not_found();
      exit();
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
  return $jaddata;
}