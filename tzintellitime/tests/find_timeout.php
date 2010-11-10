<?php

require_once(dirname(__FILE__) . '/../tzintellitime.class.inc');

$date = date_make_date('now');
if($_SERVER['argc'] == 5) {
  $date = date_make_date($_SERVER['argv'][4]);
}

$intelli_url = "http://my2.intelliplan.se/IntelliplanWeb/v2005/Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d";

// Start with 5 minutes delay, don't go beyond one hour
$highest_success = 0;
$lowest_failure = 3600;

$attempts = 0;

// Precision 30 seconds
while($lowest_failure - $highest_success > 30) {
  $next_delay = round(($highest_success + $lowest_failure)/2);

  print("Starting attempt for $next_delay seconds\n");
  $bot = new TZIntellitimeBot($intelli_url);
  $ok = $bot->login("Johan Heander", "0733623516");
  if(!$ok) {
    echo "Login failed!\n";
    exit();
  }

  print("  Login OK, starting sleep ... ");
  sleep($next_delay);
  $assignments = $bot->load_week($date);
  if($assignments) {
    print("session still alive, raising bar!\n");
    $highest_success = $next_delay;
  } else {
    print("session died, lowering bar!\n");
    $lowest_failure = $next_delay;
  }
  $attempts++;
}

print("Session lifetime = $highest_success seconds, found after $attempts attempts\n");
