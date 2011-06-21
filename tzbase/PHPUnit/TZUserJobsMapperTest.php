<?php

class TZUserJobsMapperTest extends PHPUnit_Framework_TestCase {
  public function setUp() {
    $this->dbWrapper = $this->getMock('TZDBWrapper');
    $this->mapper = new TZUserJobsMapper($this->dbWrapper);
  }

  public function testGetMappingsWhenFindCalledForUid() {
    $uid = 23;
    $jobid = 42;
    $expected_row = (object)array(
      'id' => 123,
      'uid' => $uid,
      'jobid' => $jobid,
      'start_time' => NULL,
      'end_time' => NULL,
    );

    $expected_mapping = new TZUserJobMapping($expected_row);
    $expected_mapping->setDBWrapper($this->dbWrapper);
    $expected_cursor = new stdClass();

    $this->dbWrapper->expects($this->once())
      ->method('query')
      ->with('SELECT * FROM {tzusers_tzjobs} WHERE uid = %d ORDER BY id', $uid)
      ->will($this->returnValue($expected_cursor));

    $this->dbWrapper->expects($this->exactly(2))
      ->method('fetchObject')
      ->with($expected_cursor)
      ->will($this->onConsecutiveCalls($expected_row, NULL));

    $mappings = $this->mapper->find($uid);
    $this->assertEquals(1, count($mappings));
    $this->assertEquals($expected_mapping, $mappings[0]);
  }

  public function testGetMappingWhenFindCalledForUidAndJobId() {
      $uid = 23;
    $jobid = 42;
    $expected_row = (object)array(
      'id' => 123,
      'uid' => $uid,
      'jobid' => $jobid,
      'start_time' => NULL,
      'end_time' => NULL,
    );

    $expected_mapping = new TZUserJobMapping($expected_row);
    $expected_mapping->setDBWrapper($this->dbWrapper);
    $expected_cursor = new stdClass();

    $this->dbWrapper->expects($this->once())
      ->method('query')
      ->with('SELECT * FROM {tzusers_tzjobs} WHERE uid = %d AND jobid = %d ORDER BY id', $uid, $jobid)
      ->will($this->returnValue($expected_cursor));

    $this->dbWrapper->expects($this->exactly(2))
      ->method('fetchObject')
      ->with($expected_cursor)
      ->will($this->onConsecutiveCalls($expected_row, NULL));

    $mappings = $this->mapper->find($uid, $jobid);
    $this->assertEquals(1, count($mappings));
    $this->assertEquals($expected_mapping, $mappings[0]);
  }

  public function testMayCreateReportIfOneMappingAllowsIt() {
    $expected_uid = 23;
    $expected_jobid = 48;
    $expected_begin_time = 1308829978; //  2011-06-23 13:52:58 +0200
    $expected_cursor = new stdClass();

    $expected_row = array(
      (object)array(
        'id' => 123,
        'uid' => $expected_uid,
        'jobid' => $expected_jobid,
        'start_time' => $expected_begin_time + 1,
        'end_time' => $expected_begin_time + 10,
      ),
      (object)array(
        'id' => 124,
        'uid' => $expected_uid,
        'jobid' => $expected_jobid,
        'start_time' => $expected_begin_time - 1,
        'end_time' => $expected_begin_time,
      ),
    );

    $this->dbWrapper->expects($this->once())
      ->method('query')
      ->with('SELECT * FROM {tzusers_tzjobs} WHERE uid = %d AND jobid = %d ORDER BY id', $expected_uid, $expected_jobid)
      ->will($this->returnValue($expected_cursor));

    $this->dbWrapper->expects($this->exactly(3))
      ->method('fetchObject')
      ->with($expected_cursor)
      ->will($this->onConsecutiveCalls($expected_row[0], $expected_row[1], NULL));

    $may_create_report = $this->mapper->userMayCreateReport($expected_uid, $expected_jobid, $expected_begin_time);
    $this->assertTrue($may_create_report);
  }

  public function testMayNotCreateReportMapperCannotFindMappingsForPeriod() {
    $expected_uid = 23;
    $expected_jobid = 48;
    $expected_begin_time = 1308829978; //  2011-06-23 13:52:58 +0200
    $expected_cursor = new stdClass();

    $this->dbWrapper->expects($this->once())
      ->method('query')
      ->with('SELECT * FROM {tzusers_tzjobs} WHERE uid = %d AND jobid = %d ORDER BY id', $expected_uid, $expected_jobid)
      ->will($this->returnValue($expected_cursor));

    $this->dbWrapper->expects($this->once())
      ->method('fetchObject')
      ->with($expected_cursor)
      ->will($this->returnValue(NULL));

    $may_create_report = $this->mapper->userMayCreateReport($expected_uid, $expected_jobid, $expected_begin_time);
    $this->assertFalse($may_create_report);
  }


  public function testGetAllMappings() {
    $expected_row = (object)array(
      'id' => 123,
      'uid' => $uid,
      'jobid' => $jobid,
      'start_time' => NULL,
      'end_time' => NULL,
    );

    $expected_mapping = new TZUserJobMapping($expected_row);
    $expected_mapping->setDBWrapper($this->dbWrapper);
    $expected_cursor = new stdClass();

    $this->dbWrapper->expects($this->once())
      ->method('query')
      ->with('SELECT * FROM {tzusers_tzjobs} ORDER BY id')
      ->will($this->returnValue($expected_cursor));

    $this->dbWrapper->expects($this->exactly(2))
      ->method('fetchObject')
      ->with($expected_cursor)
      ->will($this->onConsecutiveCalls($expected_row, NULL));

    $mappings = $this->mapper->findAll();
    $this->assertEquals(1, count($mappings));
    $this->assertEquals($expected_mapping, $mappings[0]);
  }

  public function testCreateJobMappingWithCorrectDBWrapper() {
    $mapping = $this->mapper->createMapping();
    $this->assertTrue($mapping instanceof TZUserJobMapping);
  }

  public function testDeleteJobMapping() {
    $expected_table = 'tzusers_tzjobs';
    $expected_id = 123;

    $this->dbWrapper->expects($this->once())
      ->method('delete')
      ->with($expected_table, $expected_id);

    $this->mapper->deleteMapping($expected_id);
  }

  public function testDeleteByJobID() {
    $expected_jobid = 445;

    $this->dbWrapper->expects($this->once())
      ->method('query')
      ->with('DELETE FROM {tzusers_tzjobs} WHERE jobid = %d', $expected_jobid)
      ->will($this->returnValue(1));

    $this->mapper->deleteAllByJobID($expected_jobid);
  }

  public function testDeleteByUserID() {
    $expected_uid = 445;

    $this->dbWrapper->expects($this->once())
      ->method('query')
      ->with('DELETE FROM {tzusers_tzjobs} WHERE uid = %d', $expected_uid)
      ->will($this->returnValue(1));

    $this->mapper->deleteAllByUserID($expected_uid);
  }
}
