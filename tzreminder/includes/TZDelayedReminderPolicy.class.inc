<?php

tzbase_include_proto_classes();

class TZDelayedReminderPolicy extends TZAbstractReminderPolicy {

  /**
   *
   * @var int how much the report may be overdue by.
   */
  private $minutes = 0;

  /**
   * Constructs a new delayed report reminder policy, that will generate a
   * reminder when a report is overdue by more than $minutes minutes.
   *
   * @param integer $minutes the delay in minutes.
   */
  function __construct($minutes) {
    if (empty($minutes) || $minutes < 0) {
      throw new InvalidArgumentException('$minutes must be 1 or larger');
    }
    $this->minutes = $minutes;
  }

  public function getQuery($time) {
    $queryBuilder = new TZReportQueryBuilder(TZFlags::REPORTED);
    $endtime = clone($time);
    $endtime->modify("-$this->minutes minute");
    $queryBuilder->setEndtimeBefore($endtime);
    return $queryBuilder->build();
  }

  public function shouldSendMessage($timeNow, $timeLastCalled) {
    if (empty($timeNow) || empty($timeLastCalled)) {
      throw new InvalidArgumentException('You must specify when the policy was last invoked and what the time is now.');
    }
    $lastCalled = clone($timeLastCalled);
    $lastCalled->modify("+$this->minutes minutes");
    return $timeNow >= $lastCalled;
  }

  public function getName() {
    return "delay";
  }
}
