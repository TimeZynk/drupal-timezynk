<?php
/**
 * @file
 * Request handler for the TimeZynk protocol
 */


/**
 * Request dispatcher
 */
function tzproto_request() {
  tzbase_include_proto_classes();

  // Read POST data
  $raw_data = file_get_contents('php://input');
  if(!$raw_data) {
    tzproto_error(t('No input data'), 405);
    die();
  }

  // Check if the message is base64 encoded
  $use_base64 = TRUE;
  $data = base64_decode($raw_data, TRUE);
  if($data === false) {
    $data = $raw_data;
    $use_base64 = FALSE;
  }

  $req = new TZRequest();
  $req->parseFromString($data);

  $response = new TZResponse();

  // Store timestamp
  $response->set_timestamp(time());

  // Authenticate user
  if(0 == tzproto_login($req)) {
    for($i = 0; $i < $req->command_size(); $i++) {
      $cmd = $req->command($i);
      watchdog('tzproto', 'received command<pre>' . print_r($cmd, TRUE) . '</pre>');

      // Return client request ID
      $result = $response->add_result();
      $result->set_client_handle($cmd->client_handle());

      if($cmd->get_user_cmd()) {
        tzproto_handle_get_user($cmd, $result);
      }
      else if($cmd->sync_cmd()) {
        tzproto_handle_sync_cmd($cmd, $result);
      }
      else if($cmd->get_job_cmd()) {
        tzproto_handle_get_job_cmd($cmd, $result);
      }
      else if($cmd->get_report_cmd()) {
        tzproto_handle_get_report_cmd($cmd, $result);
      }
      else if($cmd->create_job_cmd()) {
      	tzproto_handle_create_job_cmd($cmd, $result);
      }
      else if($cmd->create_report_cmd()) {
        tzproto_handle_create_report_cmd($cmd, $result);
      }
      else {
        $result->set_error_code(501);
        $result->set_error_msg('Not implemented');
      }
      watchdog('tzproto', 'sending result<pre>' . print_r($result, TRUE) . '</pre>');
    }
  }

  $response_data = $response->SerializeToString();
  if($use_base64) {
    $response_data = base64_encode($response_data);
  }

  print($response_data);

  tzproto_logout();

  // All handling finished, prevent output from other modules
  module_invoke_all('exit');
  exit;
}

/**
 * Validate login credentials and authenticate user
 * @param $req TZCommand
 * @param $response TZResponse to send back
 */
function tzproto_login($req) {
  global $user;

  if ($user->uid) {
    // user is already logged in
    tzproto_logout();
  }

  /* Login and run login hooks */
  $form_state = array();
  $form_state['values']['name'] = $req->username();
  $form_state['values']['pass'] = $req->password();
  $form_state['values']['op'] = t('Log in');
  drupal_execute('user_login', $form_state);

  if ($user->uid) {
    watchdog('tzproto', 'Login for !name', array('!name' => $user->name));
    return 0;
  }

  return tzproto_error(t('Wrong username or password.'), 401);
}

/**
 * Destroy session and log out
 */
function tzproto_logout() {
  /* Logout and destroy user object */
  global $user;

  watchdog('tzproto', 'Session closed for %name.', array('%name' => $user->name));

  session_destroy();
  // Only variables can be passed by reference workaround.
  $null = NULL;
  user_module_invoke('logout', $null, $user);
  $user = drupal_anonymous_user();
}

/**
 * Set the error condition
 * @param $message HTTP status message
 * @param $status HTTP error code
 */
function tzproto_error($message, $status) {
  watchdog('tzproto', 'Error !code: @error', array('!code' => $status, '@error' => $message), WATCHDOG_WARNING);
  drupal_set_header('HTTP/1.1 ' . (int)$status . ' ' . check_plain($message));
  return 1;
}

/**
 * Handle the get_user command
 */
function tzproto_handle_get_user(&$cmd, &$result) {
  global $user;
  $tzuser = new TZUser();

  // Load basic data
  $tzuser->set_id($user->uid);
  $tzuser->set_username($user->name);
  if(module_exists('realname')) {
    $tzuser->set_realname($user->realname);
  }

  $result->set_user($tzuser);
}

/**
 * Handle the sync command
 */
function tzproto_handle_sync_cmd(&$cmd, &$result) {
  global $user;

  $sync_cmd = $cmd->sync_cmd();

  $last_sync = 0;
  if($sync_cmd->last_sync()) {
    $last_sync = $sync_cmd->last_sync();
  }

  for($i = 0; $i < $sync_cmd->new_job_size(); $i++) {
      $job = tzproto_create_tzjob_from_protobuf($sync_cmd->new_job($i));
  }

  // Store new reports
  for($i = 0; $i < $sync_cmd->new_report_size(); $i++) {
      $report = tzproto_create_tzreport_from_protobuf($sync_cmd->new_report($i));
  }

  // Load jobs
  $sql = "SELECT n.nid FROM {node} n WHERE type = 'tzjob' AND status = 1 AND changed > %d";
  $db_result = db_query(db_rewrite_sql($sql), $last_sync);
  while($data = db_fetch_object($db_result)) {
    $node = node_load($data->nid);
    $tzjob = $result->add_job();
    tzproto_fill_from_node($tzjob, $node);
  }

  // Load reports
  $sql = "SELECT n.nid FROM {node} n INNER JOIN {tzreport} tz ON n.nid = tz.nid WHERE type = 'tzreport' AND status = 1 AND changed > %d AND tz.assignedto = %d";
  $db_result = db_query(db_rewrite_sql($sql), $last_sync, $user->uid);
  while($data = db_fetch_object($db_result)) {
    $node = node_load($data->nid);
    $tzreport = $result->add_report();
    tzproto_fill_from_node($tzreport, $node);
  }
}



/**
 * Handle the get job command
 */
function tzproto_handle_get_job_cmd(&$cmd, &$result) {
  $jobcmd = $cmd->get_job_cmd();
  $jobresult = new TZGetJobResult();

  // Are we looking for a single id?
  if($jobcmd->job_id()) {
    $node = node_load($jobcmd->job_id());
    if($node && $node->nid) {
      $tzjob = $jobresult->add_job();
      tzproto_fill_from_node($tzjob, $node);
    }
  }
  // Fetch a collections of jobs
  else {
    // Filtering on changed time?
    $changed = 0;
    if($jobcmd->changed_after()) {
      $changed = $jobcmd->changed_after();
    }

    $sql = "SELECT n.nid FROM {node} n WHERE type = 'tzjob' AND status = 1 AND changed > %d ORDER BY title";
    $db_result = FALSE;
    if($jobcmd->limit()) {
      $db_result = db_query_range(db_rewrite_sql($sql), $jobcmd->offset(), $jobcmd->limit(), $changed);
    } else {
      $db_result = db_query(db_rewrite_sql($sql), $changed);
    }
    while($data = db_fetch_object($db_result)) {
      $node = node_load($data->nid);
      $tzjob = $jobresult->add_job();
      tzproto_fill_from_node($tzjob, $node);
    }
  }
  $result->set_get_job_result($jobresult);
}

/**
 * Handle get report command.
 * Fetches tzreport objects from the database and transfers them over protobuf
 */
function tzproto_handle_get_report_cmd(&$cmd, &$result) {
  global $user;

  $reportcmd = $cmd->get_report_cmd();
  $reportresult = new TZGetReportResult();

  // Are we looking for a single id?
  if($reportcmd->report_id()) {
    $node = node_load($reportcmd->report_id());
    if($node && $node->nid) {
      $tzreport = $reportresult->add_report();
      tzproto_fill_from_node($tzreport, $node);
    }
  }
  // Fetch a collections of jobs
  else {
    // Filtering on changed time?
    $changed = 0;
    if($reportcmd->changed_after()) {
      $changed = $reportcmd->changed_after();
    }

    $sql = "SELECT n.nid FROM {node} n INNER JOIN {tzreport} tz ON n.vid = tz.vid WHERE n.type = 'tzreport' AND n.status = 1 AND n.changed > %d AND tz.assignedto = %d ORDER BY tz.begintime DESC";
    $db_result = FALSE;

    // Have the user requested a limited query?
    if($reportcmd->limit()) {
       // Fetch number of available nodes
      $count_query = db_rewrite_sql("SELECT COUNT(DISTINCT n.nid) FROM {node} n INNER JOIN {tzreport} tz ON n.vid = tz.vid WHERE n.type = 'tzreport' AND n.status = 1 AND n.changed > %d AND tz.assignedto = %d");
      $count = (int)db_result(db_query($count_query, $changed, $user->uid));

      $db_result = db_query_range(db_rewrite_sql($sql), $changed, $user->uid, $reportcmd->offset(), $reportcmd->limit());
      $reportresult->set_offset($reportcmd->offset());
      $reportresult->set_total_report_count($count);
    } else {
      $db_result = db_query(db_rewrite_sql($sql), $changed);
    }

    while($data = db_fetch_object($db_result)) {
      $node = node_load($data->nid);
      $tzreport = $reportresult->add_report();
      tzproto_fill_from_node($tzreport, $node);
    }
  }
  $result->set_get_report_result($reportresult);
}

/**
 * Handle create job command.
 * Receives a new tzjob over protobuf and saves it to the database.
 */
function tzproto_handle_create_job_cmd(&$cmd, &$result) {
  $jobcmd = $cmd->create_job_cmd();
  $jobresult = new TZCreateJobResult();

  // Store new job
  if($jobcmd->new_job()) {
      $job = tzproto_create_tzjob_from_protobuf($jobcmd->new_job());
      $jobresult->set_id($job->nid);
  }
  $result->set_create_job_result($jobresult);
}

/**
 * Handle create report command.
 * Receives a new tzreport over protobuf and saves it to the database.
 */
function tzproto_handle_create_report_cmd(&$cmd, &$result) {
  $reportcmd = $cmd->create_report_cmd();
  $reportresult = new TZCreateReportResult();

  // Store new reports
  if($reportcmd->new_report()) {
      $report = tzproto_create_tzreport_from_protobuf($reportcmd->new_report());
      $reportresult->set_id($report->nid);
  }
  $result->set_create_report_result($reportresult);
}

/**
 * Fill a protobuf object from a node object
 * @param $proto protobuf object to fill
 * @param $node node data
 */
function tzproto_fill_from_node(&$proto, $node) {
  $proto->set_id($node->nid);
  $proto->set_flags($node->flags);
  $proto->set_title($node->title);
  $proto->set_description($node->body);
  $proto->set_changed($node->changed);

  if($node->type == 'tzjob') {
    $proto->set_parent_id($node->parentid);
    $proto->set_job_code($node->jobcode);
  } else if($node->type == 'tzreport') {
    $proto->set_vid($node->vid);
    $proto->set_job_id($node->jobid);
    $proto->set_begin_time($node->begintime);
    $proto->set_end_time($node->endtime);
    $proto->set_break_duration($node->breakduration);
    $proto->set_signature($node->signature);
    $proto->set_travel_duration($node->travelduration);
    $proto->set_travel_km($node->travelkm);
  }
}

function tzproto_create_tzreport_from_protobuf($data) {
  global $user;

  // Find new job node id
  if(!$data->job_id()) {
    watchdog('tzproto', 'No job set for report', array(), WATCHDOG_WARNING);
    return;
  }

  $report = new stdClass();

  // Load node if this is an update
  if($data->id()) {
    $report = node_load($data->id());
  } else {
    $report->type = 'tzreport';
    if(module_exists('og')) {
      $report->og_groups = $user->og_groups;
      $report->og_public = FALSE;
    }
    $report->created = $data->changed();
  }

  $report->jobid = $data->job_id();

  // Set title if available
  if($data->title()) {
    $report->title = $data->title();
  } else {
    // Get title from job
    $title = db_result(db_query("SELECT title FROM {node} WHERE nid = %d", $report->jobid));
    $report->title = $title ? $title : 'Time Report from ' . $user->name;
  }

  $report->body = $data->description();

  // Parse start and finish
  $report->begintime = $data->begin_time();
  $report->endtime = $data->end_time();
  $report->breakduration = $data->break_duration();

  $report->travelduration = 0;
  $report->travelkm = 0;
  $report->signature = "";

  $report->flags = $data->flags();

  // Prepare node for saving
  $report = node_submit($report);

  // Set author and date
  $report->uid = $user->uid;
  $report->assignedto = $user->uid;
  $report->changed = $data->changed();
  node_save($report);

  if($data->id()) {
    watchdog('tzproto', 'Updated report !nid', array('!nid' => $report->nid));
    /* If the node has been updated and is immediately
     * reloaded after update, it will
     *  load the OLD cached version. */
    node_load(0, NULL, TRUE);
  } else {
    watchdog('tzproto', 'Inserted report !nid', array('!nid' => $report->nid));
  }

  return $report;
}

function tzproto_create_tzjob_from_protobuf($data) {
  global $user;

  $job = new stdClass();


  // Load node if this is an update
  if($data->id()) {
    $job = node_load($data->id());
  } else {
    $job->type = 'tzjob';
    if(module_exists('og')) {
      $job->og_groups = $user->og_groups;
      $job->og_public = FALSE;
    }
    $job->created = $data->changed();
  }

  $job->parentid = $data->parent_id();
  $job->title = $data->title();
  $job->body = $data->description();
  $job->jobcode = '';
  $job->flags = 0;

  // Prepare node for saving
  $job = node_submit($job);

  // Set author and date
  $job->uid = $user->uid;
  $job->changed = $data->changed();
  node_save($job);

  if($data->id()) {
    watchdog('tzproto', 'Updated job !nid', array('!nid' => $job->nid));
    /* If the node has been updated and is immediately
     * reloaded after update, it will
     *  load the OLD cached version. */
    node_load(0, NULL, TRUE);
  } else {
    watchdog('tzproto', 'Inserted job !nid', array('!nid' => $job->nid));
  }

  return $job;
}