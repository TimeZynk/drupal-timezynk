<?php

class TZUserStatus {
  const RED = 0;
  const YELLOW = 10;
  const GREEN = 20;

  private $lastLogin;
  private $dueLimit;
  private $numberOfDueReports = -1;
  private $earliestDueEndTime = 0;

  function __construct($lastLogin, $dueLimit) {
    $this->lastLogin = $lastLogin;
    $this->dueLimit = $dueLimit;
  }

  function setEarliestDueEndTime($endtime) {
    $this->earliestDueEndTime = $endtime;
  }

  function setNumberOfDueReports($count) {
    if(!is_int($count) || $count < 0) {
      throw new InvalidArgumentException('$count must be a positive integer');
    }
    $this->numberOfDueReports = $count;
  }

  function getNumberOfDueReports() {
    return $this->numberOfDueReports;
  }

  function getStatusCode($now) {
    if(empty($this->lastLogin)) {
      return TZUserStatus::RED;
    }
    if($this->numberOfDueReports === 0) {
      return TZUserStatus::GREEN;
    }
    if($this->earliestDueEndTime >= $now - $this->dueLimit) {
      return TZUserStatus::GREEN;
    }
    return TZUserStatus::YELLOW;
  }
}
