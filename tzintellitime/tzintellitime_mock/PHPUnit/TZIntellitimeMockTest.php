<?php

class TZIntellitimeMockTest extends PHPUnit_Framework_TestCase {
  function testParseTimeJustHours() {
    $time = _tzintellitime_mock_parse_time('9');
    $this->assertEquals('09:00', $time);
  }

  function testParseTimeProperTime() {
    $time = _tzintellitime_mock_parse_time('07:21');
    $this->assertEquals('07:21', $time);
  }

  function testParseTimeHoursAndMinutesWithoutColon() {
    $time = _tzintellitime_mock_parse_time('0107');
    $this->assertEquals('01:07', $time);
  }

  function testParseTimeHoursAndMinutesWithoutColonWithoutLeadingZero() {
    $time = _tzintellitime_mock_parse_time('821');
    $this->assertEquals('08:21', $time);
  }

  function testParseTimeHoursDoubleDigits() {
    $time = _tzintellitime_mock_parse_time('21');
    $this->assertEquals('21:00', $time);
  }

  function testParseTimeHoursMinutesDoubleDigits() {
    $time = _tzintellitime_mock_parse_time('2109');
    $this->assertEquals('21:09', $time);
  }

  function testParseTimeHoursMinutesDoubleDigitsProper() {
    $time = _tzintellitime_mock_parse_time('23:59');
    $this->assertEquals('23:59', $time);
  }

  function testParseTimeHoursMinutesMalplacedColon() {
    $time = _tzintellitime_mock_parse_time('2:359');
    $this->assertEquals('02:35', $time);
  }
}
