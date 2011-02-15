<?php

class TZReminderTest extends PHPUnit_Framework_TestCase {
  function testCalculateLastCall() {
    $expectedUid = 34;
    $expectedLastCalledString = "2011-02-14 14:02";
    $expectedLastCalled = 1297688521; // 2011-02-14 14:02 GMT+1
    $expectedPolicy = 'delay';
    $config = new stdClass();
    $config->last_call[$expectedUid][$expectedPolicy] = $expectedLastCalled;

    $lastCalled = _tzreminder_get_last_call($config, $expectedPolicy, $expectedUid);
    $this->assertEquals($expectedLastCalledString, $lastCalled->format('Y-m-d H:i'));
  }

  function testNowIsReallyNow() {
    $now = tzbase_make_date();
    $this->assertTrue(abs($now->format('U') - time()) < 2);
  }
}