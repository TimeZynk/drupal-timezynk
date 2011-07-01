<?php

class AvailabilityStore {
  /**
   * @var TZDBWrapper
   */
  private $dbWrapper;

  function __construct($dbWrapper) {
    $this->dbWrapper = $dbWrapper;
  }

  function findByDate($date) {
    $from = $this->dateToTimeStamp($date, 0, 0, 0);
    $to = $this->dateToTimeStamp($date, 23, 59, 59);

    $availability = array();
    $query = 'SELECT * FROM {tzavailability} WHERE (start_time >= %d AND start_time <= %d) OR (end_time > %d AND end_time <= %d) ORDER BY start_time';
    $result = db_query($query, $from, $to, $from, $to);
    while ($row = db_fetch_object($result)) {
      $availability[] = new Availability($row, $this->dbWrapper);
    }
    return $availability;
  }

  function findByTimeInterval($from, $to) {
    $availability = array();
    $query = 'SELECT * FROM {tzavailability} WHERE start_time <= %d AND end_time >= %d ORDER BY start_time';
    $result = db_query($query, $from->format('U'), $to->format('U'));
    while ($row = db_fetch_object($result)) {
      $availability[] = new Availability($row, $this->dbWrapper);
    }
    return $availability;
  }

  private function dateToTimeStamp($date, $hour, $minute, $second) {
    $new_date = clone($date);
    $new_date->setTime($hour, $minute, $second);
    return $new_date->format('U');
  }
}