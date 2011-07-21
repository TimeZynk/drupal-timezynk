<?php

class IntellitimeAvailabilityFactoryTest extends PHPUnit_Framework_TestCase {

  public function setUp() {
    $day_range = $this->createRange('08:00', '12:00');
    $evening_range = $this->createRange('12:00', '18:00');
    $night_range = $this->createRange('18:00', '00:00');
    $this->factory = new IntellitimeAvailabilityFactory($day_range, $evening_range, $night_range);
  }

  public function testOutsideDayEveningNightRangesReturnsNothing() {
    $start_time = date_make_date('2011-07-14T00:00');
    $end_time = date_make_date('2011-07-14T08:00');
    $intellitimeAvailability = $this->factory->create($this->buildAvailability($start_time, $end_time));
    $this->assertFalse($intellitimeAvailability->isAvailableDuringDay());
    $this->assertFalse($intellitimeAvailability->isAvailableDuringEvening());
    $this->assertFalse($intellitimeAvailability->isAvailableDuringNight());
  }

  public function testMatchesOneRange() {
    $start_time = date_make_date('2011-07-14T08:00');
    $end_time = date_make_date('2011-07-14T12:00');
    $intellitimeAvailability = $this->factory->create($this->buildAvailability($start_time, $end_time));
    $this->assertTrue($intellitimeAvailability->isAvailableDuringDay());
    $this->assertFalse($intellitimeAvailability->isAvailableDuringEvening());
    $this->assertFalse($intellitimeAvailability->isAvailableDuringNight());
  }

  public function testMatchesRangePastMidnight() {
    $start_time = date_make_date('2011-07-14T19:00');
    $end_time = date_make_date('2011-07-15T08:00');
    $intellitimeAvailability = $this->factory->create($this->buildAvailability($start_time, $end_time));
    $this->assertFalse($intellitimeAvailability->isAvailableDuringDay());
    $this->assertFalse($intellitimeAvailability->isAvailableDuringEvening());
    $this->assertTrue($intellitimeAvailability->isAvailableDuringNight());
  }

  public function testRangePastMidnightMatchesOnlyNight() {
    $start_time = date_make_date('2011-07-14T19:00');
    $end_time = date_make_date('2011-07-15T09:00');
    $intellitimeAvailability = $this->factory->create($this->buildAvailability($start_time, $end_time));
    $this->assertFalse($intellitimeAvailability->isAvailableDuringDay());
    $this->assertFalse($intellitimeAvailability->isAvailableDuringEvening());
    $this->assertTrue($intellitimeAvailability->isAvailableDuringNight());
  }

  private function createRange($start, $end) {
    return array('start' => $start, 'end' => $end);
  }

  private function buildAvailability($start_time, $end_time, $type = TZAvailabilityType::AVAILABLE) {
    $availability = new Availability();
    $availability->setStartTime($start_time);
    $availability->setEndTime($end_time);
    $availability->setType($type);
    return $availability;
  }

}