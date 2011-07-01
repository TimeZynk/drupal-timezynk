<?php

class Availability {
  private $id;
  private $uid;
  private $type;
  private $start_time;
  private $end_time;
  private $dbWrapper;

  function __construct($dbRow = NULL, $dbWrapper = NULL) {
    $this->setFields($dbRow);
    $this->dbWrapper = $dbWrapper;
  }

  function getId() {
    return $this->id;
  }

  public function setUserId($uid) {
    $this->uid = $uid;
  }

  public function getUserId() {
    return $this->uid;
  }

  public function setStartTime($start_time) {
    $this->start_time = clone($start_time);
  }

  public function getStartTime() {
    if (empty($this->start_time)) {
      return NULL;
    }
    return clone($this->start_time);
  }

  public function setEndTime($end_time) {
    $this->end_time = clone($end_time);
  }

  public function getEndTime() {
    if (empty($this->end_time)) {
      return NULL;
    }
    return clone($this->end_time);
  }

  public function isAvailable() {
    return $this->type == TZAvailabilityType::AVAILABLE;
  }

  public function setDBWrapper($dbWrapper) {
    $this->dbWrapper = $dbWrapper;
  }

  private function setFields($record) {
    if (empty($record)) {
      return;
    }

    $record = (object) $record;
    $this->id = $record->id;
    $this->uid = $record->uid;
    $this->type = $record->availability_type;
    if (!empty($record->start_time)) {
      $this->start_time = tzbase_make_date($record->start_time);
    }
    if (!empty($record->end_time)) {
      $this->end_time = tzbase_make_date($record->end_time);
    }
  }
}