<?php
/**
 * @file Import functions for legacy data
 */

/**
 * Number of reports to import for every request
 */
define('TZBASE_IMPORT_CHUNK', 10);

/**
 * Batch processing function for importing assignments
 */
function tzbase_import_assignments(&$context) {
  // Initialize the sandbox the first time
  if(!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = db_result(db_query('SELECT COUNT(nid) FROM {node} WHERE type = \'assignment\''));
  }

  watchdog('tzbase', 'importing assignments %d to %d', array($context['sandbox']['progress'],
    $context['sandbox']['progress'] + TZBASE_IMPORT_CHUNK));

  // Convert all assignments
  $result = db_query_range("SELECT nid FROM {node} WHERE type = 'assignment' ORDER BY nid",
        $context['sandbox']['progress'], TZBASE_IMPORT_CHUNK);
  while($data = db_fetch_object($result)) {
    $node = node_load($data->nid);

    $context['sandbox']['progress']++;
    $context['message'] = t('Importing !type !count of !max',
      array('!type' => $node->type,
            '!count' => $context['sandbox']['progress'],
            '!max' => $context['sandbox']['max']));

    $job = _tzbase_create_tzjob_from_assignment($node);
    $context['results']['jobmap'][$node->nid][0] = $job->nid;
    $context['results']['jobs'][] = $job->nid;

    foreach($node->field_subassignment as $sub) {
      $title = $sub['value'];
      if(!empty($title)) {
        $subjob = _tzbase_create_tzjob_from_assignment($node, $title, $job->nid);
        $context['results']['jobmap'][$node->nid][$title] = $subjob->nid;
        $context['results']['jobs'][] = $job->nid;
      }
    }
  }

  if($context['sandbox']['progress'] == $context['sandbox']['max']) {
    // We're done!
    unset($context['sandbox']['progress']);
    unset($context['sandbox']['max']);

    $context['finished'] = 1;
  }
  else {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}

function tzbase_import_timereports(&$context) {
  // Initialize the sandbox the first time
  if(!isset($context['sandbox']['progress'])) {
    $context['sandbox']['progress'] = 0;
    $context['sandbox']['max'] = db_result(db_query('SELECT COUNT(nid) FROM {node} WHERE type = \'timereport\''));
  }
  // Convert all time reports
  $result = db_query_range("SELECT nid FROM {node} WHERE type = 'timereport'",
        $context['sandbox']['progress'], TZBASE_IMPORT_CHUNK);
  while($data = db_fetch_object($result)) {
    $node = node_load($data->nid);

    $context['sandbox']['progress']++;
    $context['message'] = t('Importing !type !count of !max',
      array('!type' => $node->type,
            '!count' => $context['sandbox']['progress'],
            '!max' => $context['sandbox']['max']));

    $report = _tzbase_create_tzreport_from_timereport($node, $context['results']['jobmap']);
    $context['results']['reports'][] = $report->nid;
  }

  if($context['sandbox']['progress'] == $context['sandbox']['max']) {
    $context['finished'] = 1;
  }
  else {
    $context['finished'] = $context['sandbox']['progress'] / $context['sandbox']['max'];
  }
}


function tzbase_import_finished($success, $results, $operations) {
  if($success) {
    drupal_set_message(t('Legacy import complete. Created !jobs jobs and !reports reports',
      array('!jobs' => count($results['jobs']),
            '!reports' => count($results['reports']))));
  }
  else {
    $error_op = reset($operations);
    $op = array_shift($error_op);
    $args = array_shift($error_op);
    $args = implode(', ', $args);
    watchdog('tzbase', "Error when calling operation '%s'('%s')", array($op, $args));
    drupal_set_message(t('An error occurred during import and has been recorded in the system log'), 'error');
  }
}

/**
 * Create a new tzjob node based on a legacy CCK assignment node
 * @param stdClass $node The node to copy
 * @param int $parentid Optional parent node id
 */
function _tzbase_create_tzjob_from_assignment($node, $title = '', $parentid = 0) {
  $newnode = new stdClass();

  if($title) {
    $newnode->title = $title;
  }
  else {
    $newnode->title = $node->title;
  }

  $newnode->flags = $node->field_signature[0]['value'] ? TZJOB_SIGNATURE_EVERY_POST : 0;
  $newnode->type = 'tzjob';
  
  // Copy groups of available
  if(module_exists('og')) {
    $newnode->og_groups = $node->og_groups;
    $newnode->og_public = $node->og_public;
  }

  if($parentid) {
    $newnode->parentid = $parentid;
  }

  // Prepare node
  $newnode = node_submit($newnode);

  // Set author and date
  $newnode->uid = $node->uid;
  $newnode->created = $node->created;
  node_save($newnode);
  return $newnode;
}

function _tzbase_create_tzreport_from_timereport($node, &$jobmap) {
  // Find new job node id
  if(!$node->field_assignment[0]['nid']) {
    drupal_set_message('failed to create timereport for node ' . $node->nid, 'error');
    return;
  }

  $report = new stdClass();
  $report->type = 'tzreport';
  $report->og_groups = $node->og_groups;
  $report->og_public = $node->og_public;

  $job = $jobmap[(int)$node->field_assignment[0]['nid']];
  $subassignment = $node->field_report_subassignment[0]['value'];
  if($subassignment) {
    $report->jobid = $job[$subassignment];
  }
  else {
    $report->jobid = $job[0];
  }

  // Set appropriate time zone
  $oldtz = date_default_timezone_get();
  date_default_timezone_set($node->field_time[0]['timezone']);

  // Parse start and finish
  $report->begintime = strtotime($node->field_time[0]['value']);
  $report->endtime = strtotime($node->field_time[0]['value2']);

  // Restore previous time zone
  date_default_timezone_set($oldtz);

  $report->breakduration = (int)$node->field_break[0]['approx_seconds'];
  $report->travelduration = (int)$node->field_traveltime[0]['approx_seconds'];
  $report->travelkm = (int)$node->field_traveldistance[0]['value'];
  $report->signature = $node->field_signed[0]['value'];
  $report->flags = 0;

  // Prepare node for saving
  $report = node_submit($report);

  // Set author and date
  $report->uid = $node->uid;
  $report->created = $node->created;
  node_save($report);

  return $report;
}