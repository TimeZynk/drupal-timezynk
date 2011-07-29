<?php
class TZFixedDateReminderPolicyTest extends PHPUnit_Framework_TestCase {

  public function setUp() {
    $this->expectedDueDay = 15;
    $this->expectedReminderHour = 16;
    $this->policy = new TZFixedDateReminderPolicy($this->expectedDueDay, $this->expectedReminderHour);
  }

  public function testConstructorWithoutDateThrows() {
    try {
      $this->policy = new TZFixedDateReminderPolicy();
      $this->fail();
    } catch (InvalidArgumentException $e) {
      $this->assertEquals("Must set due day", $e->getMessage());
    }
  }

  public function testConstructorWithoutReminderHourThrows() {
    try {
      $this->policy = new TZFixedDateReminderPolicy(15);
      $this->fail();
    } catch (InvalidArgumentException $e) {
      $this->assertEquals("Must set reminder hour", $e->getMessage());
    }
  }

  public function testDoesNotAcceptDaysGreaterThan31() {
    try {
      $this->policy = new TZFixedDateReminderPolicy(32, 0);
      $this->fail();
    } catch (InvalidArgumentException $e) {
      $this->assertEquals("Illegal due day, must be in range 1-28", $e->getMessage());
    }
  }

  public function testDoesNotAcceptDaysLessThan1() {
    try {
      $this->policy = new TZFixedDateReminderPolicy(-2, 0);
      $this->fail();
    } catch (InvalidArgumentException $e) {
      $this->assertEquals("Illegal due day, must be in range 1-28", $e->getMessage());
    }
  }

  public function testDoesNotAcceptHoursGreaterThan23() {
    try {
      $this->policy = new TZFixedDateReminderPolicy(15, 24);
      $this->fail();
    } catch (InvalidArgumentException $e) {
      $this->assertEquals("Illegal reminder hour, must be in range 0-23", $e->getMessage());
    }
  }

  public function testDoesNotAcceptHoursLessThan0() {
    try {
      $this->policy = new TZFixedDateReminderPolicy(1, -1);
      $this->fail();
    } catch (InvalidArgumentException $e) {
      $this->assertEquals("Illegal reminder hour, must be in range 0-23", $e->getMessage());
    }
  }


  public function testGetQueryWhenDateSeparateFromDueDayShouldReturnNull() {
    $now = date_make_date("2011-05-16T00:00", date_default_timezone(FALSE));
    $this->assertNull($this->policy->getQuery($now));
  }

  public function testGetQueryBeforeRightHourReturnsNull() {
    $now = date_make_date("2011-05-".$this->expectedDueDay ."T". ($this->expectedReminderHour-1) . ":59:59",
        date_default_timezone(FALSE));
    $this->assertNull($this->policy->getQuery($now));
  }


  public function testGetQuerySetsDateToBeginningOfDayAfterDueDateInGivenYearAndMonth() {
    $now = date_make_date("2011-05-15T16:23", date_default_timezone(FALSE));
    $expectedDueDate = clone $now;
    $expectedDueDate->setDate($now->format("Y"), $now->format("m"), $this->expectedDueDay+1);
    $expectedDueDate->setTime(0,0,0);
    $expectedQuery = 'SELECT * FROM {node} n INNER JOIN {tzreport} t ON n.vid = t.vid WHERE t.flags < %d AND t.endtime < %d';
    $expectedQueryArgs = array(
      TZFlags::REPORTED,
      $expectedDueDate->format('U'),
    );
    $query = $this->policy->getQuery($now);
    $this->assertEquals($expectedQuery, $query->getQueryString());
    $this->assertEquals($expectedQueryArgs, $query->getQueryArgs());
  }

  public function testSettingDueDateToAfterEndOfMonthTruncatesDown() {
    $this->policy = new TZFixedDateReminderPolicy(28, $this->expectedReminderHour);
    $now = date_make_date("2011-02-28T16:23", date_default_timezone(FALSE));
    $expectedDueDate = date_make_date("2011-03-01T00:00", date_default_timezone(FALSE));
    $query = $this->policy->getQuery($now);
    $queryArgs = $query->getQueryArgs();
    $actualDueDate = tzbase_make_date(end($queryArgs));
    $this->assertEquals($expectedDueDate, $actualDueDate);
  }

  public function testShouldNotSendMessageIfSuccessfullyCalledForToday() {
    $now = date_make_date("2011-02-15T16:23", date_default_timezone(FALSE));
    $timeLastCalled = date_make_date("2011-02-15T00:00", date_default_timezone(FALSE));

    $this->assertFalse($this->policy->shouldSendMessage($now, $timeLastCalled));
  }

  public function testShouldNotSendMessageIfNotRightDay() {
    $now = date_make_date("2011-02-16T16:23", date_default_timezone(FALSE));
    $timeLastCalled = date_make_date("2011-02-14T00:00", date_default_timezone(FALSE));

    $this->assertFalse($this->policy->shouldSendMessage($now, $timeLastCalled));
  }

  public function testShouldNotSendMessageIfTooEarlyOnRightDay() {
    $now = date_make_date("2011-02-15T15:59:59", date_default_timezone(FALSE));
    $timeLastCalled = date_make_date("2011-02-14T00:00", date_default_timezone(FALSE));

    $this->assertFalse($this->policy->shouldSendMessage($now, $timeLastCalled));
  }

  public function testShouldSendMessageIfRightDayRightHourAndNotSentBeforeToday() {
    $now = date_make_date("2011-02-15T16:00:00", date_default_timezone(FALSE));
    $timeLastCalled = date_make_date("2011-02-14T00:00", date_default_timezone(FALSE));

    $this->assertTrue($this->policy->shouldSendMessage($now, $timeLastCalled));
  }

  public function testShouldThrowIfTimeNowIsLessThanTimeLastCalled() {
    try {
      $now = date_make_date("2011-02-15T16:00:00", date_default_timezone(FALSE));
      $timeLastCalled = date_make_date("2011-02-17T00:00", date_default_timezone(FALSE));

      $this->policy->shouldSendMessage($now, $timeLastCalled);
      $this->fail();
    } catch (InvalidArgumentException $e) {
      $this->assertEquals("Seemingly timeNow is less than timeLastCalled. Is time running backwards where you live?",
        $e->getMessage());
    }
  }

  public function testIsOfCorrectInterface() {
    $this->assertInstanceOf('TZReminderPolicy', $this->policy);
  }

  function testGetNameOfPolicy() {
    $this->assertEquals('fixed', $this->policy->getName());
  }
}