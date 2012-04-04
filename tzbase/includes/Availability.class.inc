<?php

class Availability {
  const AVAILABLE = TZAvailabilityType::AVAILABLE;
  const NOT_AVAILABLE = TZAvailabilityType::NOT_AVAILABLE;
  const DELETED = 255;

  private $id;
  private $uid;
  private $type;
  private $start_time;
  private $end_time;
  private $local_changes = FALSE;
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
    return $this;
  }

  public function getUserId() {
    return $this->uid;
  }

  public function setStartTime($start_time) {
    $this->start_time = clone($start_time);
    return $this;
  }

  public function getStartTime() {
    if (empty($this->start_time)) {
      return NULL;
    }
    return clone($this->start_time);
  }

  public function setEndTime($end_time) {
    $this->end_time = clone($end_time);
    return $this;
  }

  public function getEndTime() {
    if (empty($this->end_time)) {
      return NULL;
    }
    return clone($this->end_time);
  }

  public function setType($type) {
    $this->type = $type;
    return $this;
  }

  public function getType() {
    return $this->type;
  }

  public function isAvailable() {
    return $this->type == Availability::AVAILABLE;
  }

  public function isDeleted() {
    return $this->type == Availability::DELETED;
  }

  public function setDBWrapper($dbWrapper) {
    $this->dbWrapper = $dbWrapper;
    return $this;
  }

  public function isOverlappingRange($from, $to) {
    $from_date = date_make_date($this->start_time->format('Y-m-d') . 'T' . $from);
    $to_date = date_make_date($this->start_time->format('Y-m-d') . 'T' . $to);
    if ($to_date < $from_date) {
      $to_date->modify('+1 day');
    }
    return $this->start_time < $to_date && $this->end_time > $from_date;
  }

  public function setLocalChanges($local_changes = TRUE) {
    $this->local_changes = $local_changes;
    return $this;
  }

  public function haveLocalChanges() {
    return $this->local_changes;
  }

  public function save() {
    $this->validate();
    $written_record = $this->dbWrapper->writeRecord('tzavailability', $this->getFields());
    $this->setFields($written_record);
    return $this;
  }

  public function validate() {
    $exception = new TZDBValidationException('validations failed in Availability');
    if (empty($this->uid)) {
      $exception->addError(t('Missing user ID'));
    }
    if (empty($this->start_time)) {
      $exception->addError(t('Missing start time'));
    }
    if (empty($this->end_time)) {
      $exception->addError(t('Missing end time'));
    }
    if ($this->end_time < $this->start_time) {
      $exception->addError(t('Inverted interval'));
    }
    if ($exception->hasErrors()) {
      throw $exception;
    }
    return $this;
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
    $this->local_changes = $record->local_changes != 0;
  }

  public function getFields() {
    $fields = array();
    if (!empty($this->id)) {
      $fields['id'] = $this->id;
    }
    $fields['uid'] = $this->uid;
    $fields['availability_type'] = $this->type;
    if (!empty($this->start_time)) {
      $fields['start_time'] = $this->start_time->format('U');
    }
    if (!empty($this->end_time)) {
      $fields['end_time'] = $this->end_time->format('U');
    }
    $fields['local_changes'] = $this->local_changes ? 1 : 0;
    return $fields;
  }
}
