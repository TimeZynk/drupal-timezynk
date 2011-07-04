<?php

class AvailabilityStore {
  /**
   * @var TZDBWrapper
   */
  private $dbWrapper;

  function __construct($dbWrapper) {
    $this->dbWrapper = $dbWrapper;
  }

  function find($id) {
    $result = db_query("SELECT * from {tzavailability} WHERE id = %d", $id);
    $row = db_fetch_object($result);
    if (empty($row)) {
      return NULL;
    }
    return new Availability($row, $this->dbWrapper);
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

  function findBetween($uid, $start, $end) {
    $filter_strings = array();
    $filter_args = array();

    $filter_strings[] = 'uid = %d';
    $filter_args[] = $uid;

    if ($start) {
      $filter_strings[] = 'start_time >= %d';
      $filter_args[] = $start->format('U');
    }

    if ($end) {
      $filter_strings[] = 'start_time < %d';
      $filter_args[] = $end->format('U');
    }
    $filter_string = implode(' AND ', $filter_strings);

    $availabilities = array();
    $query = 'SELECT * FROM {tzavailability} WHERE ' . $filter_string;
    $result = db_query($query, $filter_args);
    while ($row = db_fetch_object($result)) {
      $availabilities[] = new Availability($row, $this->dbWrapper);
    }

    return $availabilities;
  }

  function delete($uid, $id) {
    $query = 'DELETE FROM {tzavailability} WHERE id = %d AND uid = %d';
    db_query($query, $id, $uid);
    return db_affected_rows();
  }

  function createAvailability() {
    return new Availability(NULL, $this->dbWrapper);
  }

  private function dateToTimeStamp($date, $hour, $minute, $second) {
    $new_date = clone($date);
    $new_date->setTime($hour, $minute, $second);
    return $new_date->format('U');
  }
}