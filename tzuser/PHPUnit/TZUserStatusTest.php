<?php

class TZUserStatusTest extends PHPUnit_Framework_TestCase {
  private $now;

  function setUp() {
    $this->redLimit = NULL;
    $this->now = time();
  }

  function testGrayOnNoLogin() {
    $status = $this->createStatus(0);
    $this->assertEquals(TZUserStatus::GREY, $status->getStatusCode($this->now));
  }

  function testRedExactlyOnRedLimit() {
    $this->redLimit = 3600;
    $status = $this->createStatus($this->now - $this->redLimit);
    $this->assertEquals(TZUserStatus::RED, $status->getStatusCode($this->now));
  }

  function testRedBeforeRedLimit() {
    $this->redLimit = 3600;
    $status = $this->createStatus($this->now - $this->redLimit - 342342);
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

  function testYellowOnManyDueReports() {
    // Last login 10 days ago
    $status = $this->createStatus(time() - 10*24*3600);
    $status->setNumberOfDueReports(2);
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
    return new TZUserStatus($lastLogin, $this->redLimit);
  }
}