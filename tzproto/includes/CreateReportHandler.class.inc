<?php

class CreateReportHandler extends CommandHandler {
  public function handle($create_report_cmd, &$result) {
    $reportresult = new TZCreateReportResult();

    // Store new reports
    $new_report = $create_report_cmd->new_report();
    if($new_report) {
      $report = $this->create_tzreport_from_protobuf($new_report);
      $reportresult->set_id($report->nid);
    }
    $result->set_create_report_result($reportresult);
  }

  private function create_tzreport_from_protobuf($data) {
    global $user;

    // Find new job node id
    if(!$data->job_id()) {
      throw new TZProtoException('no job id', 400);
    }

    $report = new stdClass();
    $original_report = NULL;

    $id = NULL;
    if ($data->id()) {
      $id = $data->id();
    } else if ($data->numeric_id()) {
      $id = $data->numeric_id();
    }

    // Load node if this is an update
    if($id) {
      $report = tzbase_node_load($id);
      if(!node_access('update', $report)) {
        throw new TZProtoException('access denied', 403);
      }

      // Save original so we can check if anything really changed later
      $original_report = clone($report);
    } else {
      if(!node_access('create', 'tzreport')) {
        throw new TZProtoException('access denied', 403);
      }
      $report->type = 'tzreport';
      $report->created = $data->changed();
    }

    $job = $this->load_job($data->job_id());
    if (empty($job)) {
      throw new TZProtoException('job not found', 400);
    }
    $report->jobid = $job->nid;


    // Set title if available
    if($data->title()) {
      $report->title = $data->title();
    } else {
      // Get title from job
      $report->title = $job->title ? $job->title : 'Time Report from ' . $user->name;
    }

    if ($data->comments()) {
      $report->body = $data->comments();
    } else {
      $report->body = $data->description();
    }

    // Parse start and finish
    $worked_time = $data->worked_time();

    if (!empty($worked_time)) {
      $start = $worked_time->start();
      $end = $worked_time->end();
      $date = $worked_time->date();
      if (empty($date) || empty($start) || empty($end)) {
        throw new TZProtoException('Report lacked a date, start time or end time value.', 400);
      }
      $begin_date_time = tzproto_create_datetime($worked_time->date(), $worked_time->start());
      $end_date_time = tzproto_create_datetime($worked_time->date(), $worked_time->end());
      if (empty($begin_date_time) || empty($end_date_time)) {
        throw new TZProtoException('Report lacked start time or end time value.', 400);
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
        throw new TZProtoException('Report ends before it begins.', 400);
      }
    }

    $break_time = $data->break_time();
    $report->breakduration = 0;
    if (!empty($break_time)) {
      $duration = $break_time->break_duration();
      if (!empty($duration)) {
        $report->breakduration = $duration->hour() * 3600 + $duration->minute() * 60;
      }
    } else if ($data->break_duration()) {
      $report->breakduration = $data->break_duration();
    }

    $report->travelduration = 0;
    $report->travelkm = 0;
    $report->signature = "";

    // Check for permissions to set the requested flags
    if(empty($report->flags) || ($report->flags != $data->flags())) {
      if($data->flags() != TZFlags::DELETED || user_access('remove own tzreport content')) {
        $report->flags = $data->flags();
      } else {
        throw new TZProtoException('access denied', 403);
      }
    }

    $report->assignedto = $user->uid;

    // Set author and date
    $report->uid = $user->uid;
    $report->name = $user->name;

    $this->validate_report($report, $job);

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

      if($id) {
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

  private function validate_report($report, $job) {
    if ($report->flags == TZFlags::DELETED) {
      return;
    }

    $this->validate_overlap($report, $job);
    $this->validate_permissions($report);
  }

  private function validate_overlap($r, $job) {
    if ($this->protocol_version < 2) {
      return;
    }
    if ($job->allow_overlap) {
      return;
    }
    $query = 'SELECT COUNT(n.nid) FROM {node} n ' .
             'INNER JOIN {tzreport} tzr ON n.vid = tzr.vid ' .
             'INNER JOIN {tzjob} tzj ON tzr.jobid = tzj.nid ' .
             'WHERE n.nid != %d AND ' .
             'tzr.flags != 255 AND ' .
             'tzj.allow_overlap = 0 AND ' .
             'tzr.assignedto = %d AND ( ' .
               '(tzr.begintime >= %d AND tzr.begintime < %d) OR ' .
               '(tzr.endtime > %d AND tzr.endtime <= %d) OR ' .
               '(tzr.begintime < %d AND tzr.endtime > %d) ' .
             ')';
    $nid = empty($r->nid) ? 0 : $r->nid;
    $result = db_result(db_query($query, $nid, $r->uid,
      $r->begintime, $r->endtime, $r->begintime, $r->endtime,
      $r->begintime, $r->endtime));
    if ($result) {
      throw new TZProtoException('Report overlaps existing report', 409);
    }
  }

  private function validate_permissions($report) {
    if (!empty($report->nid)) {
      // Updates are always allowed
      return;
    }
    if (user_access('create tzreport on any tzjob')) {
      return;
    }
    $mapper = tzbase_user_jobs_mapper();
    $may_create_report = $mapper->userMayCreateReport($report->uid, $report->jobid, $report->begintime);
    if (!$may_create_report) {
      throw new TZProtoException('May not create report on this date for this assignment', 403);
    }
  }

  private function load_job($jobid) {
    $query = 'SELECT * FROM {node} n INNER JOIN {tzjob} tzj ON n.vid = tzj.vid WHERE n.nid = %d';
    return db_fetch_object(db_query($query, $jobid));
  }
}
