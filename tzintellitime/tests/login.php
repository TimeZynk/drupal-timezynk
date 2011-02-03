<?php

/**
 * Script meant for fast testing of intellitime login code. It should be run with "drush scr login.php".
 * It boots drupal, inserts our intellitime module, and tries to login to intellitime using that module.
 */


//$intelli_url = "http://my2.intelliplan.se/IntelliplanWeb/v2005/Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d";
$intelli_url = "http://localhost/IntelliplanWeb/Portal/Login.aspx";
$bot = new TZIntellitimeBot($intelli_url);
$ok = $bot->login("test user", "test password");
if($ok) {
  echo "Login successful!\n";
} else {
  echo "Login failed!\n";
}