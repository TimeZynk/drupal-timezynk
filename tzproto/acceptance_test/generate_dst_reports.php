<?php
require_once(dirname(__FILE__) . '/../tzproto.request.inc');
require_once(dirname(__FILE__) . '/../../tzreports/tzreports.invoice.inc');
module_load_include('inc', 'node', 'node.pages');

tzbase_include_proto_classes();

function get_first_job_for_parent($parentid) {
  $sql = 'SELECT * FROM {tzjob} t INNER JOIN {node} n on t.vid = n.vid WHERE t.parentid = %d ORDER BY n.title';
  return db_fetch_object(db_query(db_rewrite_sql($sql), $parentid));
}

$job = get_first_job_for_parent(0);
$args = func_get_args();
if (count($args) < 2) {
  die("Must supply user phone number as argument to script.\n"
    . "E.g. drush scr $args[0] 0733623516\n");
}
$username = $args[1];
$account = user_load(array('name' => $username));
if (empty($account)) {
  die("Must supply a valid user name\n");
}
function post_new_report($year, $month, $day, $begintime, $endtime, $breakduration, $username, $job) {
  $form_state = array();
  $form_state['values']['jobid'] = 'nid:0:' . $job->nid;
  $form_state['values']['title'] = 'DST Acceptance Test';
  $form_state['values']['assignedto_name'] = $username;
  $form_state['values']['flags'] = TZFlags::CREATED;
  $form_state['values']['time']['begintime'] = $begintime;
  $form_state['values']['time']['endtime'] = $endtime;
  $form_state['values']['time']['breakduration'] = $breakduration;
  $form_state['values']['workdate']['year'] = $year;
  $form_state['values']['workdate']['month'] = $month;
  $form_state['values']['workdate']['day'] = $day;
  $form_state['values']['op'] = t('Save');
  $form_state['values']['name'] = $username;
  $node = (object) array('type' => 'tzreport');
  drupal_execute('tzreport_node_form', $form_state, $node);
}

post_new_report(2010, 03, 27, "08:00", "17:00", "00:30", $username, $job);
post_new_report(2010, 03, 28, "00:00", "18:00", "00:30", $username, $job);
post_new_report(2010, 03, 28, "08:00", "17:00", "00:30", $username, $job);
post_new_report(2010, 10, 30, "08:00", "17:00", "00:30", $username, $job);
post_new_report(2010, 10, 31, "00:00", "08:00", "00:30", $username, $job);
post_new_report(2010, 10, 31, "08:00", "17:00", "00:30", $username, $job);
