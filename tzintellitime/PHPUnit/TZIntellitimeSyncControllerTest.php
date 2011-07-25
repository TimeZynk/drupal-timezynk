<?php

class TZIntellitimeSyncControllerTest extends PHPUnit_Framework_TestCase {

  /**
   *
   * @var TZIntellitimeSyncPolicy
   */
  private $syncPolicy;
  /**
   *
   * @var TZIntellitimeReportStorage
   */
  private $reportStorage;
  /**
   *
   * @var TZIntellitimeWeekFactory
   */
  private $weekFactory;

  /**
   * @var TZIntellitimeUserJobsMappingPolicy
   */
  private $mappingPolicy;

  private $timezone;

  function setUp() {
    $this->syncPolicy = $this->getMock('TZIntellitimeSyncPolicy');
    $this->reportStorage = $this->getMock('TZIntellitimeReportStorage');
    $this->weekFactory = $this->getMock('TZIntellitimeWeekFactory');
    $this->mappingPolicy = $this->getMock('TZIntellitimeUserJobsMappingPolicy');
    $this->syncController = new TZIntellitimeSyncController($this->syncPolicy, $this->weekFactory, $this->reportStorage, $this->mappingPolicy);
    $this->timezone = date_default_timezone(FALSE);
    $this->clone_lambda = function($o) { return clone($o); };
  }

  public function testSyncZeroWeeks() {
    $this->syncPolicy->expects($this->once())
        ->method('getNextWeekToSync')
        ->will($this->returnValue(NULL));

    $this->reportStorage->expects($this->never())
        ->method('getReports');

    $this->weekFactory->expects($this->never())
        ->method('createWeek');

    $this->mappingPolicy->expects($this->never())
      ->method('resolveMapping');

    $status = $this->syncController->synchronize();
    $this->assertEquals(TZIntellitimeSyncController::SYNC_OK, $status);
  }

  public function testRegisteredLoggerReceivesLogMessages() {
    $loggerMock = $this->getMock('TZIntellitimeLogger');
    $this->syncController->registerLogger($loggerMock);
    $expectedDate = new DateTime('2011-01-25', $this->timezone);
    $this->syncPolicy->expects($this->exactly(2))
        ->method('getNextWeekToSync')
        ->will($this->onConsecutiveCalls($expectedDate, NULL));

    $expectedMonday = new DateTime('2011-01-24T00:00:00.000', $this->timezone);
    $expectedSunday = new DateTime('2011-01-31T00:00:00.000', $this->timezone);
    $expectedReports = array();

    $this->reportStorage->expects($this->once())
        ->method('getTZReports')
        ->with($expectedMonday, $expectedSunday)
        ->will($this->returnValue($expectedReports));

    $expectedWeek = $this->getMockBuilder('TZIntellitimeWeek')
        ->disableOriginalConstructor()
        ->getMock();

    $expectedException = new TZIntellitimeInconsistentPost('test');
    $expectedWeek->expects($this->once())
        ->method('sync')
        ->will($this->throwException($expectedException));

    $this->weekFactory->expects($this->once())
        ->method('createWeek')
        ->with($expectedDate, $expectedReports)
        ->will($this->returnValue($expectedWeek));

    $loggerMock->expects($this->once())
        ->method('logException')
        ->with($this->stringContains('Inconsistent post'), $this->equalTo($expectedException), $this->equalTo(TZIntellitimeLogger::ALERT));

    $this->syncController->synchronize();
  }

  public function testTwoRegisteredLoggersReceivesLogMessages() {
    $loggerMock = $this->getMock('TZIntellitimeLogger');
    $loggerMock2 = $this->getMock('TZIntellitimeLogger');
    $composite = new TZCompositeLogger();
    $composite->add($loggerMock);
    $composite->add($loggerMock2);
    $this->syncController->registerLogger($composite);

    $expectedDate = new DateTime('2011-01-25', $this->timezone);
    $this->syncPolicy->expects($this->exactly(2))
        ->method('getNextWeekToSync')
        ->will($this->onConsecutiveCalls($expectedDate, NULL));

    $expectedMonday = new DateTime('2011-01-24T00:00:00.000', $this->timezone);
    $expectedSunday = new DateTime('2011-01-31T00:00:00.000', $this->timezone);
    $expectedReports = array();

    $this->reportStorage->expects($this->once())
        ->method('getTZReports')
        ->with($expectedMonday, $expectedSunday)
        ->will($this->returnValue($expectedReports));

    $expectedWeek = $this->getMockBuilder('TZIntellitimeWeek')
        ->disableOriginalConstructor()
        ->getMock();

    $expectedException = new TZIntellitimeInconsistentPost('test');
    $expectedWeek->expects($this->once())
        ->method('sync')
        ->will($this->throwException($expectedException));

    $this->weekFactory->expects($this->once())
        ->method('createWeek')
        ->with($expectedDate, $expectedReports)
        ->will($this->returnValue($expectedWeek));

    $loggerMock->expects($this->once())
        ->method('logException')
        ->with($this->stringContains('Inconsistent post'), $this->equalTo($expectedException), $this->equalTo(TZIntellitimeLogger::ALERT));
    $loggerMock2->expects($this->once())
        ->method('logException')
        ->with($this->stringContains('Inconsistent post'), $this->equalTo($expectedException), $this->equalTo(TZIntellitimeLogger::ALERT));

    $this->syncController->synchronize();
  }

  public function testSyncSingleWeek() {
    $testDescription = new stdClass();
    $testDescription->date = '2011-01-25';
    $testDescription->expectedReports = array();
    $testDescription->syncResult = new TZIntellitimeWeekSyncResult();

    $syncedTzreports = array(
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30', 60, 0),
    );
    unset($syncedTzreports[0]->nid);
    unset($syncedTzreports[0]->vid);
    unset($syncedTzreports[0]->jobid);

    $testDescription->syncResult->tzreports = $syncedTzreports;

    $this->setupTestFixture($testDescription);
    $status = $this->syncController->synchronize();
    $this->assertEquals(TZIntellitimeSyncController::SYNC_OK, $status);
  }

  public function testSyncSingleWeekWithPreviousReportsNoUpdatesNeeded() {
    $testDescription = new stdClass();
    $testDescription->date = '2011-01-25';
    $testDescription->expectedReports = array(
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30', 60, 0),
    );
    $testDescription->expectedReports[0]->jobid = 2;

    $testDescription->syncResult = new TZIntellitimeWeekSyncResult();
    $testDescription->syncResult->tzreports = array_map($this->clone_lambda, $testDescription->expectedReports);
    $testDescription->mappedReports = array();

    $this->setupTestFixture($testDescription);
    $status = $this->syncController->synchronize();
    $this->assertEquals(TZIntellitimeSyncController::SYNC_OK, $status);
  }

  public function testSyncSingleWeekWithPreviousReportsUpdatesNeeded() {
    $testDescription = new stdClass();
    $testDescription->date = '2011-01-25';
    $testDescription->expectedReports = array(
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30', 60, 0),
    );
    $testDescription->expectedReports[0]->jobid = 2;

    $testDescription->syncResult = new TZIntellitimeWeekSyncResult();
    $testDescription->syncResult->tzreports = array_map($this->clone_lambda, $testDescription->expectedReports);
    $testDescription->syncResult->tzreports[0]->begintime += 1800;

    $this->setupTestFixture($testDescription);
    $status = $this->syncController->synchronize();
    $this->assertEquals(TZIntellitimeSyncController::SYNC_OK, $status);
  }

  public function testSyncSingleWeekWithPreviousReportsInvalidNID() {
    $testDescription = new stdClass();
    $testDescription->date = '2011-01-25';
    $testDescription->expectedReports = array(
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30', 60, 0),
    );
    $testDescription->expectedReports[0]->jobid = 2;

    $testDescription->syncResult = new TZIntellitimeWeekSyncResult();
    $testDescription->syncResult->tzreports = array_map($this->clone_lambda, $testDescription->expectedReports);
    $testDescription->syncResult->tzreports[0]->nid = 1234;
    $testDescription->mappedReports = array();

    $this->setupTestFixture($testDescription);
    $status = $this->syncController->synchronize();
    $this->assertEquals(TZIntellitimeSyncController::SYNC_OK, $status);
  }

  public function testSyncTwoWeeks() {
    $expectedDates = array(
      new DateTime('2011-01-25', $this->timezone),
      new DateTime('2011-02-01', $this->timezone),
    );
    $this->syncPolicy->expects($this->exactly(3))
        ->method('getNextWeekToSync')
        ->will($this->onConsecutiveCalls(
            $expectedDates[0],
            $expectedDates[1],
            NULL
        ));

    $expectedMondays = array(
      new DateTime('2011-01-24T00:00:00.000', $this->timezone),
      new DateTime('2011-01-31T00:00:00.000', $this->timezone),
    );
    $expectedSundays = array(
      new DateTime('2011-01-31T00:00:00.000', $this->timezone),
      new DateTime('2011-02-07T00:00:00.000', $this->timezone),
    );
    $expectedReports = array(
      array(),
      array(),
    );

    $this->reportStorage->expects($this->at(0))
        ->method('getTZReports')
        ->with($expectedMondays[0], $expectedSundays[0])
        ->will($this->returnValue($expectedReports[0]));

    /*
     * storeTZJobs and storeTZReports get in between, which
     * increases the sequential call index by two.
     */
    $this->reportStorage->expects($this->at(3))
        ->method('getTZReports')
        ->with($expectedMondays[1], $expectedSundays[1])
        ->will($this->returnValue($expectedReports[1]));

    $expectedWeek = $this->getMockBuilder('TZIntellitimeWeek')
        ->disableOriginalConstructor()
        ->getMock();

    $syncResult = new TZIntellitimeWeekSyncResult();

    $syncResult->intellitime_assignments = array(
      $this->createMockAssignment('Assignment Title', 'title'),
    );

    $syncResult->tzreports = array(
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30', 60, 0),
    );
    $syncResult->tzreports[0]->intellitime_jobid = $syncResult->intellitime_assignments[0]->id;
    unset($syncResult->tzreports[0]->nid);
    unset($syncResult->tzreports[0]->vid);
    unset($syncResult->tzreports[0]->jobid);


    $syncResult->unfinishedWeeks = array();

    $expectedWeek->expects($this->exactly(2))
        ->method('sync')
        ->will($this->returnValue($syncResult));

    $this->weekFactory->expects($this->at(0))
        ->method('createWeek')
        ->with($expectedDates[0], $expectedReports[0])
        ->will($this->returnValue($expectedWeek));

    $this->weekFactory->expects($this->at(1))
        ->method('createWeek')
        ->with($expectedDates[1], $expectedReports[1])
        ->will($this->returnValue($expectedWeek));

    $storedAssignments = array_map($this->clone_lambda, $syncResult->getTZJobs());
    $storedAssignments[0]->nid = 2;
    $storedAssignments[0]->vid = 3;
    $storedAssignments[0]->parentid = 0;

    $this->reportStorage->expects($this->exactly(2))
        ->method('storeTZJobs')
        ->with($syncResult->getTZJobs())
        ->will($this->returnValue($storedAssignments));

    $mappedReports = array_map($this->clone_lambda, $syncResult->tzreports);
    $mappedReports[0]->jobid = 2;
    unset($mappedReports[0]->intellitime_jobid);

    $this->reportStorage->expects($this->exactly(2))
        ->method('storeTZReports')
        ->with($mappedReports);

    $status = $this->syncController->synchronize();
    $this->assertEquals(TZIntellitimeSyncController::SYNC_OK, $status);

  }

  public function testCatchAuthException() {
    $expectedDate = new DateTime('2011-01-25', $this->timezone);
    $this->syncPolicy->expects($this->once())
        ->method('getNextWeekToSync')
        ->will($this->returnValue($expectedDate));

    $expectedMonday = new DateTime('2011-01-24T00:00:00.000', $this->timezone);
    $expectedSunday = new DateTime('2011-01-31T00:00:00.000', $this->timezone);
    $expectedReports = array();

    $this->reportStorage->expects($this->once())
        ->method('getTZReports')
        ->with($expectedMonday, $expectedSunday)
        ->will($this->returnValue($expectedReports));

    $expectedWeek = $this->getMockBuilder('TZIntellitimeWeek')
        ->disableOriginalConstructor()
        ->getMock();

    $expectedWeek->expects($this->once())
        ->method('sync')
        ->will($this->throwException(new TZAuthenticationFailureException('test')));

    $this->weekFactory->expects($this->once())
        ->method('createWeek')
        ->with($expectedDate, $expectedReports)
        ->will($this->returnValue($expectedWeek));

    $status = $this->syncController->synchronize();
    $this->assertEquals(TZIntellitimeSyncController::AUTH_FAILURE, $status);
  }

  public function testReactToReturnedAuthException() {
    $expectedDate = new DateTime('2011-01-25', $this->timezone);
    $this->syncPolicy->expects($this->once())
        ->method('getNextWeekToSync')
        ->will($this->returnValue($expectedDate));

    $expectedMonday = new DateTime('2011-01-24T00:00:00.000', $this->timezone);
    $expectedSunday = new DateTime('2011-01-31T00:00:00.000', $this->timezone);
    $expectedReports = array();

    $syncResult = new TZIntellitimeWeekSyncResult();

    $syncResult->intellitime_assignments = array(
      $this->createMockAssignment('Assignment Title', 'title'),
    );

    $syncResult->tzreports = array(
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30', 60, 0),
    );
    $syncResult->tzreports[0]->intellitime_jobid = $syncResult->intellitime_assignments[0]->id;
    unset($syncResult->tzreports[0]->nid);
    unset($syncResult->tzreports[0]->vid);
    unset($syncResult->tzreports[0]->jobid);

    $syncResult->unfinishedWeeks = array();
    $syncResult->exception = new TZAuthenticationFailureException("test");

    $this->reportStorage->expects($this->once())
        ->method('getTZReports')
        ->with($expectedMonday, $expectedSunday)
        ->will($this->returnValue($expectedReports));

    $storedAssignments = array_map($this->clone_lambda, $syncResult->getTZJobs());
    $storedAssignments[0]->nid = 2;
    $storedAssignments[0]->vid = 3;
    $storedAssignments[0]->parentid = 0;

    $this->reportStorage->expects($this->once())
        ->method('storeTZJobs')
        ->with($syncResult->getTZJobs())
        ->will($this->returnValue($storedAssignments));

    $mappedReports = array_map($this->clone_lambda, $syncResult->tzreports);
    $mappedReports[0]->jobid = 2;
    unset($mappedReports[0]->intellitime_jobid);

    $this->reportStorage->expects($this->once())
        ->method('storeTZReports')
        ->with($mappedReports);

    $expectedWeek = $this->getMockBuilder('TZIntellitimeWeek')
        ->disableOriginalConstructor()
        ->getMock();

    $expectedWeek->expects($this->once())
        ->method('sync')
        ->will($this->returnValue($syncResult));

    $this->weekFactory->expects($this->once())
        ->method('createWeek')
        ->with($expectedDate, $expectedReports)
        ->will($this->returnValue($expectedWeek));

    $status = $this->syncController->synchronize();
    $this->assertEquals(TZIntellitimeSyncController::AUTH_FAILURE, $status);
  }

  public function testReactToReturnedNetException() {
    $expectedDate = new DateTime('2011-01-25', $this->timezone);
    $this->syncPolicy->expects($this->exactly(2))
        ->method('getNextWeekToSync')
        ->will($this->onConsecutiveCalls($expectedDate, NULL));

    $expectedMonday = new DateTime('2011-01-24T00:00:00.000', $this->timezone);
    $expectedSunday = new DateTime('2011-01-31T00:00:00.000', $this->timezone);
    $expectedReports = array();

    $syncResult = new TZIntellitimeWeekSyncResult();

    $syncResult->intellitime_assignments = array(
      $this->createMockAssignment('Assignment Title', 'title'),
    );

    $syncResult->tzreports = array(
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30', 60, 0),
    );
    $syncResult->tzreports[0]->intellitime_jobid = $syncResult->intellitime_assignments[0]->id;
    unset($syncResult->tzreports[0]->nid);
    unset($syncResult->tzreports[0]->vid);
    unset($syncResult->tzreports[0]->jobid);

    $syncResult->unfinishedWeeks = array();
    $syncResult->exception = new TZNetworkFailureException("test");

    $this->reportStorage->expects($this->once())
        ->method('getTZReports')
        ->with($expectedMonday, $expectedSunday)
        ->will($this->returnValue($expectedReports));

    $storedAssignments = array_map($this->clone_lambda, $syncResult->getTZJobs());
    $storedAssignments[0]->nid = 2;
    $storedAssignments[0]->vid = 3;
    $storedAssignments[0]->parentid = 0;

    $this->reportStorage->expects($this->once())
        ->method('storeTZJobs')
        ->with($syncResult->getTZJobs())
        ->will($this->returnValue($storedAssignments));

    $mappedReports = array_map($this->clone_lambda, $syncResult->tzreports);
    $mappedReports[0]->jobid = 2;
    unset($mappedReports[0]->intellitime_jobid);

    $this->reportStorage->expects($this->once())
        ->method('storeTZReports')
        ->with($mappedReports);

    $expectedWeek = $this->getMockBuilder('TZIntellitimeWeek')
        ->disableOriginalConstructor()
        ->getMock();


    $expectedWeek->expects($this->once())
        ->method('sync')
        ->will($this->returnValue($syncResult));

    $this->weekFactory->expects($this->once())
        ->method('createWeek')
        ->with($expectedDate, $expectedReports)
        ->will($this->returnValue($expectedWeek));

    $status = $this->syncController->synchronize();
    $this->assertEquals(TZIntellitimeSyncController::NETWORK_FAILURE, $status);
  }

  public function testCatchNetworkFailureException() {
    $expectedDate = new DateTime('2011-01-25', $this->timezone);
    $this->syncPolicy->expects($this->at(0))
        ->method('getNextWeekToSync')
        ->will($this->returnValue($expectedDate));

    $this->syncPolicy->expects($this->at(1))
        ->method('getNextWeekToSync')
        ->will($this->returnValue(NULL));

    $expectedMonday = new DateTime('2011-01-24T00:00:00.000', $this->timezone);
    $expectedSunday = new DateTime('2011-01-31T00:00:00.000', $this->timezone);
    $expectedReports = array();

    $this->reportStorage->expects($this->once())
        ->method('getTZReports')
        ->with($expectedMonday, $expectedSunday)
        ->will($this->returnValue($expectedReports));

    $expectedWeek = $this->getMockBuilder('TZIntellitimeWeek')
        ->disableOriginalConstructor()
        ->getMock();

    $expectedWeek->expects($this->once())
        ->method('sync')
        ->will($this->throwException(new TZNetworkFailureException('test')));

    $this->weekFactory->expects($this->once())
        ->method('createWeek')
        ->with($expectedDate, $expectedReports)
        ->will($this->returnValue($expectedWeek));

    $status = $this->syncController->synchronize();
    $this->assertEquals(TZIntellitimeSyncController::NETWORK_FAILURE, $status);
  }

  public function testSyncUpdatesUnfinishedWeeksInfo() {
    $expectedDate = new DateTime('2011-01-25', $this->timezone);
    $expectedMonday = new DateTime('2011-01-24T00:00:00.000', $this->timezone);
    $expectedSunday = new DateTime('2011-01-31T00:00:00.000', $this->timezone);
    $expectedReports = array();

    $syncResult = new TZIntellitimeWeekSyncResult();
    $syncResult->intellitime_assignments = array(
      $this->createMockAssignment('Assignment Title', 'title'),
    );
    $syncResult->tzreports = array(
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30', 60, 0),
    );
    $syncResult->tzreports[0]->intellitime_jobid = $syncResult->intellitime_assignments[0]->id;
    unset($syncResult->tzreports[0]->nid);
    unset($syncResult->tzreports[0]->vid);
    unset($syncResult->tzreports[0]->jobid);

    $syncResult->unfinishedWeeks = array(
      new DateTime('2011-02-22', $this->timezone),
    );
    $storedAssignments = array_map($this->clone_lambda, $syncResult->getTZJobs());
    $storedAssignments[0]->nid = 2;
    $storedAssignments[0]->vid = 3;
    $storedAssignments[0]->parentid = 0;


    $mappedReports = array_map($this->clone_lambda, $syncResult->tzreports);
    $mappedReports[0]->jobid = 2;
    unset($mappedReports[0]->intellitime_jobid);

    $this->syncPolicy->expects($this->exactly(2))
        ->method('getNextWeekToSync')
        ->will($this->onConsecutiveCalls(
            $expectedDate,
            NULL
    ));

    $this->syncPolicy->expects($this->once())
      ->method('addWeeks')
      ->with($syncResult->unfinishedWeeks);

    $this->reportStorage->expects($this->once())
        ->method('getTZReports')
        ->with($expectedMonday, $expectedSunday)
        ->will($this->returnValue($expectedReports));

    $expectedWeek = $this->getMockBuilder('TZIntellitimeWeek')
        ->disableOriginalConstructor()
        ->getMock();

    $expectedWeek->expects($this->once())
        ->method('sync')
        ->will($this->returnValue($syncResult));

    $this->weekFactory->expects($this->once())
        ->method('createWeek')
        ->with($expectedDate, $expectedReports)
        ->will($this->returnValue($expectedWeek));

    $this->reportStorage->expects($this->once())
        ->method('storeTZJobs')
        ->with($syncResult->getTZJobs())
        ->will($this->returnValue($storedAssignments));

    $this->reportStorage->expects($this->once())
        ->method('storeTZReports')
        ->with($mappedReports);

    $status = $this->syncController->synchronize();
    $this->assertEquals(TZIntellitimeSyncController::SYNC_OK, $status);
  }

  public function testSyncReturnsNull() {
    $expectedDate = new DateTime('2011-01-25', $this->timezone);
    $this->syncPolicy->expects($this->at(0))
        ->method('getNextWeekToSync')
        ->will($this->returnValue($expectedDate));

    $this->syncPolicy->expects($this->at(1))
        ->method('getNextWeekToSync')
        ->will($this->returnValue(NULL));

    $expectedMonday = new DateTime('2011-01-24T00:00:00.000', $this->timezone);
    $expectedSunday = new DateTime('2011-01-31T00:00:00.000', $this->timezone);
    $expectedReports = array();

    $this->reportStorage->expects($this->once())
        ->method('getTZReports')
        ->with($expectedMonday, $expectedSunday)
        ->will($this->returnValue($expectedReports));

    $expectedWeek = $this->getMockBuilder('TZIntellitimeWeek')
        ->disableOriginalConstructor()
        ->getMock();

    $expectedWeek->expects($this->once())
        ->method('sync')
        ->will($this->returnValue(NULL));

    $this->weekFactory->expects($this->once())
        ->method('createWeek')
        ->with($expectedDate, $expectedReports)
        ->will($this->returnValue($expectedWeek));

    $this->reportStorage->expects($this->never())
        ->method('storeTZJobs');
    $this->reportStorage->expects($this->never())
        ->method('storeTZReports');

    $status = $this->syncController->synchronize();
    $this->assertEquals(TZIntellitimeSyncController::NETWORK_FAILURE, $status);
  }

  public function testCatchErrorPageException() {
    $expectedDate = new DateTime('2011-01-25', $this->timezone);
    $this->syncPolicy->expects($this->at(0))
        ->method('getNextWeekToSync')
        ->will($this->returnValue($expectedDate));

    $this->syncPolicy->expects($this->at(1))
        ->method('getNextWeekToSync')
        ->will($this->returnValue(NULL));

    $expectedMonday = new DateTime('2011-01-24T00:00:00.000', $this->timezone);
    $expectedSunday = new DateTime('2011-01-31T00:00:00.000', $this->timezone);
    $expectedReports = array();

    $this->reportStorage->expects($this->once())
        ->method('getTZReports')
        ->with($expectedMonday, $expectedSunday)
        ->will($this->returnValue($expectedReports));

    $expectedWeek = $this->getMockBuilder('TZIntellitimeWeek')
        ->disableOriginalConstructor()
        ->getMock();

    $expectedWeek->expects($this->once())
        ->method('sync')
        ->will($this->throwException(new TZIntellitimeErrorPageException('test')));

    $this->weekFactory->expects($this->once())
        ->method('createWeek')
        ->with($expectedDate, $expectedReports)
        ->will($this->returnValue($expectedWeek));

    $status = $this->syncController->synchronize();
    $this->assertEquals(TZIntellitimeSyncController::NETWORK_FAILURE, $status);
  }

  private function setupTestFixture($testDescriptions) {
    if(!is_array($testDescriptions)) {
      $testDescriptions = array($testDescriptions);
    }

    // Each key in $testDescription is the week date
    $index = 0;
    foreach($testDescriptions as $test) {
      $date = new DateTime($test->date, $this->timezone);

      $this->syncPolicy->expects($this->at(2 * $index))
          ->method('getNextWeekToSync')
          ->will($this->returnValue($date));

      $expectedDateRange = tzintellitime_week_span($date);

      /*
       * storeTZJobs and storeTZReports get in between, which
       * increases the sequential call index by two.
       */
      $this->reportStorage->expects($this->at(3 * $index))
          ->method('getTZReports')
          ->with($expectedDateRange[0], $expectedDateRange[1])
          ->will($this->returnValue($test->expectedReports));

      $expectedWeek = $this->getMockBuilder('TZIntellitimeWeek')
          ->disableOriginalConstructor()
          ->getMock();

      if(!isset($test->syncResult->intellitime_assignments)) {
        $test->syncResult->intellitime_assignments = array(
          $this->createMockAssignment('Assignment Title', 'title'),
        );
      }

      foreach($test->syncResult->tzreports as $tzreport) {
        $tzreport->intellitime_jobid = $test->syncResult->intellitime_assignments[0]->id;
      }

      if(!isset($test->syncResult->unfinishedWeeks)) {
        $test->syncResult->unfinishedWeeks = array();
      }

      $expectedWeek->expects($this->at($index))
          ->method('sync')
          ->will($this->returnValue($test->syncResult));

      $this->weekFactory->expects($this->at($index))
          ->method('createWeek')
          ->with($date, $test->expectedReports)
          ->will($this->returnValue($expectedWeek));

      $storedAssignments = array_map($this->clone_lambda, $test->syncResult->getTZJobs());
      foreach($storedAssignments as $assignment_key => $assignment) {
        $assignment->nid = 2 + $assignment_key;
        $assignment->vid = 3 + $assignment_key;
        $assignment->parentid = 0;
      }

      $this->reportStorage->expects($this->at(1 + 3 * $index))
          ->method('storeTZJobs')
          ->with($test->syncResult->getTZJobs())
          ->will($this->returnValue($storedAssignments));

      if(!isset($test->mappedReports)) {
        $test->mappedReports = array_map($this->clone_lambda, $test->syncResult->tzreports);
      }

      foreach($test->mappedReports as $tzreport) {
        $tzreport->jobid = $storedAssignments[0]->nid;
        unset($tzreport->intellitime_jobid);
      }

      $this->reportStorage->expects($this->at(2 + 3 * $index))
        ->method('storeTZReports')
        ->with($test->mappedReports);

      $this->mappingPolicy->expects($this->at($index))
        ->method('resolveMappings')
        ->with($expectedDateRange[0], $expectedDateRange[1], $this->isType('array'));

      $index++;
    }

    $this->syncPolicy->expects($this->at(2 * $index))
        ->method('getNextWeekToSync')
        ->will($this->returnValue(NULL));
  }

  private function createMockAssignment($title, $report_key) {
    return new TZIntellitimeAssignment($title, $report_key, rand(1000, 10000));
  }
}
