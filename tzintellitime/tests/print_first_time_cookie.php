<?php

$intelli_url = "https://my2.intelliplan.se/IntelliplanWeb/v2005/Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d";
$bot = new TZIntellitimeBot($intelli_url);
$ok = $bot->login("Johan Heander", "0733623516");
if(!$ok) {
  echo "Login failed!\n";
  exit();
}
$session_data = $bot->get_session_data();
print_r($session_data);
unset($bot);
readfile($session_data['cookiejar']);