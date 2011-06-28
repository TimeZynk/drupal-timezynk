<?php

class TZIntellitimeUserJobsMappingPolicyTest extends PHPUnit_Framework_TestCase {
  private $account;

  /**
   * @var TZUserJobsMapper
   */
  private $mapper;

  function setUp() {
    $this->account = (object)array(
      'uid' => 123,
    );
    $this->mapper = $this->getMock('TZUserJobsMapper');
    $this->policy = new TZIntellitimeUserJobsMappingPolicy($this->mapper, $this->account);
  }

  function testNoExistingMappingsNoNewMappingsDoesNothing() {
    $expected_mappings = array();
    $start_date = date_make_date('2011-06-23 12:36:18');
    $end_date = date_make_date('2011-06-25 12:36:18');
    $assignments = array();

    $this->mapper->expects($this->once())
      ->method('find')
      ->with($this->account->uid)
      ->will($this->returnValue($expected_mappings));

    $this->policy->resolveMappings($start_date, $end_date, $assignments);
  }

  function testNoExistingMappingsOneNewForbinnedDoesNothing() {
    $expected_mappings = array();
    $start_date = date_make_date('2011-06-23 12:36:18');
    $end_date = date_make_date('2011-06-25 12:36:18');
    $assignments = array(
      '44' => new TZIntellitimeAssignment('my title')
    );

    $this->mapper->expects($this->once())
      ->method('find')
      ->with($this->account->uid)
      ->will($this->returnValue($expected_mappings));

    $this->policy->resolveMappings($start_date, $end_date, $assignments);
  }

  function testNoExistingMappingsOneNewMappingCreatesIt() {
    $expected_jobid = 44;
    $expected_mappings = array();
    $start_date = date_make_date('2011-06-23 12:36:18');
    $end_date = date_make_date('2011-06-25 12:36:18');
    $assignments = array();
    $assignments[$expected_jobid] = new TZIntellitimeAssignment('my title');
    $assignments[$expected_jobid]->setMayCreateReport(TRUE);

    $this->mapper->expects($this->once())
      ->method('find')
      ->with($this->account->uid)
      ->will($this->returnValue($expected_mappings));

    $mock_mapping = $this->getMock('TZUserJobMapping', array('save'));
    $mock_mapping->expects($this->once())
      ->method('save');

    $this->mapper->expects($this->once())
      ->method('createMapping')
      ->will($this->returnValue($mock_mapping));

    $this->policy->resolveMappings($start_date, $end_date, $assignments);
    $this->assertEquals($this->account->uid, $mock_mapping->getUserId());
    $this->assertEquals($expected_jobid, $mock_mapping->getJobId());
    $this->assertEquals($start_date, $mock_mapping->getStartTime());
    $this->assertEquals($end_date, $mock_mapping->getEndTime());
  }

  function testNoExistingMappingsOneNewMappingCreatesItOnceOnly() {
    $expected_jobid = 44;
    $expected_mappings = array();
    $start_date = date_make_date('2011-06-23 12:36:18');
    $end_date = date_make_date('2011-06-25 12:36:18');
    $assignments = array();
    $assignments[$expected_jobid] = new TZIntellitimeAssignment('my title');
    $assignments[$expected_jobid]->setMayCreateReport(TRUE);

    $this->mapper->expects($this->once())
      ->method('find')
      ->with($this->account->uid)
      ->will($this->returnValue($expected_mappings));

    $mock_mapping = $this->getMock('TZUserJobMapping', array('save'));
    $mock_mapping->expects($this->once())
      ->method('save');

    $this->mapper->expects($this->exactly(2))
      ->method('createMapping')
      ->will($this->returnValue($mock_mapping));

    $this->policy->resolveMappings($start_date, $end_date, $assignments);
    $this->policy->resolveMappings($start_date, $end_date, $assignments);
  }


  function testExistingMatchingMappingDoesNotSave() {
    $expected_mapping_id = 88281;
    $expected_jobid = 44;
    $start_date = date_make_date('2011-06-23 12:36:18');
    $end_date = date_make_date('2011-06-25 12:36:18');
    $expected_mapping = new TZUserJobMapping(array(
    	'id' => $expected_mapping_id,
      'uid' => $this->account->uid,
      'jobid' => $expected_jobid,
      'start_time' => $start_date->format('U'),
      'end_time' => $end_date->format('U'),
    ));
    $expected_mappings = array($expected_mapping);
    $assignments = array();
    $assignments[$expected_jobid] = new TZIntellitimeAssignment('my title');
    $assignments[$expected_jobid]->setMayCreateReport(TRUE);

    $this->mapper->expects($this->once())
      ->method('find')
      ->with($this->account->uid)
      ->will($this->returnValue($expected_mappings));

    $mock_mapping = $this->getMock('TZUserJobMapping', array('save'));
    $mock_mapping->expects($this->never())
      ->method('save');

    $this->mapper->expects($this->once())
      ->method('createMapping')
      ->will($this->returnValue($mock_mapping));

    $this->policy->resolveMappings($start_date, $end_date, $assignments);
  }

  function testDeletesMappingsThatAreNotFoundInAssingmentsInThisTimeSpan() {
    $expected_mapping_id = 88281;
    $expected_jobid = 44;
    $start_date = date_make_date('2011-06-23 12:36:18');
    $end_date = date_make_date('2011-06-25 12:36:18');
    $expected_mapping = new TZUserJobMapping(array(
      'id' => $expected_mapping_id,
      'uid' => $this->account->uid,
      'jobid' => $expected_jobid,
      'start_time' => $start_date->format('U'),
      'end_time' => $end_date->format('U'),
    ));
    $expected_mappings = array($expected_mapping);
    $assignments = array();

    $this->mapper->expects($this->once())
      ->method('find')
      ->with($this->account->uid)
      ->will($this->returnValue($expected_mappings));

    $this->mapper->expects($this->once())
      ->method('deleteMapping')
      ->with($expected_mapping_id);

    $this->policy->resolveMappings($start_date, $end_date, $assignments);
  }

  function testResolveMappingsRunTwiceDeletesMappingsOnlyOnce() {
    $expected_mapping_id = 88281;
    $expected_jobid = 44;
    $start_date = date_make_date('2011-06-23 12:36:18');
    $end_date = date_make_date('2011-06-25 12:36:18');
    $expected_mapping = new TZUserJobMapping(array(
      'id' => $expected_mapping_id,
      'uid' => $this->account->uid,
      'jobid' => $expected_jobid,
      'start_time' => $start_date->format('U'),
      'end_time' => $end_date->format('U'),
    ));
    $expected_mappings = array($expected_mapping);
    $assignments = array();

    $this->mapper->expects($this->once())
      ->method('find')
      ->with($this->account->uid)
      ->will($this->returnValue($expected_mappings));

    $this->mapper->expects($this->once())
      ->method('deleteMapping')
      ->with($expected_mapping_id);

    $this->policy->resolveMappings($start_date, $end_date, $assignments);
    $this->policy->resolveMappings($start_date, $end_date, $assignments);
  }



  function testDoesNotDeleteMappingsForSameJobIdInOtherWeeks() {
    $expected_mapping_id = 88281;
    $expected_jobid = 44;
    $start_date = date_make_date('2011-06-23 12:36:18');
    $end_date = date_make_date('2011-06-25 12:36:18');
    $expected_mappings = array(
      new TZUserJobMapping(array(
        'id' => rand(1, 100),
      	'uid' => $this->account->uid,
      	'jobid' => $expected_jobid,
      	'start_time' => $start_date->format('U') - 1,
      	'end_time' => $end_date->format('U'),
      )),
      new TZUserJobMapping(array(
      	'id' => $expected_mapping_id,
      	'uid' => $this->account->uid,
      	'jobid' => $expected_jobid,
      	'start_time' => $start_date->format('U'),
      	'end_time' => $end_date->format('U'),
      )),
      new TZUserJobMapping(array(
        'id' => rand(101, 200),
      	'uid' => $this->account->uid,
      	'jobid' => $expected_jobid,
      	'start_time' => $start_date->format('U'),
      	'end_time' => $end_date->format('U')+1,
      )),

    );
    $assignments = array();

    $this->mapper->expects($this->once())
      ->method('find')
      ->with($this->account->uid)
      ->will($this->returnValue($expected_mappings));

    $this->mapper->expects($this->once())
      ->method('deleteMapping')
      ->with($expected_mapping_id);

    $this->policy->resolveMappings($start_date, $end_date, $assignments);
  }

}