<?php

class TZIntellitimeDefaultWeekFactoryTest extends PHPUnit_Framework_TestCase {
  public function setUp() {
    $this->account = (object)array(
      'name' => 'Kalle',
      'uid' => 63,
    );
  }

  public function testCreateWeek() {
    $serverInterface = $this->getMock('TZIntellitimeServerInterface');

    $weekFactory = new TZIntellitimeDefaultWeekFactory($serverInterface, $this->account);

    $datetime = new DateTime('2011-02-07');
    $tzReports = array();

    $week = $weekFactory->createWeek($datetime, $tzReports);
    $this->assertInstanceOf('TZIntellitimeWeek', $week);
  }

  public function testCreateWeekNULL() {
    try {
      $weekFactory = new TZIntellitimeDefaultWeekFactory(NULL, $this->account);
      $this->fail('Should throw InvalidArgumentException');
    } catch(InvalidArgumentException $e) {
      $this->assertTrue(TRUE);
    }
  }
}
