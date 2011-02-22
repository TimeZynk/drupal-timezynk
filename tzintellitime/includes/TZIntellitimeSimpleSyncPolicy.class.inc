<?php

class TZIntellitimeSimpleSyncPolicy implements TZIntellitimeSyncPolicy {
  const PENDING = 0;
  const SYNCED = 1;

  private $weekStatus = array();
  private $account;
  private $timezone;

  /**
   * @param object $account Drupal account object
   * @param DateTime $startDate Date for the first week to sync
   * @param int $nbrOfWeeksForward How many extra weeks to sync beyond the first week.
   */
  public function __construct($account, $startDate, $nbrOfWeeksForward = 0, $nbrOfWeeksBackward = 0) {
    tzbase_include_proto_classes();

    $this->account = $account;

    // Clone so we don't suprise the caller...
    $date = clone($startDate);

    $this->timezone = $date->getTimezone();

    // Always sync this week
    $this->addWeek($date);

    // Sync weeks forward in time according to current configuration
    for ($i = 0; $i < $nbrOfWeeksForward; $i++) {
      $date->modify('+7 days');
      $this->addWeek($date);
    }

    // Sync weeks backward in time according to current configuration
    $date = clone($startDate);
    for ($i = 0; $i < $nbrOfWeeksBackward; $i++) {
      $date->modify('-7 days');
      $this->addWeek($date);
    }

    $this->addLocalUnlockedWeeks();
    $this->addUpdatedWeeks();
  }

  public function addWeeks($weeks) {
    foreach($weeks as $weekDate) {
      $this->addWeek($weekDate);
    }
  }

  public function getNextWeekToSync() {
    $weekString = array_search(TZIntellitimeSimpleSyncPolicy::PENDING, $this->weekStatus);
    if($weekString === FALSE) {
      return NULL;
    }

    $this->weekStatus[$weekString] = TZIntellitimeSimpleSyncPolicy::SYNCED;
    $weekDate = new DateTime($weekString, $this->timezone);
    return $weekDate;
  }

  private function addWeek($dateTime) {
    $key = $dateTime->format('o\WW');
    if(!isset($this->weekStatus[$key])) {
      $this->weekStatus[$key] = TZIntellitimeSimpleSyncPolicy::PENDING;
    }
  }

  /**
   * Add weeks where we have local unfinished reports.
   */
  private function addLocalUnlockedWeeks() {
    $query = db_query('SELECT t.begintime FROM {node} n INNER JOIN {tzreport} t ON n.vid = t.vid WHERE t.assignedto = %d AND t.flags <= %d',
        $this->account->uid, TZFlags::REPORTED);
    while($time = db_result($query)) {
      $this->addWeek(tzbase_make_date($time));
    }
  }

  /**
   * Add weeks where we have updated reports as pending for sync.
   */
  private function addUpdatedWeeks() {
    $query = db_query('SELECT begintime FROM {tzreport} tzreport INNER JOIN {tzintellitime} tzintellitime ON tzreport.vid = tzintellitime.vid WHERE tzreport.assignedto = %d AND tzintellitime.intellitime_local_changes > 0', $this->account->uid);
    while($time = db_result($query)) {
      $this->addWeek(tzbase_make_date($time));
    }
  }
}
