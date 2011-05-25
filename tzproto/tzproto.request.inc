<?php
/**
 * @file
 * Request handler for the TimeZynk protocol
 */


/**
 * Request dispatcher
 */
function tzproto_request() {
  /* PHP's session handler normally adds a pretty large session cookie to
   * the HTTP headers. We here disable this cookie to reduce the
   * data transfer as much as possible. */
  ini_set('session.use_cookies', '0');
  tzbase_include_proto_classes();

  // Fetch the RPC protocol version
  $protocol_version = arg(1);
  if (empty($protocol_version)) {
    $protocol_version = 0;
  }

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
    for($i = 0; $i < $req->commands_size(); $i++) {
      $cmd = $req->command($i);

      // Return client request ID
      $result = $response->add_result();
      $result->set_client_handle($cmd->client_handle());

      if($cmd->get_user_cmd()) {
        tzproto_handle_get_user($cmd, $result);
      }
      else if($cmd->sync_cmd()) {
        tzproto_handle_sync_cmd($cmd, $result, $protocol_version);
      }
      else if($cmd->get_job_cmd()) {
        tzproto_handle_get_job_cmd($cmd, $result);
      }
      else if($cmd->get_report_cmd()) {
        tzproto_handle_get_report_cmd($cmd, $result, $protocol_version);
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
  $tzuser->set_realname(theme('username', $user, FALSE));
  $tzuser->set_may_create_job(user_access('create tzjob content', $user));
  $tzuser->set_may_create_reports(user_access('create tzreport content', $user));
  $result->set_user($tzuser);
}

/**
 * Handle the sync command
 */
function tzproto_handle_sync_cmd(&$cmd, &$result, $protocol_version = 0) {
  global $user;

  $sync_cmd = $cmd->sync_cmd();

  $last_sync = 0;
  if($sync_cmd->last_sync()) {
    $last_sync = $sync_cmd->last_sync();
  }

  for($i = 0; $i < $sync_cmd->new_jobs_size(); $i++) {
      $job = tzproto_create_tzjob_from_protobuf($sync_cmd->new_job($i), $result);
      if($result->error_code()) {
        // Break if we hit an error
        return;
      }
  }

  // Store new reports
  for($i = 0; $i < $sync_cmd->new_reports_size(); $i++) {
      $report = tzproto_create_tzreport_from_protobuf($sync_cmd->new_report($i), $result);
      if($result->error_code()) {
        // Break if we hit an error
        return;
      }
  }

  // Load jobs
  $sql = "SELECT n.nid FROM {node} n WHERE type = 'tzjob' AND status = 1 AND changed > %d";
  $db_result = db_query(db_rewrite_sql($sql), $last_sync);
  while($data = db_fetch_object($db_result)) {
    $node = node_load($data->nid, NULL, TRUE);
    $tzjob = $result->add_job();
    tzproto_fill_from_node($tzjob, $node);
  }

  // Load reports
  $sql = "SELECT n.nid FROM {node} n INNER JOIN {tzreport} tz ON n.vid = tz.vid WHERE type = 'tzreport' AND status = 1 AND changed > %d AND tz.assignedto = %d";
  $db_result = db_query(db_rewrite_sql($sql), $last_sync, $user->uid);
  while($data = db_fetch_object($db_result)) {
    $node = node_load($data->nid, NULL, TRUE);
    $tzreport = $result->add_report();
    tzproto_fill_from_node($tzreport, $node, $protocol_version);
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
    if ($jobcmd->changed_after()) {
      $changed = $jobcmd->changed_after();
    }

    $sql = "SELECT n.nid FROM {node} n WHERE type = 'tzjob' AND status = 1 AND changed > %d ORDER BY title";
    $db_result = FALSE;
    if ($jobcmd->limit()) {
      $db_result = db_query_range(db_rewrite_sql($sql), $jobcmd->offset(), $jobcmd->limit(), $changed);
    } else {
      $db_result = db_query(db_rewrite_sql($sql), $changed);
    }

    $tzjobs = array();
    while ($data = db_fetch_object($db_result)) {
      $node = node_load($data->nid, NULL, TRUE);
      $tzjob = new TZJob();
      tzproto_fill_from_node($tzjob, $node);

      if ($jobcmd->may_create_report() === NULL ||
          (boolean)$jobcmd->may_create_report() === (boolean)$tzjob->may_create_report()) {
        $tzjobs[] = $tzjob;
      }
    }
    $jobresult->set_all_jobs($tzjobs);
  }

  $result->set_get_job_result($jobresult);
}

/**
 * Handle get report command.
 * Fetches tzreport objects from the database and transfers them over protobuf
 */
function tzproto_handle_get_report_cmd(&$cmd, &$result, $protocol_version = 0) {
  global $user;

  $reportcmd = $cmd->get_report_cmd();
  $reportresult = new TZGetReportResult();

  // Are we looking for a single id?
  if($reportcmd->report_id()) {
    $node = node_load($reportcmd->report_id());
    if($node && $node->nid) {
      $tzreport = $reportresult->add_report();
      tzproto_fill_from_node($tzreport, $node, $protocol_version);
    }
  }
  // Fetch a collections of reports
  else {
    $filter_strings = array();
    $filter_args = array();

    $filter_strings[] = 'n.status = %d';
    $filter_args[] = 1;

    // Always check current assigned user
    $filter_strings[] = 'tz.assignedto = %d';
    $filter_args[] = $user->uid;

    // Filtering on changed time?
    if($reportcmd->changed_after()) {
      $filter_strings[] = 'n.changed > %d';
      $filter_args[] = $reportcmd->changed_after();
    }

    // Filtering on max flags
    if($reportcmd->max_flag() !== NULL) {
      $filter_strings[] = 'tz.flags <= %d';
      $filter_args[] = $reportcmd->max_flag();
    }
    $date_range = $reportcmd->date_range();
    if($date_range) {
      $start_date = $date_range->start();
      $time = new TZTime();
      $time->set_hour(0);
      $time->set_minute(0);
      if ($start_date) {
        // Start date, create a timestamp for this date, with time set to 00:00.
        $datetime = tzproto_create_datetime($start_date, $time);
        $filter_strings[] = 'tz.begintime >= %d';
        $filter_args[] = $datetime->format('U');
      }
      $end_date = $date_range->end();
      if (!empty($end_date)) {
        // End date, create a timestamp for the day after, with time set to 00:00.
        $datetime = tzproto_create_datetime($end_date, $time);
        $datetime->modify("+1 day");

        $filter_strings[] = 'tz.begintime < %d';
        $filter_args[] = $datetime->format('U');
      }
    } else {
      if ($reportcmd->after()) {
        $filter_strings[] = 'tz.begintime >= %d';
        $filter_args[] = $reportcmd->after();
      }

      if ($reportcmd->before()) {
        $filter_strings[] = 'tz.begintime < %d';
        $filter_args[] = $reportcmd->before();
      }
    }
    $filter_string = implode(' AND ', $filter_strings);

    $sql = "SELECT n.nid FROM {node} n INNER JOIN {tzreport} tz ON n.vid = tz.vid WHERE n.type = 'tzreport' AND $filter_string ORDER BY tz.begintime DESC";

    $db_result = FALSE;

    // Have the user requested a limited query?
    if($reportcmd->limit()) {
      // Fetch number of available nodes
      $count_query = db_rewrite_sql("SELECT COUNT(DISTINCT n.nid) FROM {node} n INNER JOIN {tzreport} tz ON n.vid = tz.vid WHERE n.type = 'tzreport' AND $filter_string");
      $count = (int)db_result(db_query($count_query, $filter_args));

      $db_result = db_query_range(db_rewrite_sql($sql), $filter_args, $reportcmd->offset(), $reportcmd->limit());
      $reportresult->set_offset($reportcmd->offset());
      $reportresult->set_total_report_count($count);
    } else {
      $db_result = db_query(db_rewrite_sql($sql), $filter_args);
    }

    while($data = db_fetch_object($db_result)) {
      $node = node_load($data->nid, NULL, TRUE);
      $tzreport = $reportresult->add_report();
      tzproto_fill_from_node($tzreport, $node, $protocol_version);
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
      $job = tzproto_create_tzjob_from_protobuf($jobcmd->new_job(), $result);
      if($result->error_code()) {
        return;
      }
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
      $report = tzproto_create_tzreport_from_protobuf($reportcmd->new_report(), $result);
      if($result->error_code()) {
        return;
      }
      $reportresult->set_id($report->nid);
  }
  $result->set_create_report_result($reportresult);
}

/**
 * Fill a protobuf object from a node object
 * @param $proto protobuf object to fill
 * @param $node node data
 */
function tzproto_fill_from_node(&$proto, $node, $protocol_version = 0) {
  $proto->set_id($node->nid);
  $proto->set_flags($node->flags);
  $proto->set_title($node->title);
  $proto->set_description($node->body);
  $proto->set_changed($node->changed);

  if($node->type == 'tzjob') {
    $proto->set_parent_id($node->parentid);
    $proto->set_job_code($node->jobcode);
    /*TODO: these just check the node type, we really want
     * special access rules to check if _this_ node and user combo
     * allows creating children/reports et c. ... */
    $proto->set_may_create_child(node_access('create', $node->type));
    $proto->set_may_create_report(node_access('create', 'tzreport') &&
        $node->flags == TZJobFlags::ACTIVE);
    $proto->set_may_edit(node_access('update', $node));
  } else if($node->type == 'tzreport') {
    $proto->set_vid($node->vid);
    $proto->set_job_id($node->jobid);
    // We changed from epoch timestamps to date+time in version 1.
    if ($protocol_version > 0) {
      $time_span = tzproto_create_tztimespan($node->begintime, $node->endtime);
      if (!empty($time_span)) {
        $proto->set_worked_time($time_span);
      }
    } else {
      $proto->set_begin_time($node->begintime);
      $proto->set_end_time($node->endtime);
    }
    $proto->set_break_duration($node->breakduration);
    $proto->set_signature($node->signature);
    $proto->set_travel_duration($node->travelduration);
    $proto->set_travel_km($node->travelkm);

    $proto->set_may_edit(node_access('update', $node));
    $proto->set_may_remove(user_access('remove own tzreport content'));
  }
}

function tzproto_create_tzreport_from_protobuf($data, &$result) {
  global $user;

  // Find new job node id
  if(!$data->job_id()) {
    $result->set_error_code(400);
    $result->set_error_msg('invalid job id');
    return;
  }

  $report = new stdClass();
  $original_report = NULL;

  // Load node if this is an update
  if($data->id()) {
    $report = node_load($data->id());
    if(!node_access('update', $report)) {
      $result->set_error_code(403);
      $result->set_error_msg('access denied');
      return;
    }

    // Save original so we can check if anything really changed later
    $original_report = clone($report);
  } else {
    if(!node_access('create', 'tzreport')) {
      $result->set_error_code(403);
      $result->set_error_msg('access denied');
      return;
    }
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
  $worked_time = $data->worked_time();

  if (!empty($worked_time)) {
    $start = $worked_time->start();
    $end = $worked_time->end();
    $date = $worked_time->date();
    if (empty($date) || empty($start) || empty($end)) {
      $result->set_error_code(400);
      $result->set_error_msg('Report lacked a date, start time or end time value.');
      return;
    }
    $begin_date_time = tzproto_create_datetime($worked_time->date(), $worked_time->start());
    $end_date_time = tzproto_create_datetime($worked_time->date(), $worked_time->end());
    if (empty($begin_date_time) || empty($end_date_time)) {
      $result->set_error_code(400);
      $result->set_error_msg('Report lacked start time or end time value.');
      return;
    }

    // We only handle this case for proto version 1 or higher,
    if ($end_date_time < $begin_date_time) {
      $end_date_time->modify('+1 day');
    }

    $report->begintime = $begin_date_time->format('U');
    $report->endtime = $end_date_time->format('U');
  } else {
    $report->begintime = $data->begin_time();
    $report->endtime = $data->end_time();
    if ($report->endtime < $report->begintime) {
      $result->set_error_code(400);
      $result->set_error_msg('Report ends before it begins.');
      return;
    }
  }
  $report->breakduration = $data->break_duration();

  $report->travelduration = 0;
  $report->travelkm = 0;
  $report->signature = "";

  // Check for permissions to set the requested flags
  if(empty($report->flags) || ($report->flags != $data->flags())) {
    if($data->flags() != TZFlags::DELETED || user_access('remove own tzreport content')) {
      $report->flags = $data->flags();
    } else {
      $result->set_error_code(403);
      $result->set_error_msg('access denied');
      return;
    }
  }

  $report->assignedto = $user->uid;

  // Set author and date
  $report->uid = $user->uid;
  $report->name = $user->name;


  // Submit and save if there are changes
  if($report != $original_report) {
    if(!empty($report->nid)) {
      // Create new revision of existing reports to avoid race conditions
      $report->revision = TRUE;
    }
    $report = node_submit($report);
    node_save($report);

    // Invoke hook to allow modules to collect statistics or billing info
    module_invoke_all('tzproto_saved_report', $report);

    if($data->id()) {
      watchdog('tzproto', 'Created revision !vid of report !nid', array('!vid' => $report->vid, '!nid' => $report->nid),
        WATCHDOG_NOTICE,
        l('View', 'node/' . $report->nid));

      /* If the node has been updated and is immediately
       * reloaded after update, it will
       *  load the OLD cached version. */
      node_load(0, NULL, TRUE);
    } else {
      watchdog('tzproto', 'Inserted report !nid', array('!nid' => $report->nid), WATCHDOG_NOTICE, l('View', 'node/' . $report->nid));
    }
  }

  return $report;
}

function tzproto_create_tzjob_from_protobuf($data, &$result) {
  global $user;

  $job = new stdClass();

  // Load node if this is an update
  if($data->id()) {
    $job = node_load($data->id());
    if(!node_access('update', $job)) {
      $result->set_error_code(403);
      $result->set_error_msg('access denied');
      return;
    }
  } else {
    // new node, check create permissions
    if(!node_access('create', 'tzjob')) {
      $result->set_error_code(403);
      $result->set_error_msg('access denied');
      return;
    }

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
  $job->changed = time();
  node_save($job);

  if($data->id()) {
    watchdog('tzproto', 'Updated job !nid', array('!nid' => $job->nid), WATCHDOG_NOTICE, l('View', 'node/' . $job->nid));
    /* If the node has been updated and is immediately
     * reloaded after update, it will
     *  load the OLD cached version. */
    node_load(0, NULL, TRUE);
  } else {
    watchdog('tzproto', 'Inserted job !nid', array('!nid' => $job->nid), WATCHDOG_NOTICE, l('View', 'node/' . $job->nid));
  }

  return $job;
}

function tzproto_create_tzdate($datetime) {
  $date = new TZDate();
  $date->set_year(intval($datetime->format('Y'), 10));
  $date->set_month(intval($datetime->format('n'), 10)); // Month w.o. leading zeroes.
  $date->set_day(intval($datetime->format('j'), 10)); // Day w.o. leading zeroes.
  return $date;
}


function tzproto_create_tztime($datetime) {
  $time = new TZTime();
  $time->set_hour(intval($datetime->format('G'), 10));
  $time->set_minute(intval($datetime->format('i'), 10));
  return $time;
}

function tzproto_create_datetime($tzdate, $tztime) {
  $datetime = NULL;

  $year = $tzdate->year();
  $month = $tzdate->month();
  $day = $tzdate->day();
  $hour = $tztime->hour();
  $minute = $tztime->minute();

  if (!empty($year) && !empty($month) && !empty($day) &&
    $hour !== NULL && $minute !== NULL ) {
        $datetime = new DateTime();
        $datetime->setTimezone(date_default_timezone(FALSE));
        $datetime->setDate($year, $month, $day);
        $datetime->setTime($hour, $minute);
  }
  return $datetime;
}

function tzproto_create_tztimespan($epoch_start, $epoch_end) {
      $datetime = tzbase_make_date($epoch_start);
      if (empty($datetime)) {
        return NULL;
      }
      $date = tzproto_create_tzdate($datetime);
      $start_time = tzproto_create_tztime($datetime);

      $datetime = tzbase_make_date($epoch_end);
      if (empty($datetime)) {
        return NULL;
      }
      $end_time = tzproto_create_tztime($datetime);

      $time_span = new TZTimeSpan();
      $time_span->set_date($date);
      $time_span->set_start($start_time);
      $time_span->set_end($end_time);
      return $time_span;
}
