<?php

class GetAvailabilityIntervalHandlerTest extends PHPUnit_Framework_TestCase {
  function setUp() {
    $this->store = $this->getMockBuilder('AvailabilityStore')
                        ->disableOriginalConstructor()
                        ->getMock();
    $this->handler = new GetAvailabilityIntervalHandler(0, $this->store);
    $this->command = new TZGetAvailabilityIntervalsCmd();
    $this->result = new TZResult();
  }

  function testDoNotReturnDisabledIntervals() {
    $intervals = new stdClass();
    $intervals->enabled = false;
    $intervals->list = array(
      (object)array('begin' => '12:19', 'end' => '17:32'),
    );

    $this->store->expects($this->once())
      ->method('getAvailabilityIntervals')
      ->will($this->returnValue($intervals));

    $this->handler->handle($this->command, $this->result);
    $this->assertFalse($this->result->get_availability_intervals_result()->enabled());
    $this->assertEquals(0, $this->result->get_availability_intervals_result()->intervals_size());
  }

  function testReturnSingleInterval() {
    $intervals = new stdClass();
    $intervals->enabled = true;
    $intervals->list = array(
      (object)array('begin' => '12:19', 'end' => '17:32'),
    );

    $this->store->expects($this->once())
      ->method('getAvailabilityIntervals')
      ->will($this->returnValue($intervals));

    $this->handler->handle($this->command, $this->result);
    $this->assertTrue($this->result->get_availability_intervals_result()->enabled());
    $this->assertEquals(1, $this->result->get_availability_intervals_result()->intervals_size());
    $tzinterval =  $this->result->get_availability_intervals_result()->interval(0);
    $this->assertEquals(12, $tzinterval->start()->hour());
    $this->assertEquals(19, $tzinterval->start()->minute());
    $this->assertEquals(17, $tzinterval->end()->hour());
    $this->assertEquals(32, $tzinterval->end()->minute());
    $this->assertFalse($tzinterval->exclusive());
  }

  function testReturnExclusiveInterval() {
    $intervals = new stdClass();
    $intervals->enabled = true;
    $intervals->list = array(
      (object)array('begin' => '12:19', 'end' => '17:32', 'exclusive' => TRUE),
    );

    $this->store->expects($this->once())
      ->method('getAvailabilityIntervals')
      ->will($this->returnValue($intervals));

    $this->handler->handle($this->command, $this->result);
    $this->assertTrue($this->result->get_availability_intervals_result()->enabled());
    $this->assertEquals(1, $this->result->get_availability_intervals_result()->intervals_size());
    $tzinterval =  $this->result->get_availability_intervals_result()->interval(0);
    $this->assertTrue($tzinterval->exclusive());
  }

  function testReturnMultipleIntervals() {
    $intervals = new stdClass();
    $intervals->enabled = true;
    $intervals->list = array(
      (object)array('begin' => '12:19', 'end' => '17:32'),
      (object)array('begin' => '08:15', 'end' => '16:33'),
      (object)array('begin' => '21:20', 'end' => '04:09'),
    );

    $this->store->expects($this->once())
      ->method('getAvailabilityIntervals')
      ->will($this->returnValue($intervals));

    $this->handler->handle($this->command, $this->result);
    $this->assertTrue($this->result->get_availability_intervals_result()->enabled());
    $this->assertEquals(3, $this->result->get_availability_intervals_result()->intervals_size());
    $tzinterval =  $this->result->get_availability_intervals_result()->interval(2);
    $this->assertEquals(21, $tzinterval->start()->hour());
    $this->assertEquals(20, $tzinterval->start()->minute());
    $this->assertEquals(4, $tzinterval->end()->hour());
    $this->assertEquals(9, $tzinterval->end()->minute());
  }

  function testReturnsDaysInAdvanceWithIntervalsDisabled() {
    $intervals = new stdClass();
    $intervals->enabled = false;
    $intervals->days_in_advance = 1;

    $this->store->expects($this->once())
      ->method('getAvailabilityIntervals')
      ->will($this->returnValue($intervals));
    
    $this->handler->handle($this->command, $this->result);
    $this->assertEquals(1, $this->result->get_availability_intervals_result()->days_in_advance());
  }

  function testReturnsDaysInAdvanceWithIntervalsEnabled() {
    $intervals = new stdClass();
    $intervals->enabled = true;
    $intervals->list = array(
      (object)array('begin' => '12:19', 'end' => '17:32'),
    );
    $intervals->days_in_advance = 23;

    $this->store->expects($this->once())
      ->method('getAvailabilityIntervals')
      ->will($this->returnValue($intervals));
    
    $this->handler->handle($this->command, $this->result);
    $this->assertEquals(23, $this->result->get_availability_intervals_result()->days_in_advance());
    $this->assertTrue($this->result->get_availability_intervals_result()->enabled());
  }

  function testStopsNegativeDaysInAdvance() {
    $intervals = new stdClass();
    $intervals->days_in_advance = -23;

    $this->store->expects($this->once())
      ->method('getAvailabilityIntervals')
      ->will($this->returnValue($intervals));
    
    $this->handler->handle($this->command, $this->result);
    $this->assertEquals(0, $this->result->get_availability_intervals_result()->days_in_advance());
  }
}
