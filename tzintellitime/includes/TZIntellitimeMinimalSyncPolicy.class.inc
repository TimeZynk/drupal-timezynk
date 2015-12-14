<?php

require_once dirname(__FILE__) . '/TZIntellitimeSimpleSyncPolicy.class.inc';

/**
  * Minimal synchronization policy designed to keep the session alive while
  * reducing server load.
  */
class TZIntellitimeMinimalSyncPolicy extends TZIntellitimeSimpleSyncPolicy {
  /**
   * @param object $account Drupal account object
   * @param DateTime $startDate Date for the first week to sync
   * @param int $nbrOfWeeksForward How many extra weeks to sync beyond the first week.
   */
  public function __construct($account, $startDate) {
    tzbase_include_proto_classes();

    $this->account = $account;

    // Clone so we don't suprise the caller...
    $date = clone($startDate);

    $this->timezone = $date->getTimezone();

    // Always sync this week
    $this->addWeek($date);
    $this->addUpdatedWeeks();
  }

  public function addWeeks($weeks) {
    return;
  }
}
