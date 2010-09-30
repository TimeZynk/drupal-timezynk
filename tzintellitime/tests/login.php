<?php

/**
 * Script meant for fast testing of intellitime login code. It should be run with "drush scr login.php".
 * It boots drupal, inserts our intellitime module, and tries to login to intellitime using that module.
 */

require_once(dirname(__FILE__) . '/../tzintellitime.module');

$intelli_url = "http://my2.intelliplan.se/IntelliplanWeb/v2005/Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d";
$bot = new TZIntellitimeBot($intelli_url);
$ok = $bot->login("Johan Heander", "0733623516");
if($ok) {
  echo "Login successful!\n";
} else {
  echo "Login failed!\n";
}