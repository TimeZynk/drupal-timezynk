<?php
function tzbase_drush_command() {
  $items['tzbase-list-reports'] = array(
    'callback' => 'tzbase_drush_list_reports',
    'description' => 'List all reports',
  );
  $items['tzbase-edit-report'] = array(
    'callback' => 'tzbase_drush_edit_report',
    'description' => 'Edits an existing report',
    'arguments' => array(
      'report_id' => 'Report NID',
      'begintime' => 'New begintime like 08:00',
      'endtime' => 'new endtime like 16:30',
      'breakminutes' => 'Breaktime in minutes',
      'comment' => 'Optional comment',
    ),
  );
  return $items;
}

function tzbase_drush_list_reports() {
  global $user;
  $query="SELECT DISTINCT nid FROM {tzreport} WHERE flags != 255";
  $query_args = array();
  if (!empty($user->uid)) {
    $query .= " AND assignedto = %d";
    $query_args[] = $user->uid;
  }
  $query .= " ORDER BY nid ASC";
  $result = db_query($query, $query_args);
  while ($nid = db_result($result)) {
    print $nid . "\n";
  }
}

function tzbase_drush_edit_report($report_id, $begin, $end, $break=NULL, $comment=NULL) {
  if (empty($report_id)) {
    print "Cannot add report without assignment id\n";
    return;
  }
  if (empty($begin) || empty($end)) {
    print "Cannot add a report with invalid start or end times\n";
    return;
  }
  $report = node_load($report_id);
  if (empty($report)) {
    print "Failed to load report\n";
    return;
  }
  if ($report->type !== 'tzreport') {
    print "report ID did not specify a report node...\n";
    return;
  }

  if ($comment !== NULL) {
    $report->body = $comment;
  }

  // Parse start and finish
  $begin_date_time = tzbase_make_date($report->begintime);
  list($hour, $minute) = explode(':', $begin);
  $begin_date_time->setTime(intval($hour, 10), intval($minute, 10));
  $report->begintime = $begin_date_time->format('U');

  list($hour, $minute) = explode(':', $end);
  $end_date_time = clone $begin_date_time;
  $end_date_time->setTime(intval($hour, 10), intval($minute, 10));

  if ($end_date_time < $begin_date_time) {
    $end_date_time->modify('+1 day');
  }

  $report->endtime = $end_date_time->format('U');
  if ($break !== NULL) {
    $report->breakduration = $break*60;
  }
  $report->revision = TRUE;
  $report = node_submit($report);
  node_save($report);

  watchdog('tzproto', 'Created revision !vid of report !nid via drush',
  array('!vid' => $report->vid, '!nid' => $report->nid),
  WATCHDOG_NOTICE,
  l('View', 'node/' . $report->nid));
  print $report->vid . "\n";
}
