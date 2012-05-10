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
    watchdog('tzproto', 'Empty POST', array(), WATCHDOG_WARNING);
    drupal_set_header('HTTP/1.1 405 No input data');
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
  if(tzproto_login($req)) {
    for ($i = 0; $i < $req->commands_size(); $i++) {
      $cmd = $req->command($i);

      // Return client request ID
      $result = $response->add_result();
      $result->set_client_handle($cmd->client_handle());

      try {
        _tzproto_handle_cmd($cmd, $result, $protocol_version);
      } catch (TZProtoException $e) {
        $result->set_error_code($e->getCode());
        $result->set_error_msg($e->getMessage());
      }
    }
  } else {
    watchdog('tzproto', 'Mobile login failed for @username',
      array('@username' => $req->username()), WATCHDOG_WARNING);
    if ($protocol_version >= 4) {
      $error = new TZError();
      $error->set_code(401);
      $error->set_msg('Wrong username or password');
      $response->set_error($error);
    } else {
      drupal_set_header('HTTP/1.1 401 Wrong username or password');
      die();
    }
  }

  _tzproto_log_request($req, $response);

  $response_data = $response->SerializeToString();
  if ($use_base64) {
    $response_data = base64_encode($response_data);
  }

  print($response_data);

  tzproto_logout();

  // All handling finished, prevent output from other modules
  module_invoke_all('exit');
  exit;
}

function _tzproto_handle_cmd($cmd, &$result, $protocol_version) {
  if($cmd->get_user_cmd()) {
    tzproto_handle_get_user($cmd->get_user_cmd(), $result);
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
    $handler = new CreateReportHandler($protocol_version, new TZDrupalDBWrapper());
    $handler->handle($cmd->create_report_cmd(), $result);
  }
  else if ($cmd->get_availability_cmd()) {
    $handler = new GetAvailabilityHandler($protocol_version, $GLOBALS['user'], tzbase_availability_store());
    $handler->handle($cmd->get_availability_cmd(), $result);
  }
  else if ($cmd->save_availability_cmd()) {
    $handler = new SaveAvailabilityHandler($protocol_version, $GLOBALS['user'], tzbase_availability_store());
    $handler->handle($cmd->save_availability_cmd(), $result);
  }
  else if ($cmd->delete_availability_cmd()) {
    $handler = new DeleteAvailabilityHandler($protocol_version, $GLOBALS['user'], tzbase_availability_store());
    $handler->handle($cmd->delete_availability_cmd(), $result);
  }
  else if ($cmd->get_availability_intervals_cmd()) {
    $handler = new GetAvailabilityIntervalHandler($protocol_version, tzbase_availability_store());
    $handler->handle($cmd->get_availability_intervals_cmd(), $result);
  }
  else if ($cmd->get_report_templates_cmd()) {
    $handler = new GetReportTemplatesHandler($protocol_version, new TZDrupalDBWrapper());
    $handler->handle($cmd->get_report_templates_cmd(), $result);
  }
  else {
    throw new TZProtoException('Not implemented', 501);
  }
}

function _tzproto_log_request($request, $response) {
  global $user;
  if (empty($user->tzuser['log_level'])) {
    return;
  }
  if ($user->tzuser['log_level'] >= TZUSER_LOG_LEVEL_MEDIUM) {
    watchdog('tzproto_medium', 'var http_vars = ' . json_encode(tzbase_http_server_vars()) . ';');
  }
  if ($user->tzuser['log_level'] >= TZUSER_LOG_LEVEL_VERBOSE) {
    watchdog('tzproto_verbose', 'var request = ' . $request->toJson() . ";\n\nvar response = " . $response->toJson() . ';');
  }
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
    return $user;
  }
  module_invoke_all('tzproto_login_failed', $req->username(), $_SERVER['HTTP_USER_AGENT']);
  return FALSE;
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
 * Handle the get_user command
 */
function tzproto_handle_get_user(&$get_user_cmd, &$result) {
  global $user;
  $tzuser = new TZUser();

  // Save client version code if available and changed
  if ($version_code = $get_user_cmd->version_code()) {
    if (empty($user->tzuser['version_code']) || $version_code != $user->tzuser['version_code']) {
      $user = user_save($user, array('tzuser' => array('version_code' => $version_code)));
    }
  }

  // Load basic data
  $tzuser->set_id($user->uid);
  $tzuser->set_username($user->name);
  $tzuser->set_realname(theme('username', $user, FALSE));
  $tzuser->set_may_create_job(user_access('create tzjob content', $user));
  $tzuser->set_may_create_reports(user_access('create tzreport content', $user));
  $tzuser->set_may_create_availability(user_access('create availability content', $user));

  $result->set_user($tzuser);
}

/**
 * Handle the get job command
 */
function tzproto_handle_get_job_cmd(&$cmd, &$result) {
  $jobcmd = $cmd->get_job_cmd();
  $jobresult = new TZGetJobResult();

  // Are we looking for a single id?
  if($jobcmd->job_id()) {
    $node = tzbase_node_load($jobcmd->job_id());
    if($node && $node->nid) {
      $tzjob = $jobresult->add_job();
      tzproto_fill_from_node($tzjob, $node);
    }
  }
  // Fetch a collections of jobs
  else {
    $tzjobs = array();

    if ($jobcmd->may_create_report() && !user_access('create tzreport on any tzjob')) {

      global $user;
      $user_job_mappings = tzbase_user_jobs_mapper()->find($user->uid);
      foreach($user_job_mappings as $mapping) {
        $jobid = $mapping->getJobId();
        if (empty($tzjobs[$jobid])) {
          $tzjobs[$jobid] = _tzproto_load_job($jobid, TRUE);
        }
      }
    } else {
      $sql = "SELECT n.nid FROM {node} n WHERE type = 'tzjob' AND status = 1 ORDER BY title";
      $db_result = FALSE;
      if ($jobcmd->limit()) {
        $db_result = db_query_range(db_rewrite_sql($sql), $jobcmd->offset(), $jobcmd->limit());
      } else {
        $db_result = db_query(db_rewrite_sql($sql));
      }

      while ($data = db_fetch_object($db_result)) {
        $tzjobs[] = _tzproto_load_job($data->nid, FALSE);
      }
    }
    $jobs = array_filter($tzjobs);
    usort($jobs, "_tzproto_sort_jobs_by_title");
    $jobresult->set_all_jobs($jobs);
  }

  $result->set_get_job_result($jobresult);
}

function _tzproto_sort_jobs_by_title($a, $b) {
  return strnatcasecmp($a->title(), $b->title());
}

function _tzproto_load_job($id, $filterAwayInactive) {
  $node = tzbase_node_load($id);
  if (empty($node) || $node->status != 1) {
    return NULL;
  }
  if ($filterAwayInactive && $node->flags != TZJobFlags::ACTIVE) {
    return NULL;
  }

  $tzjob = new TZJob();
  tzproto_fill_from_node($tzjob, $node);
  return $tzjob;
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
  $report_id = $reportcmd->report_id();
  if (!$report_id) {
    $report_id = $reportcmd->numeric_report_id();
  }
  if($report_id) {
    $node = tzbase_node_load($report_id);
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
      $datetime_range = tzproto_create_datetime_range($date_range);

      if ($datetime_range->start) {
        $filter_strings[] = 'tz.begintime >= %d';
        $filter_args[] = $datetime_range->start->format('U');
      }

      if ($datetime_range->end) {
        $filter_strings[] = 'tz.begintime < %d';
        $filter_args[] = $datetime_range->end->format('U');
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
      $node = tzbase_node_load($data->nid);
      $tzreport = $reportresult->add_report();
      tzproto_fill_from_node($tzreport, $node, $protocol_version);
    }
  }
  // Invoke hook to allow modules to collect statistics or billing info
  module_invoke_all('tzproto_fetched_reports', $user, $reportresult->reports_size());
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
 * Fill a protobuf object from a node object
 * @param $proto protobuf object to fill
 * @param $node node data
 */
function tzproto_fill_from_node(&$proto, $node, $protocol_version = 0) {
  $proto->set_id($node->nid);
  $proto->set_flags($node->flags);
  $proto->set_title($node->title);
  $proto->set_changed($node->changed);

  if($node->type == 'tzjob') {
    $proto->set_job_type($node->jobtype);
    $proto->set_parent_id($node->parentid);
    $proto->set_description($node->body);
    $proto->set_may_create_child(node_access('create', $node->type));
    $proto->set_may_create_report(node_access('create', 'tzreport') && $node->flags == TZJobFlags::ACTIVE);
    $proto->set_may_edit(node_access('update', $node));
  } else if($node->type == 'tzreport') {
    $proto->set_numeric_id($node->nid);
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
    if ($protocol_version >= 3) {
      $tzbreak = new TZBreak();
      $tzduration = new TZTime();
      $tzduration->set_hour(floor($node->breakduration / 3600));
      $tzduration->set_minute(floor(($node->breakduration - $tzduration->hour() * 3600) / 60));
      $tzbreak->set_break_duration($tzduration);
      $proto->set_break_time($tzbreak);
      $proto->set_comments($node->body);
    } else {
      $proto->set_break_duration($node->breakduration);
      $proto->set_description($node->body);
    }

    $proto->set_may_edit(node_access('update', $node));
    $proto->set_may_remove(user_access('remove own tzreport content'));
  }
}

function tzproto_create_tzjob_from_protobuf($data) {
  global $user;

  $job = new stdClass();

  // Load node if this is an update
  if($data->id()) {
    $job = tzbase_node_load($data->id());
    if(!node_access('update', $job)) {
      throw new TZProtoException('access denied', 403);
    }
  } else {
    // new node, check create permissions
    if(!node_access('create', 'tzjob')) {
      throw new TZProtoException('access denied', 403);
    }

    $job->type = 'tzjob';
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

function tzproto_create_datetime_range($date_range) {
  $range = (object)array(
    'start' => null,
    'end' => null,
  );

  if ($date_range) {
    $time = new TZTime();
    $time->set_hour(0);
    $time->set_minute(0);

    $start_date = $date_range->start();
    if ($start_date) {
      // Start date, create a timestamp for this date, with time set to 00:00.
      $range->start = tzproto_create_datetime($start_date, $time);
    }

    $end_date = $date_range->end();
    if ($end_date) {
      // End date, create a timestamp for the day after, with time set to 00:00.
      $range->end = tzproto_create_datetime($end_date, $time);
      $range->end->modify("+1 day");
    }
  }

  return $range;
}
