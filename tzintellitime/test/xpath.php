<?php
$doc = new DOMDocument();
$doc->loadHTMLFile($argv[1]);
$xpath = new DOMXPath($doc);
$actionlist = $xpath->query('//a[@href="LogOut.aspx?MId=LogOut"]');
foreach($actionlist as $action) {
  print_r($action);
  print_r($action->nodeValue);
}

print_r(_tzintellitime_parse_form_hidden($xpath));

$handle = curl_init();
$post_data = array("TextBoxUserName" => "Johan Heander", 
	           "TextBoxPassword" => "0733623516");
_tzintellitime_post($handle, "http://my2.intelliplan.se/IntelliplanWeb/v2005/(zercof5533snfaux0nedmt55)/Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d", $post_data);

function _tzintellitime_parse_form_hidden($xpath) {
  $hiddenlist = $xpath->query('//input[@type="hidden"]');
  foreach($hiddenlist as $hidden) {
    $name = $hidden->attributes->getNamedItem('name');
    $value = $hidden->attributes->getNamedItem('value');
    if($name && $value) {
      $result[$name->nodeValue] = $value->nodeValue;
    }
  }
  return $result;
}


function _tzintellitime_post($handle, $url, $post_data) {
  $curl_opts = array(
    CURLOPT_RETURNTRANSFER => TRUE,
    CURLOPT_FAILONERROR => TRUE,
    CURLOPT_URL => $url,
    CURLOPT_POSTFIELDS => http_build_query($post_data),
//    CURLOPT_POSTFIELDS => http_build_query($post_data, '', '&'),
    CURLOPT_FOLLOWLOCATION => FALSE,
    CURLOPT_POST => TRUE,
  );
  curl_setopt_array($handle, $curl_opts);
  return curl_exec($handle);
}