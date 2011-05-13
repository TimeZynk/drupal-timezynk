<?php

class TZUserStatusTest extends PHPUnit_Framework_TestCase {
  private $now;
  private $redLimit;
  private $expectedUid;

  function setUp() {
    $this->expectedUid = 181881;
    $this->redLimit = 0;
    $this->now = time();
  }

  function testReturnsUid() {
    $status = $this->createStatus(0);
    $this->assertEquals($this->expectedUid, $status->getUid());
  }

  function testReturnsTimeStamp() {
    $status = $this->createStatus(0);
    $this->assertEquals($this->now, $status->getStatusTimeStamp());
  }

  function testGrayOnNoLogin() {
    $status = $this->createStatus(0);
    $this->assertEquals(TZUserStatus::GREY, $status->getStatusCode());
  }

  function testRedExactlyOnRedLimit() {
    $this->redLimit = 3600;
    $status = $this->createStatus($this->now - $this->redLimit);
    $this->assertEquals(TZUserStatus::RED, $status->getStatusCode());
  }

  function testRedBeforeRedLimit() {
    $this->redLimit = 3600;
    $status = $this->createStatus($this->now - $this->redLimit - 342342);
    $this->assertEquals(TZUserStatus::RED, $status->getStatusCode());
  }

  function testYellowOnLogin() {
    // Last login 10 days ago
    $this->redLimit = 10*24*3600 + 1;
    $status = $this->createStatus($this->now - 10*24*3600);
    $this->assertEquals(TZUserStatus::YELLOW, $status->getStatusCode());
  }

  function testGreenOnZeroDueReports() {
    // Last login 1 days ago
    $this->redLimit = 10*24*3600 + 1;
    $status = $this->createStatus($this->now - 10*24*3600);
    $status->setNumberOfDueReports(0);
    $this->assertEquals(TZUserStatus::GREEN, $status->getStatusCode());
  }

  function testYellowOnManyDueReports() {
    // Last login 10 days ago
    $this->redLimit = 10*24*3600 + 1;
    $status = $this->createStatus($this->now - 10*24*3600);
    $status->setNumberOfDueReports(2);
    $this->assertEquals(TZUserStatus::YELLOW, $status->getStatusCode());
  }

  function testThrowsOnNegativeCount() {
    $status = $this->createStatus($this->now - 10*24*3600);
    try {
      $status->setNumberOfDueReports(-1);
      $this->fail('Expect exception');
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

  function testThrowsOnStringCount() {
    $status = $this->createStatus($this->now - 10*24*3600);
    try {
      $status->setNumberOfDueReports('abc');
      $this->fail('Expect exception');
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

  private function createStatus($lastLogin) {
    return new TZUserStatus($this->expectedUid, $this->now, $lastLogin, $this->redLimit);
  }
}