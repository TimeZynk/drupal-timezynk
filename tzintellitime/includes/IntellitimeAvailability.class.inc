<?php

class IntellitimeAvailability {
  /**
   * @var DateTime
   */
  private $date;

  /**
   * @var String
   */
  private $formId;

  private $dayAvailable = FALSE;
  private $eveningAvailable = FALSE;
  private $nightAvailable = FALSE;
  private $localChanges = FALSE;

  /**
   * Models an availability list for a given date in intellitime.
   */
  public function __construct($date = NULL, $formId = '') {
    $this->date = isset($date) ? clone $date : NULL;
    $this->formId = $formId;
  }

  function setDate($date) {
    $this->date = clone $date;
    return $this;
  }

  function getDate() {
    return clone $this->date;
  }

  function setFormId($formId) {
    $this->formId = $formId;
    return $this;
  }

  function getFormId() {
    return $this->formId;
  }

  function setDay($isAvailable) {
    $this->dayAvailable = $isAvailable;
    return $this;
  }

  function isAvailableDuringDay() {
    return $this->dayAvailable;
  }

  function setEvening($isAvailable) {
    $this->eveningAvailable = $isAvailable;
    return $this;
  }

  function isAvailableDuringEvening() {
    return $this->eveningAvailable;
  }

  function setNight($isAvailable) {
    $this->nightAvailable = $isAvailable;
    return $this;
  }

  function isAvailableDuringNight() {
    return $this->nightAvailable;
  }

  function setLocalChanges() {
    $this->localChanges = TRUE;
    return $this;
  }

  function haveLocalChanges() {
    return $this->localChanges;
  }
}