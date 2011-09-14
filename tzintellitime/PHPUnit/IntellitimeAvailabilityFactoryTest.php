<?php

class IntellitimeAvailabilityFactoryTest extends PHPUnit_Framework_TestCase {

  public function setUp() {
    $this->store = new AvailabilityStore(NULL);
    $day_range = '08:00-12:00';
    $evening_range = '12:00-18:00';
    $night_range = '18:00-24:00';
    $this->factory = new IntellitimeAvailabilityFactory($this->store, $day_range, $evening_range, $night_range);
  }

  public function testOutsideDayEveningNightRangesReturnsNothing() {
    $start_time = date_make_date('2011-07-14T00:00');
    $end_time = date_make_date('2011-07-14T08:00');
    $intellitimeAvailability = $this->factory->createIntellitimeAvailability($this->buildAvailability($start_time, $end_time));
    $this->assertFalse($intellitimeAvailability->isAvailableDuringDay());
    $this->assertFalse($intellitimeAvailability->isAvailableDuringEvening());
    $this->assertFalse($intellitimeAvailability->isAvailableDuringNight());
  }

  public function testMatchesOneRange() {
    $start_time = date_make_date('2011-07-14T08:00');
    $end_time = date_make_date('2011-07-14T12:00');
    $intellitimeAvailability = $this->factory->createIntellitimeAvailability($this->buildAvailability($start_time, $end_time));
    $this->assertTrue($intellitimeAvailability->isAvailableDuringDay());
    $this->assertFalse($intellitimeAvailability->isAvailableDuringEvening());
    $this->assertFalse($intellitimeAvailability->isAvailableDuringNight());
  }

  public function testMatchesSmallerRange() {
    $start_time = date_make_date('2011-07-14T07:00');
    $end_time = date_make_date('2011-07-14T13:00');
    $intellitimeAvailability = $this->factory->createIntellitimeAvailability($this->buildAvailability($start_time, $end_time));
    $this->assertTrue($intellitimeAvailability->isAvailableDuringDay());
    $this->assertTrue($intellitimeAvailability->isAvailableDuringEvening());
    $this->assertFalse($intellitimeAvailability->isAvailableDuringNight());
  }

  public function testMatchesRangePastMidnight() {
    $start_time = date_make_date('2011-07-14T19:00');
    $end_time = date_make_date('2011-07-15T08:00');
    $intellitimeAvailability = $this->factory->createIntellitimeAvailability($this->buildAvailability($start_time, $end_time));
    $this->assertFalse($intellitimeAvailability->isAvailableDuringDay());
    $this->assertFalse($intellitimeAvailability->isAvailableDuringEvening());
    $this->assertTrue($intellitimeAvailability->isAvailableDuringNight());
  }

  public function testRangePastMidnightMatchesOnlyNight() {
    $start_time = date_make_date('2011-07-14T19:00');
    $end_time = date_make_date('2011-07-15T09:00');
    $intellitimeAvailability = $this->factory->createIntellitimeAvailability($this->buildAvailability($start_time, $end_time));
    $this->assertFalse($intellitimeAvailability->isAvailableDuringDay());
    $this->assertFalse($intellitimeAvailability->isAvailableDuringEvening());
    $this->assertTrue($intellitimeAvailability->isAvailableDuringNight());
  }

  public function testSingleFieldBecomesSingleAvailability() {
    $expected_ia = new IntellitimeAvailability(date_make_date('2011-06-23'), 'abc_form_id');
    $expected_ia->setDay(TRUE);
    $availabilities = $this->factory->createAvailabilities($expected_ia);
    $this->assertEquals(1, count($availabilities));
    $this->assertEquals('2011-06-23 08:00:00', $availabilities[0]->getStartTime()->format('Y-m-d H:i:s'));
    $this->assertEquals('2011-06-23 12:00:00', $availabilities[0]->getEndTime()->format('Y-m-d H:i:s'));
  }

  public function testTwoConsecutiveFieldsBecomesSingleAvailability() {
    $expected_ia = new IntellitimeAvailability(date_make_date('2011-06-23'), 'abc_form_id');
    $expected_ia->setDay(TRUE);
    $expected_ia->setEvening(TRUE);
    $availabilities = $this->factory->createAvailabilities($expected_ia);
    $this->assertEquals(1, count($availabilities));
    $this->assertEquals('2011-06-23 08:00:00', $availabilities[0]->getStartTime()->format('Y-m-d H:i:s'));
    $this->assertEquals('2011-06-23 18:00:00', $availabilities[0]->getEndTime()->format('Y-m-d H:i:s'));
  }

  public function testThreeConsecutiveFieldsBecomesSingleAvailability() {
    $expected_ia = new IntellitimeAvailability(date_make_date('2011-06-23'), 'abc_form_id');
    $expected_ia->setDay(TRUE)->setEvening(TRUE)->setNight(TRUE);
    $availabilities = $this->factory->createAvailabilities($expected_ia);
    $this->assertEquals(1, count($availabilities));
    $this->assertEquals('2011-06-23 08:00:00', $availabilities[0]->getStartTime()->format('Y-m-d H:i:s'));
    $this->assertEquals('2011-06-24 00:00:00', $availabilities[0]->getEndTime()->format('Y-m-d H:i:s'));
  }

  public function testNonConsecutiveFieldsBecomesTwoAvailabilities() {
    $expected_ia = new IntellitimeAvailability(date_make_date('2011-06-23'), 'abc_form_id');
    $expected_ia->setDay(TRUE)->setEvening(FALSE)->setNight(TRUE);
    $availabilities = $this->factory->createAvailabilities($expected_ia);
    $this->assertEquals(2, count($availabilities));
    $this->assertEquals('2011-06-23 08:00:00', $availabilities[0]->getStartTime()->format('Y-m-d H:i:s'));
    $this->assertEquals('2011-06-23 12:00:00', $availabilities[0]->getEndTime()->format('Y-m-d H:i:s'));
    $this->assertEquals('2011-06-23 18:00:00', $availabilities[1]->getStartTime()->format('Y-m-d H:i:s'));
    $this->assertEquals('2011-06-24 00:00:00', $availabilities[1]->getEndTime()->format('Y-m-d H:i:s'));
  }

  public function testAllFieldsOffBecomesEmptyArray() {
    $expected_ia = new IntellitimeAvailability(date_make_date('2011-06-23'), 'abc_form_id');
    $expected_ia->setDay(FALSE)->setEvening(FALSE)->setNight(FALSE);
    $availabilities = $this->factory->createAvailabilities($expected_ia);
    $this->assertEquals(0, count($availabilities));
  }

  public function testNonConsecutiveRangesBecomesThreeAvailabilities() {
    $expected_ia = new IntellitimeAvailability(date_make_date('2011-06-23'), 'abc_form_id');
    $expected_ia->setDay(TRUE)->setEvening(TRUE)->setNight(TRUE);
    $factory = new IntellitimeAvailabilityFactory($this->store, "08:00-11:59", "12:00-17:59", "18:00-01:00");
    $availabilities = $factory->createAvailabilities($expected_ia);
    $this->assertEquals(3, count($availabilities));
    $this->assertEquals('2011-06-23 08:00:00', $availabilities[0]->getStartTime()->format('Y-m-d H:i:s'));
    $this->assertEquals('2011-06-23 11:59:00', $availabilities[0]->getEndTime()->format('Y-m-d H:i:s'));
    $this->assertEquals('2011-06-23 12:00:00', $availabilities[1]->getStartTime()->format('Y-m-d H:i:s'));
    $this->assertEquals('2011-06-23 17:59:00', $availabilities[1]->getEndTime()->format('Y-m-d H:i:s'));
    $this->assertEquals('2011-06-23 18:00:00', $availabilities[2]->getStartTime()->format('Y-m-d H:i:s'));
    $this->assertEquals('2011-06-24 01:00:00', $availabilities[2]->getEndTime()->format('Y-m-d H:i:s'));
  }

  private function buildAvailability($start_time, $end_time, $type = TZAvailabilityType::AVAILABLE) {
    $availability = new Availability();
    $availability->setStartTime($start_time);
    $availability->setEndTime($end_time);
    $availability->setType($type);
    return $availability;
  }
}