<?php

class TZTimeOfDaySpanTest extends PHPUnit_Framework_TestCase {
  function setUp() {
    $this->tz = date_default_timezone();
  }

  function testJustAfterStart() {
    $span = new TZTimeOfDaySpan('22:00', '10:00');
    $date = new DateTime('2011-02-10 22:00', $this->tz);
    $this->assertTrue($span->isInsideSpan($date));
  }

  function testJustBeforeStart() {
    $span = new TZTimeOfDaySpan('20:00', '10:00');
    $date = new DateTime('2011-02-01 19:59:59', $this->tz);
    $this->assertFalse($span->isInsideSpan($date));
  }

  function testJustBeforeEnd() {
    $span = new TZTimeOfDaySpan('22:00', '07:00');
    $date = new DateTime('2011-02-15 07:00:00', $this->tz);
    $this->assertTrue($span->isInsideSpan($date));
  }

  function testJustAfterEnd() {
    $span = new TZTimeOfDaySpan('22:00', '08:00');
    $date = new DateTime('2011-02-20 08:00:01', $this->tz);
    $this->assertFalse($span->isInsideSpan($date));
  }

  function testMiddleBeforeMidnight() {
    $span = new TZTimeOfDaySpan('19:00', '10:00');
    $date = new DateTime('2011-02-27 23:59:59', $this->tz);
    $this->assertTrue($span->isInsideSpan($date));
  }

  function testMiddleAfterMidnight() {
    $span = new TZTimeOfDaySpan('23:00', '10:00');
    $date = new DateTime('2011-12-31 00:00:01', $this->tz);
    $this->assertTrue($span->isInsideSpan($date));
  }

  function testIllegalStartTimeThrows() {
    try {
      $span = new TZTimeOfDaySpan('abcd', '10:00');
      $this->fail('Expected exception');
    } catch(InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

  function testIllegalEndTimeThrows() {
    try {
      $span = new TZTimeOfDaySpan('13:00', '10:0A');
      $this->fail('Expected exception');
    } catch(InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

}