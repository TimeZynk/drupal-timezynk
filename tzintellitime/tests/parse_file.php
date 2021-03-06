<?php

autoload_registry_rebuild();

function loadHTMLFile($filename) {
    $full_name = dirname(__FILE__) . "/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    return new TZIntellitimeParser($contents);
}

print_r($_SERVER['argv']);

if($_SERVER['argc'] > 4) {
  $file = basename($_SERVER['argv'][3]);
}

//if (empty($file)) {
//  $file = 'intellitime-v9-timereport-administrator-absence-not-done.txt';
//}
$parser = loadHTMLFile($file);
$reports = $parser->parse_reports();
print_r ($reports);

$assignments = $parser->parse_assignments();
$assignments += $parser->parse_absence_types();
print_r($assignments);

print("Unfinished: ");
foreach($parser->parse_unfinished_weeks() as $unfinishedDate) {
   print($unfinishedDate->format('\'o\WW\'') . ", ");
}
print("\n");
