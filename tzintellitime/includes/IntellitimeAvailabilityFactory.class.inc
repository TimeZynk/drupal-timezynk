<?php
class IntellitimeAvailabilityFactory {
  /**
   * @var AvailabilityStore
   */
  private $store;
  /**
   * @var Array
   */
  private $day_range;
  /**
   * @var Array
   */
  private $evening_range;
    /**
   * @var Array
   */
  private $night_range;

  public function __construct($availability_store, $day_range_text, $evening_range_text, $night_range_text) {
    $this->store = $availability_store;
    $this->day_range = $this->makeRange($day_range_text);
    $this->evening_range = $this->makeRange($evening_range_text);
    $this->night_range = $this->makeRange($night_range_text);
  }

  /**
   * Creates an IntellitimeAvailability from a regular Availability.
   * @param Availability $availability
   * @return IntellitimeAvailability the created availability.
   */
  public function createIntellitimeAvailability($availability) {
    $ia = new IntellitimeAvailability($availability->getStartTime());
    if ($availability->isAvailable()) {
      $ia->setDay($availability->isOverlappingRange($this->day_range['start'], $this->day_range['end']));
      $ia->setEvening($availability->isOverlappingRange($this->evening_range['start'], $this->evening_range['end']));
      $ia->setNight($availability->isOverlappingRange($this->night_range['start'], $this->night_range['end']));
    }
    if ($availability->haveLocalChanges()) {
      $ia->setLocalChanges();
    }
    return $ia;
  }

  /**
   * Creates Availability from IntellitimeAvailability
   * @param IntellitimeAvailability $ia
   * @return Array of Availability objects
   */
  public function createAvailabilities($ia) {
    $result = array();

    $date_string = $ia->getDate()->format('Y-m-d');
    // Convert the three possible areas into a sequence of timestamps
    $timestamps = array();
    if ($ia->isAvailableDuringDay()) {
      $timestamps = array_merge($timestamps, $this->timestampRange($date_string, $this->day_range));
    }
    if ($ia->isAvailableDuringEvening()) {
      $timestamps = array_merge($timestamps, $this->timestampRange($date_string, $this->evening_range));
    }
    if ($ia->isAvailableDuringNight()) {
      $timestamps = array_merge($timestamps, $this->timestampRange($date_string, $this->night_range));
    }

    // Build histogram of timestamps
    $ranges = array();
    foreach ($timestamps as $timestamp) {
      if (empty($ranges[$timestamp])) {
        $ranges[$timestamp] = 1;
      } else {
        $ranges[$timestamp] += 1;
      }
    }
    // Only keep timestamps that have occurred only once
    $ranges = array_keys($ranges, 1, TRUE);
    sort($ranges);

    $a = NULL;
    foreach ($ranges as $timestamp) {
      if ($a) {
        $a->setEndTime(tzbase_make_date($timestamp));
        $result[] = $a;
        $a = NULL;
      } else {
        $a = $this->store->createAvailability()
                         ->setStartTime(tzbase_make_date($timestamp))
                         ->setType(Availability::AVAILABLE);
      }
    }

    if ($a) {
      throw new InvalidArgumentException('Could not match intellitime availability to availabilities');
    }

    return $result;
  }

  private function timestampRange($date_string, $range) {
    $date_string = $date_string . 'T';
    $from_date = date_make_date($date_string . $range['start']);
    $to_date = date_make_date($date_string . $range['end']);
    if ($to_date < $from_date) {
      $to_date->modify('+1 day');
    }
    return array(
      $from_date->format('U'),
      $to_date->format('U'),
    );
  }

  private function makeRange($range_text) {
    $range_parts = explode('-', $range_text);
    return array_combine(array('start', 'end'), $range_parts);
  }
}