<?php

$drupal_base_url = parse_url('http://localhost/');
$_SERVER['HTTP_HOST'] = $drupal_base_url['host'];
$_SERVER['PHP_SELF'] = $drupal_base_url['path'].'/index.php';
$_SERVER['REQUEST_URI'] = $_SERVER['SCRIPT_NAME'] = $_SERVER['PHP_SELF'];
$_SERVER['REMOTE_ADDR'] = NULL;
$_SERVER['REQUEST_METHOD'] = NULL;

error_reporting(E_ALL);
include_once './includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

tzbase_include_proto_classes();
cache_clear_all();

function createMockReport($id, $date, $begintime, $endtime, $breakminutes = 60, $localchanges = 1, $flags = TZFlags::REPORTED) {
  static $nid = 22;
  $nid++;
  tzbase_include_proto_classes();
  $begindate = new DateTime($date . 'T' . $begintime);
  $enddate = new DateTime($date . 'T' . $endtime);
  if($enddate < $begindate) {
    $enddate->add('1 day');
  }

  $mockReport = (object) array(
    'nid' => $nid,
    'uid'=> 42,
    'vid' => $nid,
    'title'=> "title",
    'body' => '',
    'jobid'=> 342,
    'assignedto' => 63,
    'begintime' => $begindate->format('U'),
    'endtime' => $enddate->format('U'),
    'breakduration'=> 60 * $breakminutes,
    'flags'=> $flags,
    'intellitime_id' => NULL,
    'intellitime_local_changes' => $localchanges,
  );

  $date = tzbase_make_date($mockReport->begintime);
  if ($id) {
    $mockReport->intellitime_id = serialize(array($date->format('Y-m-d'), (string) $mockReport->assignedto, (string) $id));
  }
  return $mockReport;
}

function createMockITReport($id, $date, $begintime, $endtime, $breakminutes = 60, $localchanges = 1) {
  $tzreport = createMockReport($id, $date, $begintime, $endtime, $breakminutes, $localchanges);
  return new TZIntellitimeReport($tzreport);
}


function toPostHash($postString) {
  $postArray = explode('&', $postString);
  $postHash = array();
  foreach ($postArray as $keyValue) {
    $entry = array_map('urldecode', explode('=', $keyValue, 2));
    $postHash[$entry[0]] = $entry[1];
  }
  return $postHash;
}


function read_all_function($path) {
  return function($filename) use ($path) {
    $full_name = "$path/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    return $contents;
  };
}
