<?php

class TZUserJobMappingTest extends PHPUnit_Framework_TestCase {

  public function setUp() {
    $this->expected_uid = 23;
    $this->expected_jobid = 42;
    $this->expected_start_time = new DateTime('now');
    $this->expected_end_time = new DateTime('now');
    $this->mapping = new TZUserJobMapping();
    $this->dbWrapper = $this->getMock('TZDBWrapper');
  }

  public function testSetUid() {
    $this->mapping->setUserID($this->expected_uid);
    $this->assertEquals($this->expected_uid, $this->mapping->getUserID());
  }

  public function testSetJobid() {
    $this->mapping->setJobID($this->expected_jobid);
    $this->assertEquals($this->expected_jobid, $this->mapping->getJobID());
  }

  public function testSetStartTime() {
    $this->mapping->setStartTime($this->expected_start_time);
    $this->assertEquals($this->expected_start_time, $this->mapping->getStartTime());
  }


  public function testSetEndTime() {
    $this->mapping->setEndTime($this->expected_end_time);
    $this->assertEquals($this->expected_end_time, $this->mapping->getEndTime());
  }

  function testCannotSaveUnlessUidAndJobidPresent() {
    try {
      $this->mapping->save();
      $this->fail('should have received validation exception');
    } catch(TZDBValidationException $e) {
      $expected_errors = array (t('Missing user ID'), t('Missing job ID'));
      $this->assertEquals($expected_errors, $e->getErrors());
    }
  }

  function testCannotSaveWithInvertedInterval() {
    try {
      $expected_record = array (
      	'uid' => $this->expected_uid,
        'jobid' => $this->expected_jobid,
        'start_time' => 1308829978, //  2011-06-23 13:52:58 +0200
        'end_time' => 1308825379, // 2011-06-23 12:36:18 +0200
      );

      $mapping = new TZUserJobMapping($expected_record);
      $mapping->setDBWrapper($this->dbWrapper);
      $mapping->save();
      $this->fail('should have received validation exception');
    } catch(TZDBValidationException $e) {
      $expected_errors = array (t('Inverted interval'));
      $this->assertEquals($expected_errors, $e->getErrors());
    }
  }

  function testSaveShouldNotFailValidationForValidMapping() {
    $this->mapping->setDBWrapper($this->dbWrapper);
    $this->mapping->setUserID($this->expected_uid);
    $this->mapping->setJobID($this->expected_jobid);
    try {
      $this->mapping->save();
    } catch (TZDBValidationException $e) {
      $this->fail('should not fail validation');
    }
  }

  function testSaveNewCallsWriteRecordWithRightParameters() {
    $expected_record = array (
    	'uid' => $this->expected_uid,
      'jobid' => $this->expected_jobid,
      'start_time' => 1308825378, // 2011-06-23 12:36:18 +0200
      'end_time' => 1308829978, //  2011-06-23 13:52:58 +0200
    );

    $this->mapping->setDBWrapper($this->dbWrapper);
    $this->mapping->setUserID($this->expected_uid);
    $this->mapping->setJobID($this->expected_jobid);
    $this->mapping->setStartTime(tzbase_make_date($expected_record['start_time']));
    $this->mapping->setEndTime(tzbase_make_date($expected_record['end_time']));

    $this->dbWrapper->expects($this->once())
      ->method('writeRecord')
      ->with('tzusers_tzjobs', $expected_record);

      $this->mapping->save();
  }


  function testSaveNewFillsInIDOnSuccess() {
    $expected_id = 242;
    $expected_record = array (
    	'uid' => $this->expected_uid,
      'jobid' => $this->expected_jobid,
    );

    $written_record = $expected_record;
    $written_record['id'] = $expected_id;

    $this->mapping->setDBWrapper($this->dbWrapper);
    $this->mapping->setUserID($this->expected_uid);
    $this->mapping->setJobID($this->expected_jobid);

    $this->dbWrapper->expects($this->once())
      ->method('writeRecord')
      ->with('tzusers_tzjobs', $expected_record)
      ->will($this->returnValue($written_record));

    $this->mapping->save();
    $this->assertEquals($expected_id, $this->mapping->getId());
  }

  function testConstructorParsesStartAndEndTimesCorrectly() {
    $expected_record = array (
    	'uid' => $this->expected_uid,
      'jobid' => $this->expected_jobid,
      'start_time' => 1308825378, // 2011-06-23 12:36:18 +0200
      'end_time' => 1308829978, //  2011-06-23 13:52:58 +0200
    );
    $mapping = new TZUserJobMapping($expected_record);
    $this->assertEquals(tzbase_make_date($expected_record['start_time']), $mapping->getStartTime());
    $this->assertEquals(tzbase_make_date($expected_record['end_time']), $mapping->getEndTime());
  }

  function testConstructorHandlesEmptyStartAndEndTimesCorrectly() {
    $expected_record = array (
    	'uid' => $this->expected_uid,
      'jobid' => $this->expected_jobid,
    );

    $mapping = new TZUserJobMapping($expected_record);
    $this->assertNull($mapping->getStartTime());
    $this->assertNull($mapping->getEndTime());
  }

  function testMatchingUidAndJobidAndNoTimeLimitAllowsReportCreation() {
    $expected_begintime = 1308825378; // 2011-06-23 12:36:18 +0200
    $expected_record = array (
    	'uid' => $this->expected_uid,
      'jobid' => $this->expected_jobid,
    );

    $mapping = new TZUserJobMapping($expected_record);
    $may_create_report = $mapping->mayCreateReport($this->expected_uid, $this->expected_jobid, $expected_begintime);
    $this->assertTrue($may_create_report);
  }

  function testBeginTimeBeforeIntervalPreventsReportCreation() {
    $expected_begintime = 1308825378; // 2011-06-23 12:36:18 +0200
    $expected_record = array (
    	'uid' => $this->expected_uid,
      'jobid' => $this->expected_jobid,
      'start_time' => 1308825379, // 2011-06-23 12:36:19 +0200
      'end_time' => 1308829978, //  2011-06-23 13:52:58 +0200
    );

    $mapping = new TZUserJobMapping($expected_record);
    $may_create_report = $mapping->mayCreateReport($this->expected_uid, $this->expected_jobid, $expected_begintime);
    $this->assertFalse($may_create_report);
  }

  function testBeginTimeAfterIntervalPreventsReportCreation() {
    $expected_begintime = 1308829979; // 2011-06-23 12:36:19 +0200
    $expected_record = array (
    	'uid' => $this->expected_uid,
      'jobid' => $this->expected_jobid,
      'start_time' => 1308825379, // 2011-06-23 12:36:18 +0200
      'end_time' => 1308829978, //  2011-06-23 13:52:58 +0200
    );

    $mapping = new TZUserJobMapping($expected_record);
    $may_create_report = $mapping->mayCreateReport($this->expected_uid, $this->expected_jobid, $expected_begintime);
    $this->assertFalse($may_create_report);
  }

  function testBeginTimeInsideIntervalAllowsReportCreation() {
    $expected_begintime = 1308829978; // 2011-06-23 12:36:18 +0200
    $expected_record = array (
    	'uid' => $this->expected_uid,
      'jobid' => $this->expected_jobid,
      'start_time' => 1308825379, // 2011-06-23 12:36:18 +0200
      'end_time' => 1308829978, //  2011-06-23 13:52:58 +0200
    );

    $mapping = new TZUserJobMapping($expected_record);
    $may_create_report = $mapping->mayCreateReport($this->expected_uid, $this->expected_jobid, $expected_begintime);
    $this->assertTrue($may_create_report);
  }

}
