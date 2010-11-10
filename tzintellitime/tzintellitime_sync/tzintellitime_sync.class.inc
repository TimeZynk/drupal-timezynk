<?php

/**
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
  private $data = array(
   'assignments' => array(),
   'reports' => array(),
  );

  function __construct($account) {
    tzbase_include_proto_classes();
    tzintellitime_include_classes();
    $this->account = $account;
    $this->bot = new TZIntellitimeBot(TZINTELLITIME_BASE_URL, $account->intellitime_session_data);
  }

  /**
   * Synchronize with intellitime.
   * @return SYNC_OK on success, and NETWORK_FAILURE or AUTH_FAILURE respectively on error.
   */
  function synchronize() {

    $this->add_standard_weeks();
    $this->add_unlocked_weeks();
    $this->send_updated_reports();
    $this->load_pending_weeks();

    // Check if we could sync anything
    if(array_search(TZIntellitimeSyncController::NETWORK_FAILURE, $this->week_status)) {
      // Failed to load week because of network error, give up for now
      return TZIntellitimeSyncController::NETWORK_FAILURE;
    } else if(array_search(TZIntellitimeSyncController::AUTH_FAILURE, $this->week_status)) {
      // Authentication failed
      return TZIntellitimeSyncController::AUTH_FAILURE;
    }

    // All weeks fetched, convert data
    $jobmap = $this->convert_assignments();
    $seen_reports = $this->convert_reports($jobmap);

    // Mark removed reports
    $this->mark_removed_reports($seen_reports);


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
   * Load pending weeks
   */
  function load_pending_weeks() {
    $timezone = date_default_timezone(FALSE);
    while($week = array_search(TZIntellitimeSyncController::PENDING, $this->week_status)) {
      $weekdate = new DateTime($week, $timezone);
      $data = $this->bot->load_week($weekdate);
      $this->process_week_data($week, $data);

      // If authentication failed, there is not much point to try again with a different page
      if($this->week_status[$week] == TZIntellitimeSyncController::AUTH_FAILURE) {
        break;
      }
    }
  }

  /**
   * Process the week data returned from TZIntellitimeBot.
   * It will go through all assignments and reports and merge them into
   * the $this->data field. It also checks for new unfinished weeks and
   * adds them as pending.
   * @param string $week current week as formatted by week_key()
   * @param $data data from TZIntellitimeBot
   */
  function process_week_data($week, $data) {
    if($data && isset($data['reports']) && isset($data['assignments'])) {
      foreach($data['assignments'] as $assignment) {
        $this->data['assignments'][$assignment->id] = $assignment;
      }
      foreach($data['reports'] as $report) {
        $this->data['reports'][$report->get_intellitime_id($this->account->uid)] = $report;
      }

      // Check for extra weeks that needs to be synchronized
      if(!empty($data['unfinished_weeks'])) {
        foreach($data['unfinished_weeks'] as $weekdate) {
          $weekstr = $this->week_key($weekdate);
          if(!isset($this->week_status[$weekstr])) {
            $this->week_status[$weekstr] = TZIntellitimeSyncController::PENDING;
          }
        }
      }

      $this->week_status[$week] = TZIntellitimeSyncController::SYNC_OK;
    } elseif(FALSE === $data) {
      $this->week_status[$week] = TZIntellitimeSyncController::NETWORK_FAILURE;
    } else {
      $this->week_status[$week] = TZIntellitimeSyncController::AUTH_FAILURE;
    }
  }

  /**
   * Convert all TZIntellitimeAssignments in $this->data to tzjobs.
   * @return Array map from <job key> => <job node id> for use with convert_reports
   */
  function convert_assignments() {
    $jobmap = array();

    // Create default absence parent job if needed
    $absence_assignment = new TZIntellitimeAssignment();
    $absence_assignment->id = TZINTELLITIME_SYNC_ABSENCE_PARENT_JOB_ID;
    $absence_assignment->title = t('Absence');
    $absence_node = $this->create_job($absence_assignment);

    foreach($this->data['assignments'] as $tzassignment) {
      $parentid = 0;
      // Place absence type assignments as children of the absence parent job
      if ($tzassignment->type == TZIntellitimeAssignment::TYPE_ABSENCE) {
        $parentid = $absence_node->nid;
      }
      $job = $this->create_job($tzassignment, $parentid);
      $jobmap[$tzassignment->report_key] = $job->nid;
    }
    return $jobmap;
  }

  /**
   * Convert all TZIntellitimeReports in $this->data to tzreports.
   * @param Array $jobmap jobmap from convert_assignments()
   * @return Array array of tzreport->nid for all processed reports.
   */
  function convert_reports($jobmap) {
    $seen_reports = array();
    foreach($this->data['reports'] as $itreport) {
      $tzreport = $this->create_report($itreport, $jobmap);
      $seen_reports[] = $tzreport->nid;
    }
    return $seen_reports;
  }

  /**
   * Mark reports as deleted if they are in our synchronized date range but was not
   * found on the server.
   * @param Array $seen_reports lists of all reports found during synchronization
   */
  function mark_removed_reports($seen_reports) {
    $timezone = date_default_timezone(FALSE);
    $affected_rows = 0;
    $seenquery = array();
    foreach($seen_reports as $nid) {
      $seenquery[] = 'n.nid != %d';
    }
    $seenquery = implode(' AND ', $seenquery);
    foreach(array_keys($this->week_status) as $week) {
      $date = new DateTime($week, $timezone);
      // Find beginning and end of week
      $date->modify('midnight');
      $begin = $date->format('U');
      $date->modify('+7 days');
      $end = $date->format('U');
      // Select reports in range
      $queryargs = array(time(), TZFlags::DELETED, TZFlags::DELETED, $begin, $end, $this->account->uid);
      $queryargs = array_merge($queryargs, $seen_reports);
      // Update tzreport->flags to TZFlags::DELETED if the node ID is not found in the $seen_reports array
      $query = 'UPDATE {node} n INNER JOIN {tzreport} t ON n.nid = t.nid SET changed = %d, flags = %d WHERE flags != %d AND begintime >= %d AND begintime < %d AND assignedto = %d AND ' . $seenquery;
      db_query($query, $queryargs);
      $affected_rows += db_affected_rows();
    }
    if($affected_rows) {
      watchdog('tzintellitime',
               'Removed !rows reports after sync',
               array('!rows' => $affected_rows,
                     '!user' => $this->account->name,
                     '!uid' => $this->account->uid),
               WATCHDOG_NOTICE,
               theme('username', $this->account));
    }
  }

  /**
   * Find reports that have been updated since last sync and send them to the server
   */
  function send_updated_reports() {
    // Find reports that need update
    $result = db_query('SELECT n.nid FROM {node} n INNER JOIN {tzreport} t ON n.vid = t.vid WHERE n.changed > %d AND t.assignedto = %d',
                       $this->account->intellitime_last_sync, $this->account->uid);
    while($nid = db_result($result)) {
      $tzreport = node_load($nid, NULL, TRUE);
      $begindate = tzbase_make_date($tzreport->begintime);
      watchdog('tzintellitime', 'Sending report with nid !nid to week !week',
        array('!nid' => $tzreport->nid, '!itid' => $tzreport->intellitime_id, '!week' => $begindate->format('o\WW')),
        WATCHDOG_NOTICE,
        l('Node', 'node/' . $tzreport->nid) . ', ' . theme('username', $this->account));
      // Convert and post report
      $itreport = new TZIntellitimeReport($tzreport);
      $data = $this->bot->update_report($itreport);
      $this->process_week_data($this->week_key($begindate), $data);
    }
  }

  /**
   * Create or update a tzreport from a TZIntellitimeReport
   * @param TZIntellitimeReport $intelli_report
   * @param Array $job_map as returned from convert_assignments
   */
  function create_report($intelli_report, $job_map = array()) {
    // Try to fetch previous report
    $tzreport = tzintellitime_node_load_by_intellitime_id($intelli_report->get_intellitime_id($this->account->uid));

    $original_tzreport = NULL;
    if($tzreport) {
      $original_tzreport = clone $tzreport;
    }

    $tzreport = $intelli_report->convert_to_tzreport($this->account, $tzreport);

    // Map job to report by searching for matching title
    $jobid = $this->match_job($job_map, $intelli_report->title);
    if ($jobid) {
      $tzreport->jobid = $jobid;
    } else {
      // Should not be able to create a report without a valid job.
      watchdog('tzintellitime', 'No match for "!title" in jobmap', array('!title' => $intelli_report->title), WATCHDOG_WARNING);
      return NULL;
    }

    if($tzreport != $original_tzreport) {
      $tzreport = $this->store_node($tzreport);
    }

    return $tzreport;
  }

  /**
   * Create or update a tzjob from a TZIntellitimeAssignment
   * @param TZIntellitimeAssignment $intelli_assignment
   * @param int $parentid optional parentid to set
   */
  function create_job($intelli_assignment, $parentid = 0) {
    // Load node if this is an update
    $job = tzintellitime_node_load_by_intellitime_id($intelli_assignment->id);

    // Clone job to be able to check for changes later
    $original_job = NULL;
    if($job) {
      $original_job = clone $job;
    }

    $job = $intelli_assignment->convert_to_tzjob($this->account, $job);
    $job->parentid = $parentid;

    if($job != $original_job) {
      $job = $this->store_node($job);
    }

    return $job;
  }

  /**
   * Does the actual work of storing a node to the drupal database.
   * @param Object $node_data actual node data to save.
   * @return Object Full drupal node object.
   */
  function store_node($node_data) {
    $node = node_submit($node_data);
    node_save($node);

    watchdog('tzintellitime_sync', '!operation !type !nid',
             array(
               '!operation' => (isset($node_data->nid) && $node_data->nid) ? 'Updated' : 'Inserted',
               '!type' => $node->type,
               '!nid' => $node->nid
             ),
             WATCHDOG_NOTICE,
             l('View', 'node/' . $node->nid));

    return $node;
  }

  static function match_job($jobmap, $jobtitle) {
    if(!empty($jobmap[$jobtitle]) && $jobmap[$jobtitle]) {
      return $jobmap[$jobtitle];
    }

    $jobpath = explode(', ', $jobtitle);
    foreach($jobmap as $key => $jobid) {
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
        return $jobid;
      }
    }

    return FALSE;
  }
}
