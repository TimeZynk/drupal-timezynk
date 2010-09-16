<?php
$query = $argv[1];
$doc = new DOMDocument();
@$doc->loadHTMLFile($argv[2]);

print("Searching " . $argv[2] . " for query '$query'\n");

$doc = simplexml_import_dom($doc);
$elements = $doc->xpath($query);
print_r($elements);
