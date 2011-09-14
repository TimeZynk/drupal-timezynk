<?php

class AvailabilityTest extends PHPUnit_Framework_TestCase {
  function setUp() {

  }

  function testSameTimeOverlaps() {
    $a = new Availability(array (
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308825360, // 2011-06-23 12:36:00 +0200
      'end_time' => 1308829980, //  2011-06-23 13:52:00 +0200
    ));
    $this->assertTrue($a->isOverlappingRange('12:36', '13:53'));
  }

  function testCompletelyWithinTimeOverlaps() {
    $a = new Availability(array(
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308825900, // 2011-06-23 12:45:00 +0200
      'end_time' => 1308829500, //  2011-06-23 13:45:00 +0200
    ));
    $this->assertTrue($a->isOverlappingRange('12:36', '13:53'));
  }

  function testBeginsWithinEndsAfterTimeOverlaps() {
    $a = new Availability(array(
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308825900, // 2011-06-23 12:45:00 +0200
      'end_time' => 1308829500, //  2011-06-23 13:45:00 +0200
    ));
    $this->assertTrue($a->isOverlappingRange('12:36', '13:40'));
  }

  function testEndBeforeTimeDoesNotOverlap() {
    $a = new Availability(array(
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308825900, // 2011-06-23 12:45:00 +0200
      'end_time' => 1308829500, //  2011-06-23 13:45:00 +0200
    ));
    $this->assertFalse($a->isOverlappingRange('13:45', '15:40'));
  }

  function testStartAfterTimeDoesNotOverlap() {
    $a = new Availability(array(
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308825900, // 2011-06-23 12:45:00 +0200
      'end_time' => 1308829500, //  2011-06-23 13:45:00 +0200
    ));
    $this->assertFalse($a->isOverlappingRange('10:45', '12:45'));
  }

  function testCompletelyWithinOverlaps() {
    $a = new Availability(array(
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308825900, // 2011-06-23 12:45:00 +0200
      'end_time' => 1308829500, //  2011-06-23 13:45:00 +0200
    ));
    $this->assertTrue($a->isOverlappingRange('13:00', '13:30'));
  }

  function testWrapsMidnightDoesNotOverlapMorning() {
    $a = new Availability(array(
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308865500, // 2011-06-23 23:45:00 +0200
      'end_time' => 1308897900, //  2011-06-24 08:45:00 +0200
    ));
    $this->assertFalse($a->isOverlappingRange('07:45', '08:45'));
  }

  function testNewDoesNotHaveLocalChangesSet() {
    $a = new Availability(array(
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308865500, // 2011-06-23 23:45:00 +0200
      'end_time' => 1308897900, //  2011-06-24 08:45:00 +0200
    ));
    $this->assertFalse($a->haveLocalChanges());
  }

  function testLocalChangeSetInDBPropagatesToObject() {
    $a = new Availability(array(
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308865500, // 2011-06-23 23:45:00 +0200
      'end_time' => 1308897900, //  2011-06-24 08:45:00 +0200
      'local_changes' => 1,
    ));
    $this->assertTrue($a->haveLocalChanges());
  }

  function testAvailabilityCanSetsLocalChanges() {
    $a = new Availability(array(
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308865500, // 2011-06-23 23:45:00 +0200
      'end_time' => 1308897900, //  2011-06-24 08:45:00 +0200
      'local_changes' => 0,
    ));
    $this->assertFalse($a->haveLocalChanges());
    $a->setLocalChanges();
    $this->assertTrue($a->haveLocalChanges());
  }

  function testSavesLocalChangesToDB() {
    $db = $this->getMock('TZDBWrapper');
    $dbRow = array(
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308865500, // 2011-06-23 23:45:00 +0200
      'end_time' => 1308897900, //  2011-06-24 08:45:00 +0200
      'local_changes' => 0,
    );

    $a = new Availability($dbRow, $db);
    $a->setLocalChanges();

    $expected_db_row = $dbRow;
    $expected_db_row['local_changes'] = 1;

    $db->expects($this->once())
       ->method('writeRecord')
       ->with($this->equalTo('tzavailability'), $this->equalTo($expected_db_row));

    $a->save();
  }
}