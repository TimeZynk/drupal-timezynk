<?php
function loadHTMLFile($filename) {
    autoload_registry_update();
    $full_name = dirname(__FILE__) . "/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    return new TZIntellitimeParser($contents);
}

if($_SERVER['argc'] > 4) {
  $file = basename($_SERVER['argv'][3]);
}

//if (empty($file)) {
//  $file = 'intellitime-v9-timereport-administrator-absence-not-done.txt';
//}
$parser = loadHTMLFile($file);
$reports = $parser->parse_reports();
foreach($reports as $report) {
  $datestring = $report->get_date_string();
  print("createMockReport('$report->id', '$datestring', '$report->begin', '$report->end', $report->break_duration_minutes),\n");
}
