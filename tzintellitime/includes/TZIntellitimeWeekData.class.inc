<?php

class TZIntellitimeErrorPageException extends TZIntellitimeServerException {}

class TZIntellitimeWeekData {

  /**
   * @var TZIntellitimeParser
   */
  private $parser;
  private $rows = array();
  private $tzjobs;

  /**
   * @param TZIntellitimeParser $parser
   */
  function __construct($parser) {
    if(empty($parser)) {
      throw new InvalidArgumentException('Must supply valid TZIntellitimeParser object');
    }

    $this->parser = $parser;

    $error = $parser->parse_page_error();
    if($error) {
      throw new TZIntellitimeErrorPageException($error, (string)$parser);
    }

    if(!$parser->is_valid_login()) {
      throw new TZAuthenticationFailureException('Session expired', (string)$parser);
    }

    $reports = $parser->parse_reports();
    foreach($reports as $report) {
      $this->rows[$report->id] = $report;
    }

    $assignments = array_merge(
        $parser->parse_assignments(),
        $parser->parse_absence_types()
    );

    $this->tzjobs = $this->createTZJobs($reports, $assignments);
  }

  /**
   * @return array TZIntellitimeAssignment array
   */
  function getTZJobs() {
    return array_values($this->tzjobs);
  }

  /**
   * @return array List of unfinished weeks in the format of i.e. 2011W02
   */
  function getUnfinishedWeeks() {
    return $this->parser->parse_unfinished_weeks();
  }

  /**
   * Build a TZPostData object that make the intellitime state one step
   * closer to the state specified in the $tzreports array.
   *
   * @param array $tzreports TZIntellitimeReport array
   * @return TZPostData
   */
  function buildPost(array $tzreports) {
    $postData = NULL;

    $allReportsDone = TRUE;
    $unlockImmutable = FALSE;
    $needsUpdating = array();
    $needsUnlocking = array();
    $seenReportIDs = array();
    foreach($tzreports as $tzreport) {
      $row = $this->findReportRow($tzreport);
      if(empty($row)) {
        continue;
      }

      $seenReportIDs[$row->id] = TRUE;

      if($tzreport->flags == TZFlags::CREATED) {
        $allReportsDone = FALSE;
      }

      $itreport = new TZIntellitimeReport($tzreport);

      if($tzreport->intellitime_local_changes) {
        $needsUpdating[] = $itreport;

        if($row->state == TZIntellitimeReport::STATE_REPORTED) {
          $needsUnlocking[] = $itreport;
          if($row->stateImmutable) {
            $unlockImmutable = TRUE;
          }
        }
      } else if($this->stateChangedAndCanModify($itreport, $row)) {
        $needsUpdating[] = $itreport;
      }
    }

    /* The code above has checked if the reports stored in our database are
     * all done. However, to be able to say that everything is done we must
     * also check any new rows on the page and see if they are done.
     */
    $allReportsDone = $allReportsDone && $this->checkIfReportRowsDone($seenReportIDs);

    if(!empty($needsUnlocking)) {
      $postData = new TZIntellitimePostData();
      $postData->setPostAction('TimeReport/' . $this->parser->parse_form_action());
      $postData->setPostData($this->parser->build_unlock_post($needsUnlocking, $unlockImmutable));
      $postData->setIntermediate(TRUE);
    } elseif (!empty($needsUpdating)) {
      $postData = new TZIntellitimePostData();
      $postData->setPostAction('TimeReport/' . $this->parser->parse_form_action());
      $postData->setPostData($this->parser->build_update_reports_post($needsUpdating, $allReportsDone));
    }

    return $postData;
  }

  /**
   * Correlates the page contents and the $tzreports array to create
   * a new array that contains a updated version with new values and
   * any new reports. Reports that no longer exist on the page are
   * set to DELETED.
   *
   * @param array $tzreports
   * @param object $account
   * @param boolean $finalPost Set to overwrite even locally changed reports
   * @return array $tzreports with updates and additions
   */
  public function updateReports($tzreports, $account, $finalPost = FALSE) {
    $updatedReports = $this->buildReportMapWithDeletionsMarked($tzreports);

    foreach($this->rows as $intellitime_id => $row) {
      if(isset($updatedReports[$intellitime_id])) {
        $tzreport = $updatedReports[$intellitime_id];
        /* Reports without local changes or where the server state has moved to one
         * of the immutable states needs to be overwritten with the server data.
         *
         * If $finalPost is set, this is the final version after any needed
         * POSTs and we should therefore update ALL reports, regardless of
         * local changes.
         */
        if(!$tzreport->intellitime_local_changes ||
            $row->state > TZIntellitimeReport::STATE_REPORTED ||
            $finalPost) {
          $tzreport = $row->convert_to_tzreport($account, $tzreport);
        }
      } else {
        // New report from Intellitime
        $updatedReports[$intellitime_id] = $row->convert_to_tzreport($account);
      }
    }

    if($finalPost) {
      $this->mapIntellitimeJobIdToReport($updatedReports);
    }

    return array_values($updatedReports);
  }



  /**
   * Creates a new array where the reports in $tzreports have updated
   * intellitime_last_state fields according to what was found on the
   * page. It is necessary to call this method after completing an
   * "intermediate"-post to avoid regressions in the report state after
   * the final post.
   *
   * @param type $tzreports
   * @return array TZReport array
   */
  public function updateLastStates($tzreports) {
    $updatedReports = array();
    foreach($tzreports as $tzreport) {
      $row = $this->findReportRow($tzreport);
      if(empty($row)) {
        $updatedReports[] = $tzreport;
      } else {
        $tzreport_clone = clone($tzreport);
        $tzreport_clone->intellitime_last_state = $row->state;
        $updatedReports[] = $tzreport_clone;
      }
    }
    return $updatedReports;
  }

  /**
   * Builds a array map using intellitime_id as keys and the
   * tzreports as values. Any reports not found on the page
   * are marked as DELETED.
   *
   * @param array $tzreports
   * @return array TZReport->intellitime_id => TZReport
   */
  private function buildReportMapWithDeletionsMarked($tzreports) {
    $updatedReports = array();

    foreach($tzreports as $tzreport) {
      $row = $this->findReportRow($tzreport);
      $tzreport_clone = clone($tzreport);
      if(empty($row)) {
        $tzreport_clone->flags = TZFlags::DELETED;
        $tzreport_clone->intellitime_local_changes = 0;
      }
      $updatedReports[tzintellitime_get_intellitime_id($tzreport)] = $tzreport_clone;
    }

    return $updatedReports;
  }

  private function mapIntellitimeJobIdToReport($tzreports) {
    foreach($tzreports as $tzreport) {
      $report_key = $tzreport->title;
      if(isset($this->tzjobs[$report_key])) {
        $tzreport->intellitime_jobid = $this->tzjobs[$report_key]->intellitime_id;
      }
    }
  }

  private function stateChangedAndCanModify($itreport, $row) {
    return $itreport->state != $row->state && !$row->stateImmutable;
  }

  private function checkIfReportRowsDone($skipReportIDs) {
    foreach($this->rows as $intellitime_id => $itreport) {
      if(!empty($skipReportIDs[$intellitime_id])) {
        continue;
      }
      if($itreport->state == TZIntellitimeReport::STATE_OPEN) {
        return FALSE;
      }
    }
    return TRUE;
  }



  private function findReportRow($tzreport) {
    $id = tzintellitime_get_intellitime_id($tzreport);
    if(isset($this->rows[$id])) {
      return $this->rows[$id];
    }
    return NULL;
  }

  private function createTZJobs($reports, $assignments) {
    $tzjobs = array();

    foreach($reports as $report) {
      $title = $report->title;

      $assignment = $this->matchAssignmentTitle($assignments, $title);
      if($assignment) {
        $assignment->report_key = $title;
      } else {
        $assignment = new TZIntellitimeAssignment($title);
      }

      $tzjobs[$assignment->report_key] = $assignment->convert_to_tzjob();
    }

    return $tzjobs;
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
  private function matchAssignmentTitle($assignments, $title) {
    foreach($assignments as $assignment) {
      if($title == $assignment->title) {
        return $assignment;
      }

      if($title == $assignment->report_key) {
        return $assignment;
      }

      $match = TRUE;
      $keypath = explode(', ', $assignment->title);
      $jobpath = explode(', ', $title);
      if(count($jobpath) != count($keypath)) {
        continue;
      }
      for($i = 0; $i < count($jobpath); $i++) {
        $keysegment = $keypath[$i];
        $job = $jobpath[$i];
        $segment = substr($job, 0, strlen($keysegment));
        if($keysegment != $segment) {
          $match = FALSE;
          break;
        }
      }
      if($match) {
        return $assignment;
      }
    }

    return NULL;
  }
}
