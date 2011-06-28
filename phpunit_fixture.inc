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

$files = glob(dirname(__FILE__) . '/*/includes/*.class.inc');
foreach($files as $file) {
  require_once($file);
}
tzbase_include_proto_classes();
cache_clear_all();

function createMockReport($id, $date, $begintime, $endtime, $breakminutes = 60, $localchanges = 1) {
  tzbase_include_proto_classes();
  $begindate = new DateTime($date . 'T' . $begintime);
  $enddate = new DateTime($date . 'T' . $endtime);
  if($enddate < $begindate) {
    $enddate->add('1 day');
  }

  $mockReport = (object) array(
    'nid' => 23,
    'uid'=> 42,
    'vid' => 54,
    'title'=> "title",
    'body' => '',
    'jobid'=> 342,
    'assignedto' => 63,
    'begintime' => $begindate->format('U'),
    'endtime' => $enddate->format('U'),
    'breakduration'=> 60 * $breakminutes,
    'flags'=> TZFlags::REPORTED, // reported
    'intellitime_id' => 0,
    'intellitime_local_changes' => $localchanges,
  );

  $date = tzbase_make_date($mockReport->begintime);
  $mockReport->intellitime_id = serialize(array($date->format('Y-m-d'), (string) $mockReport->assignedto, (string) $id));
  return $mockReport;
}
