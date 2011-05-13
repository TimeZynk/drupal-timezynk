<?php

class TZUserStatus {
  const GREY = -10;
  const RED = 0;
  const YELLOW = 10;
  const GREEN = 20;

  /**
   * Unknown number of reports
   */
  const NUMBER_OF_REPORTS_UNKNOWN = -2;
  const NUMBER_OF_REPORTS_NOT_SET = -1;

  private $uid;
  private $now;
  private $lastLogin;
  private $loginLimit;
  private $numberOfDueReports = -1;

  function __construct($uid, $now, $lastLogin, $loginLimit) {
    $this->uid = $uid;
    $this->now = $now;
    $this->lastLogin = $lastLogin;
    if (!is_int($loginLimit)) {
      throw new InvalidArgumentException('loginLimit must be an integer in TZUserStatus');
    }
    $this->loginLimit = $loginLimit;
  }

  function setNumberOfDueReports($count) {
    if(!is_int($count) || $count < 0) {
      throw new InvalidArgumentException('$count must be a positive integer');
    }
    $this->numberOfDueReports = $count;
  }

  function getUid() {
    return $this->uid;
  }

  function getStatusTimeStamp() {
    return $this->now;
  }

  function getLastLogin() {
    return $this->lastLogin;
  }

  function getLoginLimit() {
    return $this->loginLimit;
  }

  function getNumberOfDueReports() {
    return $this->numberOfDueReports;
  }

  function getStatusCode() {
    if (empty($this->lastLogin)) {
      return TZUserStatus::GREY;
    }
    if ($this->lastLogin <= $this->now - $this->loginLimit) {
      return TZUserStatus::RED;
    }
    if ($this->numberOfDueReports === 0) {
      return TZUserStatus::GREEN;
    }
    return TZUserStatus::YELLOW;
  }
}
