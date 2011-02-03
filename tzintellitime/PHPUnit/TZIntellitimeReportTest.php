<?php

class TZIntellitimeReportTest extends PHPUnit_Framework_TestCase {
  private $report = NULL;

  public function setUp() {
    $this->originalTimeZone = date_default_timezone_name();
    date_default_timezone_set('Europe/Stockholm');
    $this->report = new TZIntellitimeReport();
  }

  public function tearDown() {
    parent::tearDown();
    date_default_timezone_set($this->originalTimeZone);
    $this->report = NULL;
  }

  public function testComparisonIdentical() {
    $other_report = new TZIntellitimeReport();
    _tzintellitime_populate_report($this->report);
    _tzintellitime_populate_report($other_report);

    $this->assertEquals($this->report, $other_report, 'Expected the two reports to be considered equal.');
  }

  public function testComparisonDifferent() {
    $other_report = new TZIntellitimeReport();
    _tzintellitime_populate_report($this->report);
    _tzintellitime_populate_report($other_report);
    $other_report->overtime_hours = 6;

    $this->assertNotEquals($this->report, $other_report, 'Expected the two reports to not be considered equal.');
  }

  public function testConvertToTZReport() {
    _tzintellitime_populate_report($this->report);
    $mockaccount = (object)array('uid' => '1234', 'name' => 'Mock User');
    $tzreport = $this->report->convert_to_tzreport($mockaccount);
    $this->assertNotNull($tzreport, 'Expected $tzreport not NULL.');
    $this->assertEquals($this->report->title, $tzreport->title, "Expected equal titles");
    $this->assertEquals($this->report->comment, $tzreport->body, "Expected equal comments");
    $this->assertEquals(1284530400, $tzreport->begintime, 'Expected 1284530400 as begintime, got ' . $tzreport->begintime);
    $this->assertEquals(1284566400, $tzreport->endtime, 'Expected 1284566400 as endtime, got ' . $tzreport->endtime);
    $this->assertEquals($this->report->break_duration_minutes*60, $tzreport->breakduration, "Expected break durations to match");
  }

  public function testConvertToTZReportWithEndTimeOnNextDay() {
    _tzintellitime_populate_report($this->report);
    $mockaccount = (object)array('uid' => '1234', 'name' => 'Mock User');
    $this->report->begin="12:00";
    $this->report->end="11:00";
    $tzreport = $this->report->convert_to_tzreport($mockaccount);
    $this->assertNotNull($tzreport, 'Expected $tzreport not NULL.');
    $this->assertEquals($this->report->title, $tzreport->title, "Expected equal titles");
    $this->assertEquals($this->report->comment, $tzreport->body, "Expected equal comments");
    $this->assertEquals(1284544800, $tzreport->begintime);
    $this->assertEquals(1284627600, $tzreport->endtime);
    $this->assertEquals($this->report->break_duration_minutes*60, $tzreport->breakduration, "Expected break durations to match");
  }


  public function testUpdateTZReport() {
    _tzintellitime_populate_report($this->report);
    $mockaccount = (object)array('uid' => '1234', 'name' => 'Mock User');
    $tzreport = $this->report->convert_to_tzreport($mockaccount);
    $tzreport->nid = 12345;
    $this->report->break_duration_minutes = 35;
    $tzreport = $this->report->convert_to_tzreport($mockaccount, $tzreport);
    $this->assertNotNull($tzreport, 'Expected $tzreport not NULL.');
    $this->assertEquals(12345, $tzreport->nid);
    $this->assertEquals($this->report->title, $tzreport->title, "Expected equal titles");
    $this->assertEquals(1284530400, $tzreport->begintime, 'Expected 1284530400 as begintime, got ' . $tzreport->begintime);
    $this->assertEquals(1284566400, $tzreport->endtime, 'Expected 1284566400 as endtime, got ' . $tzreport->endtime);
    $this->assertEquals($this->report->break_duration_minutes*60, $tzreport->breakduration, "Expected break durations to match");
  }

  function testNoIntellitimeIdInReport() {
    try {
      $tzReport = createMockReport(NULL, "2010-10-10", "10:10", "22:10");
      unset($tzReport->intellitime_id);

      new TZIntellitimeReport($tzReport);
      $this->fail("No id, no report!");
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

  function testConstructorHandlesDELETED() {
    $tzreport = createMockReport(NULL, "2010-10-10", "10:10", "22:10");
    $tzreport->flags = TZFlags::DELETED;
    $itreport = new TZIntellitimeReport($tzreport);
    $this->assertEquals(TZIntellitimeReport::STATE_DELETED, $itreport->state);
  }
}

function _tzintellitime_populate_report(&$report) {
    $report->id = 'mhbLP96iqH2Zr7CDtWyWb84hbku5Eii3';
    $report->done = FALSE;
    $report->year = 2010;
    $report->month = 9;
    $report->day = 15;
    $report->title = "En dag i livet för en apa.";
    $report->begin = "08:00";
    $report->end = "18:00";
    $report->break_duration_minutes = 24;
    $report->overtime_hours = 4;
    $report->comment = 'TimeZynk';
}