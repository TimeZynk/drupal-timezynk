<?php

/*
 * Controller class for handling data synchronization between intellitime and timezynk
 */
class TZIntellitimeSyncController {
  const SYNC_OK = 0;
  const PENDING = 1;
  const NETWORK_FAILURE = 2;
  const AUTH_FAILURE = 3;

  private $account = NULL;
  private $bot = NULL;
  private $week_status = array();

  function __construct($account) {
    tzbase_include_proto_classes();
    tzintellitime_include_classes();
    $this->account = $account;
    $this->persister = new TZIntellitimePersister($this->account);
    $this->bot = new TZIntellitimeBot(TZINTELLITIME_BASE_URL, $account->intellitime_session_data);
  }

  /**
   * Synchronize with intellitime.
   * @return SYNC_OK on success, and NETWORK_FAILURE or AUTH_FAILURE respectively on error.
   */
  function synchronize() {
    $this->add_standard_weeks();
    $this->add_unlocked_weeks();
    $this->add_updated_weeks();

    $this->process_pending_weeks();

    // Check status for the weeks

    // Authentication failed
    if(array_search(TZIntellitimeSyncController::AUTH_FAILURE, $this->week_status)) {
      return TZIntellitimeSyncController::AUTH_FAILURE;
    }

    // Failed to load some weeks because of network error
    if(array_search(TZIntellitimeSyncController::NETWORK_FAILURE, $this->week_status)) {
      return TZIntellitimeSyncController::NETWORK_FAILURE;
    }

    // All went fine!
    return TZIntellitimeSyncController::SYNC_OK;
  }

  /**
   * Get a week-unique key for this DateTime object. Suitable for grouping
   * several requests by week. Current format is <WeekYear>W<Week> which
   * is chosen because it can be parsed straight up by DateTime.
   * @param DateTime $datetime
   * @return string YYYY'W'WW
   */
  function week_key($datetime) {
    return $datetime->format('o\WW');
  }

  /**
   * Make an array containing the boundaries for the week expressed as epoch timestamps.
   * @param $week Week as a string <WeekYear>W<Week>
   * @param $timezone timezone to use
   * @return Array (first second of week, first second of next week)
   */
  function epoch_span($week, $timezone) {
    $epoch_span = array();
    $date = new DateTime($week, $timezone);
      // Find beginning and end of week
    $date->modify('midnight');
    $epoch_span[0] = $date->format('U');
    $date->modify('+7 days');
    $epoch_span[1] = $date->format('U');
    return $epoch_span;
  }


  function set_week_status($datetime, $status) {
    $this->week_status[$this->week_key($datetime)] = $status;
  }

  /**
   * Add default weeks to the pending synchronization
   */
  function add_standard_weeks() {
    $date = tzbase_make_date();

    // Always sync this week
    $this->set_week_status($date, TZIntellitimeSyncController::PENDING);

    // Sync weeks forward in time according to current configuration
    $weeks_to_sync = (int)TZINTELLITIME_SYNC_WEEKS_FORWARD;
    for($i = 1; $i <= $weeks_to_sync; $i++) {
      $date->modify('+7 days');
      $this->set_week_status($date, TZIntellitimeSyncController::PENDING);
    }
  }

  /**
   * Add weeks where we have unlocked reports as pending for sync.
   */
  function add_unlocked_weeks() {
    $result = db_query('SELECT begintime FROM {tzreport} WHERE assignedto = %d AND flags <= %d', $this->account->uid, TZFlags::REPORTED);
    while($time = db_result($result)) {
      $this->set_week_status(tzbase_make_date($time), TZIntellitimeSyncController::PENDING);
    }
  }

  /**
   * Add weeks where we have updated reports as pending for sync.
   */
  function add_updated_weeks() {
    $result = db_query('SELECT begintime FROM {tzreport} tzreport INNER JOIN {tzintellitime} tzintellitime ON tzreport.vid = tzintellitime.vid WHERE tzreport.assignedto = %d AND tzintellitime.intellitime_local_changes > 0', $this->account->uid);
    while($time = db_result($result)) {
      $this->set_week_status(tzbase_make_date($time), TZIntellitimeSyncController::PENDING);
    }
  }

  /**
   * Checks for new unfinished weeks and adds them as pending.
   * @param $data data from TZIntellitimeBot
   */
  function add_unfinished_weeks($data) {
    if($data && !empty($data['unfinished_weeks'])) {
      foreach($data['unfinished_weeks'] as $weekdate) {
        $weekstr = $this->week_key($weekdate);
        if(!isset($this->week_status[$weekstr])) {
          $this->week_status[$weekstr] = TZIntellitimeSyncController::PENDING;
        }
      }
    }
  }

  /**
   * Process weeks that needs to be written to the server or read from the
   * server.
   */
  function process_pending_weeks() {
    $timezone = date_default_timezone(FALSE);
    $deleted_reports = 0;

    while($week = array_search(TZIntellitimeSyncController::PENDING, $this->week_status)) {
      $data = NULL;

      $weekdate = new DateTime($week, $timezone);

      /* To avoid race conditions with the user, we start by fetching current copies of
       * all local reports. We then contact the intellitime server and fetches its version
       * of the reports. Afterwards, the reports are compared and if we find any changes
       * from the server they are saved as an updated to the originally fetched revision.
       * For this scheme to work, we must always create a new revision whenever a user
       * updates a report on our server so that we don't risk user changes being overwritten
       * with old data from the intellitime server. */
      $updated_reports = array();
      $epoch_span = $this->epoch_span($week, $timezone);
      $result = db_query('SELECT n.nid FROM {node} n INNER JOIN {tzreport} t ON n.vid = t.vid WHERE t.assignedto = %d AND t.begintime >= %d AND t.begintime < %d AND t.flags != 255 AND n.status = 1',
                         $this->account->uid, $epoch_span[0], $epoch_span[1]);
      while($nid = db_result($result)) {
        $node = node_load($nid, NULL, TRUE);
        if(!empty($node->intellitime_id)) {
          $this->persister->add_existing_report($node);
          if(!empty($node->intellitime_local_changes)) {
            $updated_reports[] = $node;
          }
        }
      }

      try {
        if(!empty($updated_reports)) {
          $data = $this->send_updated_reports($updated_reports);
        }
        if(empty($data)) {
          $data = $this->bot->load_week($weekdate);
        }
        $this->week_status[$week] = TZIntellitimeSyncController::SYNC_OK;

        //
        $this->persister->convert_assignments($data['assignments']);
        $seen_reports = $this->persister->convert_reports($data['reports']);

        $deleted_reports += $this->mark_removed_reports($seen_reports, $epoch_span[0], $epoch_span[1]);

        $this->add_unfinished_weeks($data);

      } catch(TZAuthenticationFailureException $e) {
        // If authentication failed, there is not much point to try another week
        $this->week_status[$week] = TZIntellitimeSyncController::AUTH_FAILURE;
        break;
      } catch(TZNetworkFailureException $e) {
        // No data, try a different week
        $this->week_status[$week] = TZIntellitimeSyncController::NETWORK_FAILURE;
      }
    }

    if($deleted_reports) {
      watchdog('tzintellitime',
               'Removed !nbr reports after sync',
               array('!nbr' => $deleted_reports),
               WATCHDOG_NOTICE,
               theme('username', $this->account));
    }
  }



  /**
   * Mark reports as deleted if they are in our synchronized date range but was not
   * found on the server.
   * @param Array $seen_reports lists of all reports found in the date span
   * @param int $begin Epoch timestamp for the beginning of the date span
   * @param int $end Epoch timestamp for the end for the date span
   * @return number of deleted reports
   */
  function mark_removed_reports($seen_reports, $begin, $end) {
    $seenquery = array();
    foreach($seen_reports as $nid) {
      $seenquery[] = 'n.nid != %d';
    }
    $seenquery = implode(' AND ', $seenquery);

    // Select reports in range
    $queryargs = array(time(), TZFlags::DELETED, TZFlags::DELETED, $begin, $end, $this->account->uid);
    $queryargs = array_merge($queryargs, $seen_reports);
    $query = 'UPDATE {node} n INNER JOIN {tzreport} t ON n.nid = t.nid SET changed = %d, flags = %d WHERE flags != %d AND begintime >= %d AND begintime < %d AND assignedto = %d';
    if(!empty($seen_reports)) {
      // Update tzreport->flags to TZFlags::DELETED if the node ID is not found in the $seen_reports array
      $query .= ' AND ' . $seenquery;
    }
    db_query($query, $queryargs);

    return db_affected_rows();
  }

  /**
   * Send updated reports to server
   */
  function send_updated_reports($reports) {
    $data = array();
    foreach($reports as $tzreport) {

      /* TODO: Since we send reports per week anyway we might as well
               implement the batching in bot->update_report instead. */
      $begindate = tzbase_make_date($tzreport->begintime);
      // Convert and post report
      $itreport = new TZIntellitimeReport($tzreport);
      $data = $this->bot->update_report($itreport);

      watchdog('tzintellitime', 'Sent report with nid !nid to week !week',
        array('!nid' => $tzreport->nid, '!itid' => $tzreport->intellitime_id, '!week' => $begindate->format('o\WW')),
        WATCHDOG_NOTICE,
        l('Node', 'node/' . $tzreport->nid) . ', ' . theme('username', $this->account));
    }
    return $data;
  }
}

class TZIntellitimePersister {

  public function __construct($account) {
    $this->account = $account;
    $this->jobmap = array();
    $this->existing_reports = array();
  }

  public function add_existing_report($node) {
    $this->existing_reports[$node->intellitime_id] = $node;
  }

  /**
   * Convert all TZIntellitimeAssignments in $intelli_assignments to tzjobs.
   * @param Array $intelli_assignments array of TZIntellitimeAssignments.
   * @return nothing
   */
  function convert_assignments($intelli_assignments) {
    // Create default absence parent job if needed
    $absence_assignment = new TZIntellitimeAssignment();
    $absence_assignment->id = TZINTELLITIME_SYNC_ABSENCE_PARENT_JOB_ID;
    $absence_assignment->title = t('Absence');
    $absence_node = $this->store_job($absence_assignment);

    foreach($intelli_assignments as $intelli_assignment) {
      $parentid = 0;
      // Place absence type assignments as children of the absence parent job
      if ($intelli_assignment->type == TZIntellitimeAssignment::TYPE_ABSENCE) {
        $parentid = $absence_node->nid;
      }
      $this->store_job($intelli_assignment, $parentid);
    }
  }

  /**
   * Convert all TZIntellitimeReports in $intelli_reports to tzreports.
   * @param Array $intelli_reports array of TZIntellitimeReports.
   * @return Array array of tzreport->nid for all processed reports.
   */
  function convert_reports($intelli_reports) {
    $seen_reports = array();
    foreach($intelli_reports as $itreport) {
      $tzreport = $this->store_report($itreport);
      if(!empty($tzreport)) {
        $seen_reports[] = $tzreport->nid;
      }
    }
    return $seen_reports;
  }


  /**
   * Create or update a tzreport from a TZIntellitimeReport
   * @param TZIntellitimeReport $intelli_report
   */
  function store_report($intelli_report) {
    $intellitime_id = $intelli_report->get_intellitime_id($this->account->uid);

    // Try to find existing report
    $tzreport = NULL;
    if(!empty($this->existing_reports[$intellitime_id])) {
      $tzreport = $this->existing_reports[$intellitime_id];
    };

    $original_tzreport = NULL;
    if($tzreport) {
      $original_tzreport = clone $tzreport;
    }

    $tzreport = $intelli_report->convert_to_tzreport($this->account, $tzreport);

    // Map job to report by searching for matching title
    $jobid = $this->match_job($intelli_report->title);
    if (!$jobid) {
      // Create placeholder job if needed
      $placeholder_assignment = new TZIntellitimeAssignment();
      $placeholder_assignment->id = "PLACEHOLDER_ID_" . md5($intelli_report->title);
      $placeholder_assignment->title = $intelli_report->title;
      $placeholder_assignment->report_key = $intelli_report->title;

      $placeholder_node = $this->store_job($placeholder_assignment);
      $jobid = $placeholder_node->nid;
    }

    $tzreport->jobid = $jobid;
    $tzreport = $this->store_node($tzreport, $original_tzreport);

    return $tzreport;
  }

  /**
   * Create or update a tzjob from a TZIntellitimeAssignment
   * @param TZIntellitimeAssignment $intelli_assignment
   * @param int $parentid optional parentid to set
   */
  function store_job($intelli_assignment, $parentid = 0) {
    // Load node if this is an update
    $job = tzintellitime_node_load_by_intellitime_id($intelli_assignment->id);

    // Clone job to be able to check for changes later
    $original_job = NULL;
    if($job) {
      $original_job = clone $job;
    }

    $job = $intelli_assignment->convert_to_tzjob($this->account, $job);
    $job->parentid = $parentid;

    $job = $this->store_node($job, $original_job);

    /* If the assignment lacks a valid report_key it has been internally created
     * and should not be used for matching reports. */
    if (!empty($intelli_assignment->report_key)) {
      $this->jobmap[$intelli_assignment->report_key] = $job->nid;
    }

    return $job;
  }




 /**
   * Does the actual work of storing a node to the drupal database.
   * @param Object $node_data actual node data to save.
   * @param Object $original_node original node data to compare to before saving.
   * @return Object Full drupal node object.
   */
  function store_node($node, $original_node) {
    /* Set local_changes here to make sure the comparison below
     * triggers a save if the node is synched for the first time */
    $node->intellitime_local_changes = 0;

    if($node == $original_node) {
      return $node;
    }

    /* Save synchronized flag so we can check it the node has been modified
     * when we run the synchronization the next time */
    $node->intellitime_synchronized = TRUE;

    $is_new = empty($node->nid);

    $node = node_submit($node);
    node_save($node);

    watchdog('tzintellitime', '!operation !type !nid',
             array(
               '!operation' => ($is_new ? 'Inserted' : 'Updated'),
               '!type' => $node->type,
               '!nid' => $node->nid
             ),
             WATCHDOG_NOTICE,
             l('Node', 'node/' . $node->nid) . ', ' . theme('username', $this->account));

    return $node;
  }

  /**
   * Intellitime job ids are comma-separated strings from several
   * task levels that are sometimes abbreviated in different places
   * on the web page. The abbreviation algorithm removes a few
   * characters from the end of each string before concatenating
   * each string separated by commas.
   *
   * We need to be able to match the full-length versions of these
   * strings with each abbreviated version, and that is the task
   * of this function.
   *
   * <h2>Example</h2>
   * Full version: 'Test Company, My assignment, My task, My reporting code'
   * Abbreviated version: 'Test Com, My assig, My t, My repor'
   *
   * @param String $jobtitle
   */
  function match_job($jobtitle) {
    if(!empty($this->jobmap[$jobtitle])) {
      return $this->jobmap[$jobtitle];
    }

    $jobpath = explode(', ', $jobtitle);
    foreach($this->jobmap as $key => $jobid) {
      $match = TRUE;
      $keypath = explode(', ', $key);
      if(count($jobpath) != count($keypath)) {
        continue;
      }
      for($i = 0; $i < count($jobpath); $i++) {
        $key = $keypath[$i];
        $job = $jobpath[$i];
        $segment = substr($job, 0, strlen($key));
        if($key != $segment) {
          $match = FALSE;
          break;
        }
      }
      if($match) {
        $this->jobmap[$jobtitle] = $jobid;
        return $jobid;
      }
    }

    return FALSE;
  }
}