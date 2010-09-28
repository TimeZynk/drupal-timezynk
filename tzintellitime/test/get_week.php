<?php

require_once(dirname(__FILE__) . '/../tzintellitime.class.inc');

$date = time();
if($_SERVER['argc'] == 5) {
  $date = strtotime($_SERVER['argv'][4]);
}

$intelli_url = "http://my2.intelliplan.se/IntelliplanWeb/v2005/Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d";
$bot = new TZIntellitimeBot($intelli_url);
$ok = $bot->login("Johan Heander", "0733623516");
if(!$ok) {
  echo "Login failed!\n";
  exit();
}
$start_time = microtime(TRUE);
$assignments = $bot->load_assignments($date);
$stop_time = microtime(TRUE);
print_r($assignments);
echo "\n";

echo ($stop_time - $start_time) . "\n";
