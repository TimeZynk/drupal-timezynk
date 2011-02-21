<?php

class TZUserStatusTest extends PHPUnit_Framework_TestCase {
  private $dueLimit;
  private $now;

  function setUp() {
    $this->dueLimit = 26*60*60; // 24 hours
    $this->now = time();
  }

  function testRedOnNoLogin() {
    $status = $this->createStatus(0);
    $this->assertEquals(TZUserStatus::RED, $status->getStatusCode($this->now));
  }

  function testYellowOnLogin() {
    // Last login 10 days ago
    $status = $this->createStatus(time() - 10*24*3600);
    $this->assertEquals(TZUserStatus::YELLOW, $status->getStatusCode($this->now));
  }

  function testGreenOnZeroDueReports() {
    // Last login 10 days ago
    $status = $this->createStatus(time() - 10*24*3600);
    $status->setNumberOfDueReports(0);
    $this->assertEquals(TZUserStatus::GREEN, $status->getStatusCode($this->now));
  }

  function testGreenOnManyButRecentDueReports() {
    // Last login 10 days ago
    $status = $this->createStatus(time() - 10*24*3600);
    $status->setNumberOfDueReports(2);
    $status->setEarliestDueEndTime($this->now - $this->dueLimit);
    $this->assertEquals(TZUserStatus::GREEN, $status->getStatusCode($this->now));
  }

  function testYellowOnManyNonRecentDueReports() {
    // Last login 10 days ago
    $status = $this->createStatus(time() - 10*24*3600);
    $status->setNumberOfDueReports(2);
    $status->setEarliestDueEndTime($this->now - $this->dueLimit-1);
    $this->assertEquals(TZUserStatus::YELLOW, $status->getStatusCode($this->now));
  }

  function testThrowsOnNegativeCount() {
    $status = $this->createStatus(time() - 10*24*3600);
    try {
      $status->setNumberOfDueReports(-1);
      $this->fail('Expect exception');
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

  function testThrowsOnStringCount() {
    $status = $this->createStatus(time() - 10*24*3600);
    try {
      $status->setNumberOfDueReports('abc');
      $this->fail('Expect exception');
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

  private function createStatus($lastLogin) {
    return new TZUserStatus($lastLogin, $this->dueLimit);
  }
}