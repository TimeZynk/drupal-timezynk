<?
// just create from the proto file a pb_prot[NAME].php file
require_once(dirname(__FILE__) . '/pb4php-read-only/parser/pb_parser.php');

$parser = new PBParser();
$parser->parse(dirname(__FILE__) . '/tzbase.proto');

print("File parsing done!\n");
?>