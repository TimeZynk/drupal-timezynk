<?php

class TZIntellitimeWeekSyncResult {
  /**
   * @var array TZReport array
   */
  public $tzreports;

  /**
   * @var array TZIntellitimeAssignment array
   */
  public $intellitime_assignments;

  /**
   *
   * @var array List of unfinished weeks in the format of i.e. 2011W02
   */
  public $unfinishedWeeks;

  /**
   * @var TZIntellitimeServerException set if any exception was caught during the synchronization
   */
  public $exception;

  public function getTZJobs() {
    $tzjobs = array();
    foreach ($this->intellitime_assignments as $a) {
      $tzjobs[] = $a->convert_to_tzjob();
    }
    return $tzjobs;
  }
}