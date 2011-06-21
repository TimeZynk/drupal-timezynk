<?php

class TZUserJobMapping {
  private $id;
  private $uid;
  private $jobid;
  private $start_time;
  private $end_time;
  private $dbWrapper;

  function __construct($dbRow = NULL) {
    $this->setFields($dbRow);
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

  public function setJobId($jobid) {
    $this->jobid = $jobid;
  }

  public function getJobId() {
    return $this->jobid;
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

  public function setDBWrapper($dbWrapper) {
    $this->dbWrapper = $dbWrapper;
  }

  public function mayCreateReport($uid, $jobid, $begintime) {
    if ($uid != $this->uid || $jobid != $this->jobid) {
      return FALSE;
    }

    if (!empty($this->start_time)) {
      $start_timestamp = $this->start_time->format('U');
      if ($begintime < $start_timestamp) {
        return FALSE;
      }
    }

    if (!empty($this->end_time)) {
      $end_timestamp = $this->end_time->format('U');
      if ($begintime > $end_timestamp) {
        return FALSE;
      }
    }

    return TRUE;
  }

  public function save() {
    $this->validate();
    $written_record = $this->dbWrapper->writeRecord('tzusers_tzjobs', $this->getFields());
    $this->setFields($written_record);
  }

  public function validate() {
    $exception = new TZDBValidationException('validations failed in TZUserJobMapping');
    if (empty($this->uid)) {
      $exception->addError(t('Missing user ID'));
    }
    if (empty($this->jobid)) {
      $exception->addError(t('Missing job ID'));
    }
    if (!empty($this->start_time) && !empty($this->end_time) && $this->end_time < $this->start_time) {
      $exception->addError(t('Inverted interval'));
    }
    if ($exception->hasErrors()) {
      throw $exception;
    }
  }

  private function getFields() {
    $fields = array();
    if (!empty($this->id)) {
      $fields['id'] = $this->id;
    }
    $fields['uid'] = $this->uid;
    $fields['jobid'] = $this->jobid;
    if (!empty($this->start_time)) {
      $fields['start_time'] = $this->start_time->format('U');
    }
    if (!empty($this->end_time)) {
      $fields['end_time'] = $this->end_time->format('U');
    }
    return $fields;
  }

  private function setFields($record) {
    if (!empty($record)) {
      $record = (object) $record;
      $this->id = $record->id;
      $this->uid = $record->uid;
      $this->jobid = $record->jobid;
      if (!empty($record->start_time)) {
        $this->start_time = tzbase_make_date($record->start_time);
      }
      if (!empty($record->end_time)) {
        $this->end_time = tzbase_make_date($record->end_time);
      }
    }
  }
}