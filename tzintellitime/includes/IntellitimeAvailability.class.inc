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

  /**
   * Models an availability list for a given date in intellitime.
   */
  public function __construct($date = NULL, $formId = '') {
    $this->date = $date;
    $this->formId = $formId;
  }

  function setDate($date) {
    $this->date = clone $date;
  }

  function getDate() {
    return clone $this->date;
  }

  function setFormId($formId) {
    $this->formId = $formId;
  }

  function getFormId() {
    return $this->formId;
  }

  function setDay($isAvailable) {
    $this->dayAvailable = $isAvailable;
  }

  function isAvailableDuringDay() {
    return $this->dayAvailable;
  }

  function setEvening($isAvailable) {
    $this->eveningAvailable = $isAvailable;
  }

  function isAvailableDuringEvening() {
    return $this->eveningAvailable;
  }

  function setNight($isAvailable) {
    $this->nightAvailable = $isAvailable;
  }

  function isAvailableDuringNight() {
    return $this->nightAvailable;
  }
}