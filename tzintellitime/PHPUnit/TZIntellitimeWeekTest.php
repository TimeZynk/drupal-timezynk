<?php

class TZIntellitimeWeekTest extends PHPUnit_Framework_TestCase {
  private function loadHTMLFile($filename) {
    $full_name = dirname(__FILE__) . "/../tests/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    $parser = new TZIntellitimeParser($contents);
    return new TZIntellitimeWeekData($parser);
  }

  public function setUp() {
    $this->account = (object)array(
      'name' => 'Kalle',
      'uid' => 63,
    );
  }

  public function testConstructorThrowsOnNullReports() {
    try {
      $serverInterface = $this->getMock('TZIntellitimeServerInterface');
      $week = new TZIntellitimeWeek(new DateTime('2011-01-25'), $serverInterface, NULL, $this->account);
      $this->fail("Should have caught exception.");
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

  public function testConstructorDoesNotThrowOnEmptyReports() {
    try {
      $serverInterface = $this->getMock('TZIntellitimeServerInterface');
      $week = new TZIntellitimeWeek(new DateTime('2011-01-25'), $serverInterface, array(), $this->account);
      $this->assertInstanceOf('TZIntellitimeWeek', $week);
    } catch (InvalidArgumentException $e) {
      $this->fail('Caught unexpected exception');
    }
  }

  public function testV9SyncSingleOpenUpdate() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-single-open.txt');

    $serverInterface = $this->getMock('TZIntellitimeServerInterface');
    $serverInterface->expects($this->once())
        ->method('refreshWeek')
        ->will($this->returnValue($weekData));

    $weekDataAfterPost = $this->loadHTMLFile('intellitime-v9-timereport-single-done.txt');
    $serverInterface->expects($this->once())
        ->method('postWeek')
        ->with($this->logicalAnd($this->isInstanceOf('TZIntellitimePostData'), $this->classHasAttribute('postData')))
        ->will($this->returnValue($weekDataAfterPost));

    $reports = array(
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30'),
    );

    $week = new TZIntellitimeWeek(new DateTime('2011-01-25'), $serverInterface, $reports, $this->account);

    $syncResult = $week->sync();
    $this->assertNotNull($syncResult);

    $this->assertEquals(1, count($syncResult->tzreports));
    $this->assertReportUpdatedCorrectly($reports[0], $syncResult->tzreports[0]);
    $this->assertReportTitlesInAssignments($syncResult);
  }

  public function testV9SyncSingleOpenNew() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-single-open.txt');

    $serverInterface = $this->getMock('TZIntellitimeServerInterface');
    $serverInterface->expects($this->once())
        ->method('refreshWeek')
        ->will($this->returnValue($weekData));

    $weekDataAfterPost = $this->loadHTMLFile('intellitime-v9-timereport-single-done.txt');
    $serverInterface->expects($this->never())
        ->method('postWeek');

    $reports = array();

    $week = new TZIntellitimeWeek(new DateTime('2011-01-25'), $serverInterface, $reports, $this->account);

    $syncResult = $week->sync();
    $this->assertNotNull($syncResult);

    $this->assertEquals(1, count($syncResult->tzreports));
    $report = $syncResult->tzreports[0];
    $this->assertFalse(isset($report->jobid));
    $this->assertNotNull($syncResult->tzjobs);
    $this->assertReportTitlesInAssignments($syncResult);
  }

  public function testV9SyncFindsTwoNewLockedAndUpdatesTheOneExistingReport() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-locked-week-with-new-report.txt');

    $serverInterface = $this->getMock('TZIntellitimeServerInterface');
    $serverInterface->expects($this->once())
        ->method('refreshWeek')
        ->will($this->returnValue($weekData));

    $weekDataAfterPost = $this->loadHTMLFile('intellitime-v9-timereport-two-locked-one-done.txt');

    $serverInterface->expects($this->once())
        ->method('postWeek')
        ->with($this->logicalAnd($this->isInstanceOf('TZIntellitimePostData'), $this->classHasAttribute('postData')))
        ->will($this->returnValue($weekDataAfterPost));

    $reports = array(
      createMockReport('mhbLP96iqH2II35gn6Dx4M4hbku5Eii3', '2010-09-11', '08:00', '12:00', 15),
    );

    $week = new TZIntellitimeWeek(new DateTime('2010-09-11'), $serverInterface, $reports, $this->account);

    $syncResult = $week->sync();
    $this->assertNotNull($syncResult);

    $this->assertEquals(3, count($syncResult->tzreports));
    $this->assertReportUpdatedCorrectly($reports[0], $syncResult->tzreports[0]);


    $this->assertEquals(TZFlags::LOCKED, $syncResult->tzreports[1]->flags);
    $this->assertEquals(TZFlags::LOCKED, $syncResult->tzreports[2]->flags);
    $this->assertFalse(isset($syncResult->tzreports[1]->nid));
    $this->assertFalse(isset($syncResult->tzreports[2]->nid));
    $this->assertReportTitlesInAssignments($syncResult);
  }

  function testV9ChangingReportedRowsRequiresTwoPosts() {
    $weekData = array(
      $this->loadHTMLFile('Week_v9ChangingReportedRowsRequiresTwoPosts_step_1.txt'),
      $this->loadHTMLFile('Week_v9ChangingReportedRowsRequiresTwoPosts_step_2.txt'),
      $this->loadHTMLFile('Week_v9ChangingReportedRowsRequiresTwoPosts_step_3.txt'),
    );

    $serverInterface = $this->getMock('TZIntellitimeServerInterface');
    $serverInterface->expects($this->once())
        ->method('refreshWeek')
        ->will($this->returnValue($weekData[0]));

    $serverInterface->expects($this->at(1))
        ->method('postWeek')
        ->with($this->attributeEqualTo('intermediate', TRUE))
        ->will($this->returnValue($weekData[1]));

    $serverInterface->expects($this->at(2))
        ->method('postWeek')
        ->with($this->attributeEqualTo('intermediate', FALSE))
        ->will($this->returnValue($weekData[2]));

    $reports = array(
      createMockReport('mhbLP96iqH2ZmvPmA4%2fZ0M4hbku5Eii3', '2011-01-24', '08:00', '17:00', 60),
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30', 60),
      createMockReport('mhbLP96iqH04xVvkWCFl884hbku5Eii3', '2011-01-26', '08:00', '17:00', 60),
    );
    $reports[0]->intellitime_local_changes = 0;
    $reports[2]->intellitime_local_changes = 0;

    $week = new TZIntellitimeWeek(new DateTime('2011-01-25'), $serverInterface, $reports, $this->account);

    $syncResult = $week->sync();
    $this->assertNotNull($syncResult);

    $this->assertEquals(array('2011W01', '2011W02', '2011W03', '2011W04', '2011W05'),
        array_map(function($datetime) {
          return $datetime->format('o\WW');
        }, $syncResult->unfinishedWeeks)
    );
    for($i = 0; $i < count($reports); $i++) {
      $this->assertReportUpdatedCorrectly($reports[$i], $syncResult->tzreports[$i], TRUE);
    }
    $this->assertReportTitlesInAssignments($syncResult);
  }

  public function testV9SyncDeletedOnServer() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-single-open.txt');

    $serverInterface = $this->getMock('TZIntellitimeServerInterface');
    $serverInterface->expects($this->once())
        ->method('refreshWeek')
        ->will($this->returnValue($weekData));

    $weekDataAfterPost = $this->loadHTMLFile('intellitime-v9-timereport-single-done.txt');
    $serverInterface->expects($this->once())
        ->method('postWeek')
        ->with($this->logicalAnd($this->isInstanceOf('TZIntellitimePostData'), $this->classHasAttribute('postData')))
        ->will($this->returnValue($weekDataAfterPost));

    $reports = array(
      createMockReport('NONEXISTANT1', '2011-01-24', '08:00', '17:00'),
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:00', '17:00'),
      createMockReport('NONEXISTANT2', '2011-01-26', '08:00', '17:00'),
    );

    $week = new TZIntellitimeWeek(new DateTime('2011-01-25'), $serverInterface, $reports, $this->account);

    $syncResult = $week->sync();
    $this->assertNotNull($syncResult);

    $this->assertEquals(3, count($syncResult->tzreports));
    $this->assertEquals(
      array('2010W47', '2011W01', '2011W02', '2011W03'),
      array_map(function($datetime) {
          return $datetime->format('o\WW');
        }, $syncResult->unfinishedWeeks)
    );

    $this->assertReportUpdatedCorrectly($reports[1], $syncResult->tzreports[1]);
    $this->assertEquals(TZFlags::DELETED, $syncResult->tzreports[0]->flags);
    $this->assertEquals(TZFlags::DELETED, $syncResult->tzreports[2]->flags);
    $this->assertReportTitlesInAssignments($syncResult);
  }

  public function testV9RestoreStateOnNetworkException() {
    $weekData = array(
      $this->loadHTMLFile('Week_v9ChangingReportedRowsRequiresTwoPosts_step_1.txt'),
      $this->loadHTMLFile('Week_v9ChangingReportedRowsRequiresTwoPosts_step_2.txt'),
      $this->loadHTMLFile('Week_v9ChangingReportedRowsRequiresTwoPosts_step_3.txt'),
    );

    $serverInterface = $this->getMock('TZIntellitimeServerInterface');
    $serverInterface->expects($this->at(0))
        ->method('refreshWeek')
        ->will($this->returnValue($weekData[0]));

    $serverInterface->expects($this->at(1))
        ->method('postWeek')
        ->with($this->attributeEqualTo('intermediate', TRUE))
        ->will($this->returnValue($weekData[1]));

    $serverInterface->expects($this->at(2))
        ->method('postWeek')
        ->with($this->attributeEqualTo('intermediate', FALSE))
        ->will($this->throwException(new TZNetworkFailureException()));

    $reports = array(
      createMockReport('mhbLP96iqH2ZmvPmA4%2fZ0M4hbku5Eii3', '2011-01-24', '08:00', '17:00', 60),
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30', 60),
      createMockReport('mhbLP96iqH04xVvkWCFl884hbku5Eii3', '2011-01-26', '08:00', '17:00', 60),
    );
    $reports[0]->intellitime_local_changes = 0;
    $reports[2]->intellitime_local_changes = 0;

    $week = new TZIntellitimeWeek(new DateTime('2011-01-25'), $serverInterface, $reports, $this->account);

    $syncResult = $week->sync();
    $this->assertNotNull($syncResult);
    $this->assertInstanceOf('TZNetworkFailureException', $syncResult->exception);

    foreach($reports as $i => $report) {
      $updatedReport = $syncResult->tzreports[$i];
      $this->assertEquals($report->vid, $updatedReport->vid);
      $this->assertEquals($report->begintime, $updatedReport->begintime);
      $this->assertEquals($report->endtime, $updatedReport->endtime);
      $this->assertEquals($report->breakduration, $updatedReport->breakduration);
    }

    $this->assertEquals(0, $syncResult->tzreports[0]->intellitime_local_changes);
    $this->assertEquals(1, $syncResult->tzreports[1]->intellitime_local_changes);
    $this->assertEquals(0, $syncResult->tzreports[2]->intellitime_local_changes);

    $this->assertEquals(TZIntellitimeReport::STATE_REPORTED, $syncResult->tzreports[0]->intellitime_last_state);
    $this->assertEquals(TZIntellitimeReport::STATE_OPEN, $syncResult->tzreports[1]->intellitime_last_state);
    $this->assertEquals(TZIntellitimeReport::STATE_OPEN, $syncResult->tzreports[2]->intellitime_last_state);
  }

  private function assertReportUpdatedCorrectly($report, $updatedReport, $checkTimes = FALSE) {
    $this->assertEquals($report->nid, $updatedReport->nid);
    $this->assertEquals($report->vid, $updatedReport->vid);
    $this->assertEquals($report->assignedto, $updatedReport->assignedto);
    $this->assertEquals($report->intellitime_id, $updatedReport->intellitime_id);
    if($checkTimes) {
      $this->assertEquals($report->begintime, $updatedReport->begintime);
      $this->assertEquals($report->endtime, $updatedReport->endtime);
      $this->assertEquals($report->breakduration, $updatedReport->breakduration);
    }
    $this->assertEquals(0, $updatedReport->intellitime_local_changes);
  }

  /**
   * $param TZIntellitimeSyncResult $syncResult
   * $return bool
   */
  private function assertReportTitlesInAssignments($syncResult) {
    foreach ($syncResult->tzreports as $report) {
      if ($report->flags == TZFlags::DELETED) {
        continue;
      }
      $match = FALSE;
      foreach ($syncResult->tzjobs as $tzjob) {
        if ($report->title == $tzjob->jobcode) {
          $match = TRUE;
          break;
        }
      }
      if (!$match) {
        $this->fail('Could not find a match for report title "' . $report->title . '"');
      }
    }
    $this->assertTrue(TRUE, "All reports match");
  }
}