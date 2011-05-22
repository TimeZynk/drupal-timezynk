<?php
/**
 * Run with drush scr
 */

// just create from the proto file a pb_prot[NAME].php file
$pb4php_path = libraries_get_path('pb4php') . '/parser/pb_parser.php';
print("pb4php path: $pb4php_path\n");
if(!file_exists($pb4php_path)) {
  die('Could not find pb4php');
}
require_once($pb4php_path);

$tzbase_path = drupal_get_path('module', 'tzbase');
$pbfilename = $tzbase_path . '/pb_proto_tzbase.php';
print("tzbase path: $tzbase_path\n");
$parser = new PBParser();
$parser->parse($tzbase_path . '/tzbase.proto', $pbfilename);

print("Output written to: $pbfilename\n");
?>