<?php

class TZIntellitimeWeekData {

  /**
   * @var IntellitimeWeekPage
   */
  private $page;
  private $rows = array();
  private $assignments;

  /**
   * @param IntellitimeWeekPage $page
   */
  function __construct($page) {
    if(empty($page)) {
      throw new InvalidArgumentException('Must supply valid IntellitimeWeekPage');
    }

    $this->page = $page;

    $reports = $page->getReports();
    foreach($reports as $report) {
      $this->rows[$report->id] = $report;
    }

    $this->assignments = $page->getAssignments();
    $this->mapReportTitleToAssignments($reports);
  }

  /**
   * @return array TZIntellitimeAssignment array
   */
  function getAssignments() {
    return $this->assignments;
  }

  /**
   * @return array List of unfinished weeks in the format of i.e. 2011W02
   */
  function getUnfinishedWeeks() {
    return $this->page->getUnfinishedWeeks();
  }

  /**
   * Build a TZPostData object that make the intellitime state one step
   * closer to the state specified in the $tzreports array.
   *
   * @param array $tzreports TZIntellitimeReport array
   * @return TZPostData
   */
  function buildPost(array $tzreports) {
    $allReportsDone = TRUE;
    $unlockImmutable = FALSE;
    $needsUpdating = array();
    $needsUnlocking = array();
    $needsInserting = array();
    $seenReportIDs = array();
    foreach($tzreports as $tzreport) {
      if (empty($tzreport->intellitime_id) &&
          $tzreport->flags < TZFlags::DELETED &&
          !empty($tzreport->intellitime_jobid)) {
        $needsInserting[] = $tzreport;
        continue;
      }

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

    $post = NULL;
    if(!empty($needsUnlocking)) {
      $post = $this->page->getUnlockPost($needsUnlocking, $unlockImmutable);
    }

    if (!$post && !empty($needsUpdating)) {
      $post = $this->page->getUpdatePost($needsUpdating, $allReportsDone);
    }

    if (!$post && !empty($needsInserting)) {
      $tzreport = $needsInserting[0];
      $itreport = new TZIntellitimeReport($tzreport);
      $post = $this->page->getInsertPost($tzreport->nid, $itreport);
    }

    return $post;
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
          $updatedReports[$intellitime_id] = $row->convert_to_tzreport($account, $tzreport);
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

  public function postProcessPost($post, $updatedReports, $account) {
    if ($post instanceof IntellitimeWeekInsertPost) {
      return $this->updateAfterAddReportPost($post, $updatedReports, $account);
    } else {
      return $this->updateLastStates($updatedReports);
    }
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
  private function updateLastStates($tzreports) {
    $updatedReports = array();
    foreach($tzreports as $tzreport) {
      if (empty($tzreport->intellitime_id)) {
        $updatedReports[] = $tzreport;
      } else {
        $row = $this->findReportRow($tzreport);
        if(empty($row)) {
          $updatedReports[] = $tzreport;
        } else {
          $tzreport_clone = clone($tzreport);
          $tzreport_clone->intellitime_last_state = $row->state;
          $updatedReports[] = $tzreport_clone;
        }
      }
    }
    return $updatedReports;
  }

  /**
   * Updates updatedReports array after an attempted row creation
   * @param IntellitimeWeekInsertPost $post
   * @param Array $tzreports
   * @param stdClass $account
   * @return array TZReport array
   */
  private function updateAfterAddReportPost($post, $tzreports, $account) {
    $nid = $post->getNid();
    $updatedReports = array();
    $newlyAddedReport = NULL;
    foreach ($tzreports as $tzreport) {
      $row = $this->findReportRow($tzreport);
      if ($row) {
        $row->visitedByUpdateAfterAddReportPost = TRUE;
      }

      if (empty($tzreport->nid) || $tzreport->nid != $nid) {
        $updatedReports[] = $tzreport;
      } else {
        $newlyAddedReport = $tzreport;
      }
    }

    $potential_rows = array();
    foreach($this->rows as $row) {
      if (!empty($row->visitedByUpdateAfterAddReportPost)) {
        continue;
      }
      $potential_rows[] = $row;
    }

    if (count($potential_rows) == 1) {
      $mappedReport = $potential_rows[0]->convert_to_tzreport($account, $newlyAddedReport);
      $mappedReport->flags = $newlyAddedReport->flags;
      $mappedReport->intellitime_local_changes = TRUE;
      $updatedReports[] = $mappedReport;
    } else {
      /* Several rows created from a single post. Dangerous ground, try to escape by
       * removing the original report and overwriting it with what intellitime returned */
      $tzreport_clone = clone($newlyAddedReport);
      $tzreport_clone->flags = TZFlags::DELETED;
      $tzreport_clone->intellitime_local_changes = 0;
      $updatedReports[] = $tzreport_clone;
    }

    return $updatedReports;
  }

  /**
   * Builds a array map using intellitime_id as keys and the
   * tzreports as values. Any reports not found on the page
   * are marked as DELETED.
   *
   * @param array $tzreports
   * @param boolean $finalPost Set to delete all reports not found on server
   * @return array TZReport->intellitime_id => TZReport
   */
  private function buildReportMapWithDeletionsMarked($tzreports) {
    $updatedReports = array();

    foreach($tzreports as $tzreport) {
      $row = $this->findReportRow($tzreport);
      if(empty($row) && !empty($tzreport->intellitime_id)) {
        $tzreport = clone($tzreport);
        $tzreport->flags = TZFlags::DELETED;
        $tzreport->intellitime_local_changes = 0;
      }
      $updatedReports[tzintellitime_get_intellitime_id($tzreport)] = $tzreport;
    }

    return $updatedReports;
  }



  private function mapIntellitimeJobIdToReport($tzreports) {
    foreach($tzreports as $tzreport) {
      $report_key = $tzreport->title;
      if(isset($this->reportTitleAssignmentMap[$report_key])) {
        $tzreport->intellitime_jobid = $this->reportTitleAssignmentMap[$report_key]->id;
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

  private function mapReportTitleToAssignments($reports) {
    $this->reportTitleAssignmentMap = array();
    foreach($reports as $report) {
      $full_title = $report->title;

      if (!empty($this->reportTitleAssignmentMap[$full_title])) {
        continue;
      }

      $assignment = $this->matchAssignmentTitle($full_title);
      if(!$assignment) {
        $assignment = new TZIntellitimeAssignment($full_title);
        $this->assignments[] = $assignment;
      }

      $this->reportTitleAssignmentMap[$full_title] = $assignment;
    }
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
  private function matchAssignmentTitle($title) {
    foreach($this->assignments as $assignment) {
      if($assignment->matchFullTitle($title)) {
        return $assignment;
      }
    }
    return NULL;
  }
}
