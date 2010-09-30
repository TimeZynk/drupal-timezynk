<?php

require_once(dirname(__FILE__) . '/../tzintellitime.module');

$intelli_url = "http://my2.intelliplan.se/IntelliplanWeb/v2005/Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d";
$bot = new TZIntellitimeBot($intelli_url);
$ok = $bot->login("Johan Heander", "0733623516");
if(!$ok) {
  echo "Login failed!\n";
  exit();
}

$result = $bot->load_week();
print_r($result);
$reports = $result['reports'];

// Change report[3] and update it again
$reports[1]->begin = "10:16";
$reports[1]->end = "16:55";
$reports[1]->done = TRUE;

print_r($bot->update_report($reports[1]));
