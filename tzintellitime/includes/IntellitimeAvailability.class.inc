<?php

class IntellitimeAvailability {
  /**
   * @var DateTime
   */
  private $date;

  /**
   * Models an availability list for a given date in intellitime.
   */
  public function __construct() {
  }


  function setDate($date) {
    $this->date = clone $date;
  }

  function getDate() {
    return clone $this->date;
  }

}