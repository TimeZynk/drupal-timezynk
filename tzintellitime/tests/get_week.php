<?php

tzintellitime_include_classes();

$date = date_make_date('now');
if($_SERVER['argc'] == 5) {
  $date = date_make_date($_SERVER['argv'][4]);
}


$intelli = array(
  'url' => "https://my2.intelliplan.se/IntelliplanWeb/v2005/Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d",
  'user' => "Johan Heander",
  'pass' => "0733623516",
);

/*
$intelli = array(
  'url' => "http://localhost/IntelliplanWeb/v2005/Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d",
  'user' => "test user",
  'pass' => "test password",
);*/

$bot = new TZIntellitimeBot($intelli['url']);
$ok = $bot->login($intelli['user'], $intelli['pass']);
if(!$ok) {
  echo "Login failed!\n";
  exit();
}
$start_time = microtime(TRUE);
$data = $bot->load_week($date);
$stop_time = microtime(TRUE);
print_r($data);

print "\n" . serialize($data['reports']) . "\n";
print "\n" . serialize($data['assignments']) . "\n";

print "\nDuration: " . ($stop_time - $start_time) . "\n";
