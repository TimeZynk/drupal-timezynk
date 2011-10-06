<?php

class TZIntellitimeWeekFactoryTest extends PHPUnit_Framework_TestCase {
  public function setUp() {
    $this->account = (object)array(
      'name' => 'Kalle',
      'uid' => 63,
    );
  }

  public function testCreateWeek() {
    $server = $this->getMock('TZIntellitimeServer');

    $weekFactory = new TZIntellitimeWeekFactory($server, $this->account);

    $datetime = new DateTime('2011-02-07');
    $tzReports = array();

    $week = $weekFactory->createWeek($datetime, $tzReports);
    $this->assertInstanceOf('TZIntellitimeWeek', $week);
  }
}
