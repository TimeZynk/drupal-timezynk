<?php

class IntellitimeAvailabilityPageTest extends PHPUnit_Framework_TestCase {
  public function setUp() {
    $this->bot = $this->getMock('TZIntellitimeBot');
  }

  public function testWhenBuildingFromEmptyPage_ItShouldReturnNoAvailableDays() {
    $page = $this->build_from_page('availability-0-days.txt');
    $days = $page->getAvailableDays();
    $this->assertEquals(count($days), 0);
  }

  public function testWhenBuildingFromPageWithOneAvailableDay_ItShouldReturnOneAvailableDay() {
    $page = $this->build_from_page('availability-1-day.txt');
    $days = $page->getAvailableDays();
    $this->assertEquals(count($days), 1);
  }

  public function testWhenBuildingFromPageWithEightAvailableDay_ItShouldReturnEightAvailableDay() {
    $page = $this->build_from_page('availability-8-days.txt');
    $days = $page->getAvailableDays();
    $this->assertEquals(count($days), 8);
  }

  public function testWhenBuildingFromPageWith23AvailableDay_ItShouldReturn23AvailableDay() {
    $page = $this->build_from_page('availability-23-days.txt');
    $days = $page->getAvailableDays();
    $this->assertEquals(count($days), 23);
  }

  public function testShouldReturnCorrectDateForPageWithOneAvailableDay() {
    $page = $this->build_from_page('availability-1-day.txt');
    $days = $page->getAvailableDays();
    $expected_dates = array_fill_keys(array ('2011-07-14'), TRUE);
    $this->checkForDates($days, $expected_dates);
  }

  public function testShouldReturnCorrectDatesInOrderForPageWith8AvailableDays() {
    $page = $this->build_from_page('availability-8-days.txt');
    $days = $page->getAvailableDays();
    $expected_dates = array_fill_keys(array(
      '2011-07-14',
      '2011-07-15',
      '2011-07-20',
      '2011-07-22',
      '2011-07-26',
      '2011-07-28',
      '2011-07-29',
      '2011-08-11',
    ), TRUE);
    $this->checkForDates($days, $expected_dates);
  }

  public function testShouldReturnCorrectFieldsForPageWithOneAvailableDay() {
    $page = $this->build_from_page('availability-1-day.txt');
    $days = $page->getAvailableDays();
    $ia = $days['2011-07-14'];
    $this->assertTrue($ia->isAvailableDuringDay());
    $this->assertTrue($ia->isAvailableDuringEvening());
    $this->assertTrue($ia->isAvailableDuringNight());
  }

  public function testShouldReturnCorrectFieldsForPageWithEightAvailableDays() {
    $page = $this->build_from_page('availability-8-days.txt');
    $days = $page->getAvailableDays();

    $this->assertTrue($days['2011-07-14']->isAvailableDuringDay());
    $this->assertFalse($days['2011-07-14']->isAvailableDuringEvening());
    $this->assertTrue($days['2011-07-14']->isAvailableDuringNight());

    $this->assertTrue($days['2011-08-11']->isAvailableDuringDay());
    $this->assertFalse($days['2011-08-11']->isAvailableDuringEvening());
    $this->assertFalse($days['2011-08-11']->isAvailableDuringNight());
  }

  /**
   * Posting with setting any data implies clearing all availabilities.
   */
  public function testShouldReturnUpdatePostWhenGettingPostWithoutData() {
    $page = $this->build_from_page('availability-0-days.txt');
    $this->assertInstanceOf('IntellitimeAvailabilityUpdatePost', $page->getPost());
  }

  public function testShouldGetAPostAfterAddingData() {
    $expectedAvailability = new IntellitimeAvailability();
    $expectedAvailability->setDate(date_make_date('2011-07-15'));
    $page = $this->build_from_page('availability-0-days.txt');
    $page->setAvailabilities(array($expectedAvailability));
    $this->assertNotNull($page->getPost());
  }

  public function testCorrectPostWhenAddingDays() {
    $expectedAvailability = new IntellitimeAvailability();
    $expectedAvailability->setDate(date_make_date('2011-07-15'));
    $page = $this->build_from_page('availability-0-days.txt');
    $page->setAvailabilities(array($expectedAvailability));
    $this->assertInstanceOf('IntellitimeAvailabilityAddPost', $page->getPost());
  }

  public function testCorrectPostWhenUpdatingDays() {
    $expectedAvailability = new IntellitimeAvailability();
    $expectedAvailability->setDate(date_make_date('2011-07-14'));
    $page = $this->build_from_page('availability-1-day.txt');
    $page->setAvailabilities(array($expectedAvailability));
    $this->assertInstanceOf('IntellitimeAvailabilityUpdatePost', $page->getPost());
  }

  public function testThrowsWhenAddingAvailabilitiesBeforeDateRange() {
    $expectedAvailability = new IntellitimeAvailability();
    $expectedAvailability->setDate(date_make_date('2011-07-13'));
    $page = $this->build_from_page('availability-1-day.txt');
    try {
      $page->setAvailabilities(array($expectedAvailability));
      $this->fail();
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e, 'Expected exception');
    }
  }

  private function checkForDates($days, $expected_dates) {
    $this->assertEquals(count($days), count($expected_dates));
    foreach ($days as $day) {
      $date_string = $day->getDate()->format('Y-m-d');
      $this->assertTrue($expected_dates[$date_string]);
      $expected_dates[$date_string] = FALSE;
    }
  }

  private function build_from_page($filename) {
    $full_name = dirname(__FILE__) . "/../tests/availability/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    return new IntellitimeAvailabilityPage($contents, $this->bot);
  }
}