<?php

class TZIntellitimeSimpleSyncPolicyTest extends PHPUnit_Framework_TestCase {
  private $timezone;

  public function setUp() {
    $this->timezone = date_default_timezone(FALSE);
    $this->account = (object)array(
      'name' => 'Kalle',
      'uid' => 63,
    );
  }

  public function testSingleWeek() {
    $startDate = new DateTime('2011-02-07', $this->timezone);
    $policy = new TZIntellitimeSimpleSyncPolicy($this->account, $startDate, 0);
    $this->assertNotNull($policy);

    $firstWeek = $policy->getNextWeekToSync();
    $this->assertEquals('2011W06', $firstWeek->format('o\WW'));

    $secondWeek = $policy->getNextWeekToSync();
    $this->assertNull($secondWeek);
  }

  public function testTwelveWeeks() {
    $expectedNbrOfWeeks = 12;
    $startDate = new DateTime('2011-02-07', $this->timezone);
    $policy = new TZIntellitimeSimpleSyncPolicy($this->account, $startDate, $expectedNbrOfWeeks);
    $this->assertNotNull($policy);

    $i = 0;
    while($week = $policy->getNextWeekToSync()) {
      $expectedWeek = sprintf('2011W%02d', 6 + $i);
      $this->assertEquals($expectedWeek, $week->format('o\WW'));
      $i++;
    }

    $this->assertEquals($expectedNbrOfWeeks + 1, $i);
  }

  public function testAddUnfinishedWeeks() {
    $expectedNbrOfWeeks = 0;
    $expectedWeeks = array(
      new DateTime('2011-02-07', $this->timezone),
      new DateTime('2010W51', $this->timezone),
      new DateTime('2011W01', $this->timezone),
      new DateTime('2011W05', $this->timezone),
      new DateTime('2010W51', $this->timezone),
      new DateTime('2011-02-07', $this->timezone),
    );

    $policy = new TZIntellitimeSimpleSyncPolicy($this->account, $expectedWeeks[0], $expectedNbrOfWeeks);

    $policy->addWeeks($expectedWeeks);

    $countWeeks = array();
    foreach($expectedWeeks as $weekDate) {
      $countWeeks[$weekDate->format('o\WW')] = 0;
    }

    while($week = $policy->getNextWeekToSync()) {
      $key = $week->format('o\WW');
      if(!isset($countWeeks[$key])) {
        $this->fail('Returned week we never asked for');
      }
      $countWeeks[$key]++;
      $this->assertEquals(1, $countWeeks[$key]);
    }

    foreach($countWeeks as $week => $count) {
      $this->assertEquals(1, $count, "Week $week never returned");
    }
  }
}
