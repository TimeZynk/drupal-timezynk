<?php

$date = date_make_date('now');
if($_SERVER['argc'] == 5) {
  $date = date_make_date($_SERVER['argv'][4]);
}


$intelli = array(
  'url' => "https://my2.intelliplan.eu/IntelliplanWeb/v2005/Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d",
  'user' => "Johan Heander",
  'pass' => "0733623516",
);

/*
$intelli = array(
  'url' => "http://localhost/IntelliplanWeb/v2005/Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d",
  'user' => "test user",
  'pass' => "test password",
);*/
$start_time = microtime(TRUE);
$bot = new TZIntellitimeBot($intelli['url']);
if($bot->login($intelli['user'], $intelli['pass'])) {
  $data = $bot->load_week($date);
  print_r($data);
} else {
  print 'Login failed!';
}
$stop_time = microtime(TRUE);
print "\nDuration: " . ($stop_time - $start_time) . "\n";
