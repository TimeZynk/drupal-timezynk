<?php

class TZUserStatus {
  const GREY = -10;
  const RED = 0;
  const YELLOW = 10;
  const GREEN = 20;

  private $lastLogin;
  private $loginLimit;
  private $numberOfDueReports = -1;

  function __construct($lastLogin, $loginLimit) {
    $this->lastLogin = $lastLogin;
    $this->loginLimit = $loginLimit;
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
    if (empty($this->lastLogin)) {
      return TZUserStatus::GREY;
    }
    if (is_numeric($this->loginLimit) && $this->lastLogin <= $now - $this->loginLimit) {
      return TZUserStatus::RED;
    }
    if ($this->numberOfDueReports === 0) {
      return TZUserStatus::GREEN;
    }
    return TZUserStatus::YELLOW;
  }
}
