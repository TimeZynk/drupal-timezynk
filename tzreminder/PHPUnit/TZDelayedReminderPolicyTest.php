<?php

class TZDelayedReminderPolicyTest extends PHPUnit_Framework_TestCase {

  /**
   *
   * @var integer
   */
  private $expectedDelay = 23;
  
  public function setUp() {
    $this->nowTimestamp = 1234567890;
    $this->expectedDueTimestamp = $this->nowTimestamp - ($this->expectedDelay*60);
    $this->now = tzbase_make_date($this->nowTimestamp);
    $this->policy = new TZDelayedReminderPolicy($this->expectedDelay);
  }
  
  function testConstructorNoMinutesThrows() {
    try {
      new TZDelayedReminderPolicy();
      $this->fail("Expected exception");
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

  function testConstructorNegativeInputThrows() {
    try {
      new TZDelayedReminderPolicy(-1);
      $this->fail("Expected exception");
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

  function testGetQuery() {
    $expectedQuery = 'SELECT * FROM {node} n INNER JOIN {tzreport} t ON n.vid = t.vid WHERE t.flags < %d AND t.endtime < %d';
    $expectedQueryArgs = array( 
      TZFlags::REPORTED,
      $this->expectedDueTimestamp,
    );
    $query = $this->policy->getQuery($this->now);
    $this->assertEquals($expectedQuery, $query->getQueryString());
    $this->assertEquals($expectedQueryArgs, $query->getQueryArgs());
  }

  function testGetMessageMissingNumberOfReportsThrows() {
    try {
      $this->policy->getMessage();
      $this->fail("Should not work");
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

  function testGetMessageOneReport() {
    $expectedMessage = "Hi! We are waiting for one of your time reports. Please fill it in!";
    $this->assertEquals($expectedMessage, $this->policy->getMessage(1));
  }

  function testGetMessage23Reports() {
    $expectedMessage = "Hi! We are waiting for 23 of your time reports. Please fill them in!";
    $this->assertEquals($expectedMessage, $this->policy->getMessage(23));
  }

  function testShouldSendMessageReturnsFalseWhenNotEnoughTimeHasPassedSinceLastCall() {
    $timeLastCalled = clone($this->now);
    $timeNow = clone($timeLastCalled);
    $timeNow->modify('+2 minutes');
    $this->assertFalse($this->policy->shouldSendMessage($timeNow, $timeLastCalled));
  }

  function testShouldSendMessageReturnsTrueWhenEnoughTimeHasPassedSinceLastCall() {
    $timeLastCalled = clone($this->now);
    $timeNow = clone($timeLastCalled);
    $timeNow->modify('+23 minutes');
    $this->assertTrue($this->policy->shouldSendMessage($timeNow, $timeLastCalled));
  }

  function testShouldSendMessageThrowsOnMissingArguments() {
    try {
      $this->policy->shouldSendMessage();
      $this->fail();
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

  function testShouldSendMessageThrowsOnMissingLastCalled() {
    try {
      $this->policy->shouldSendMessage($timeNow);
      $this->fail();
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }
}
