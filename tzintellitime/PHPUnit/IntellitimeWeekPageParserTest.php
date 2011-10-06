<?php

class IntellitimeWeekPageParserTest extends PHPUnit_Framework_TestCase {
  public function testParseStateImmutable() {
    $parser = $this->loadHTMLFile('intellitime-v8-timereport-two-done-one-open.txt');
    $reports = $parser->parse_reports();
    $this->assertEquals(3, count($reports));
    foreach($reports as $report) {
      $this->assertTrue($report->stateImmutable);
    }
  }

  public function testParseStateImmutableMixed() {
    $parser = $this->loadHTMLFile('intellitime-v9-timereport-administrator-absence-not-done.txt');
    $reports = $parser->parse_reports();
    $this->assertEquals(4, count($reports));
    foreach($reports as $report) {
      if($report->id == 'mhbLP96iqH0993oUoX8OKM4hbku5Eii3') {
        $this->assertTrue($report->stateImmutable);
      } else {
        $this->assertFalse($report->stateImmutable);
      }
    }
  }

  public function testParseReportsCorrectCount() {
    $parser = $this->loadHTMLFile('intellitime-timereport-page.html');
    $reports = $parser->parse_reports();
    $this->assertEquals(7, count($reports), "expect seven reports");
  }

  public function testParseReportsOnlyUniqueIDs() {
    $parser = $this->loadHTMLFile('intellitime-timereport-page.html');
    $reports = $parser->parse_reports();
    foreach($reports as $report) {
      $id[$report->id] = TRUE;
    }
    $this->assertEquals(count($reports), count($id), "all IDs unique");
  }

  public function testParseReportsUTF8Encoding() {
    $parser = $this->loadHTMLFile('intellitime-timereport-page.html');
    $reports = $parser->parse_reports();
    $this->assertEquals("Testföretaget Effekt, Lagerarbetare", $reports[3]->title, "expect UTF-8 encoding");
  }

  public function testReadComments() {
    $parser = $this->loadHTMLFile('intellitime-timereport-with-comments.html');
    $reports = $parser->parse_reports();
    $this->assertEquals('asda', $reports[0]->comment);
    $this->assertEquals('', $reports[1]->comment);
    $this->assertEquals('Panda panda panda "test mjauu" hej!*\'112\'', $reports[2]->comment);
  }

  public function testParseTimeFields() {
    $parser = $this->loadHTMLFile('intellitime-timereport-with-comments.html');
    $reports = $parser->parse_reports();
    $this->assertEquals('08:50', $reports[0]->begin);
    $this->assertEquals('17:00', $reports[0]->end);
    $this->assertEquals(60, $reports[0]->break_duration_minutes);
    $this->assertEquals('12:00', $reports[1]->begin);
    $this->assertEquals('13:00', $reports[1]->end);
    $this->assertEquals(10, $reports[1]->break_duration_minutes);
    $this->assertEquals('10:00', $reports[2]->begin);
    $this->assertEquals('23:59', $reports[2]->end);
    $this->assertEquals(120, $reports[2]->break_duration_minutes);
  }

  /**
   * Absence reports miss a break duration field in intellitime.
   * We have to infer it from total time and start and end.
   */
  public function testInferBreakDurationForAbsence() {
    $parser = $this->loadHTMLFile('intellitime-timereport-multiple-year-hrefs.html');
    $reports = $parser->parse_reports();
    $this->assertEquals('13:00', $reports[3]->begin);
    $this->assertEquals('19:30', $reports[3]->end);
    $this->assertEquals(30, $reports[3]->break_duration_minutes);
    $this->assertEquals(0, $reports[3]->overtime_hours);
    $this->assertEquals('08:00', $reports[4]->begin);
    $this->assertEquals('17:30', $reports[4]->end);
    $this->assertEquals(30, $reports[4]->break_duration_minutes);
    $this->assertEquals(0, $reports[4]->overtime_hours);
    $this->assertEquals('22:00', $reports[5]->begin);
    $this->assertEquals('08:00', $reports[5]->end);
    $this->assertEquals(30, $reports[5]->break_duration_minutes);
    $this->assertEquals(0, $reports[5]->overtime_hours);
  }

  /**
   * Due to the page containing links to 'unfinished weeks' in both 2010 and 2011,
   * the old code mistakenly fell in to a mode where it thought new year's eve was in november.
   * This is a regression test to avoid that particular stupidity again. :)
   */
  public function testParseYears() {
    $parser = $this->loadHTMLFile('intellitime-timereport-multiple-year-hrefs.html');
    $reports = $parser->parse_reports();
    foreach ($reports as $report) {
      $this->assertEquals(2010, $report->year);
    }
  }

  public function testAbsenceReportNotMarkedDone() {
    $parser = $this->loadHTMLFile('intellitime-v9-timereport-administrator-absence-not-done.txt');
    $reports = $parser->parse_reports();
    $this->assertEquals(TZIntellitimeReport::STATE_OPEN, $reports[1]->state);
  }

  public function testAbsenceReportAllMarkedDone() {
    $parser = $this->loadHTMLFile('intellitime-v9-timereport-administrator-absence-all-marked-done.txt');
    $reports = $parser->parse_reports();
    foreach($reports as $report) {
      $this->assertEquals(TZIntellitimeReport::STATE_REPORTED, $report->state);
    }
  }

  public function testReportsMarkedDoneV8() {
    $parser = $this->loadHTMLFile('intellitime-v8-timereport-w1102-all-done.txt');
    $reports = $parser->parse_reports();
    foreach($reports as $report) {
      $this->assertEquals(TZIntellitimeReport::STATE_REPORTED, $report->state);
    }
  }

  public function testReportsNotMarkedDoneV8() {
    $parser = $this->loadHTMLFile('intellitime-v8-timereport-w1102-not-done.txt');
    $reports = $parser->parse_reports();
    foreach($reports as $report) {
      $this->assertEquals(TZIntellitimeReport::STATE_OPEN, $report->state);
    }
  }

  public function testV9HandleMultipleYearsInSameWeek() {
    $parser = $this->loadHTMLFile('Parser_v9HandleMultipleYearsInSameWeek.txt');
    $reports = $parser->parse_reports();
    $this->assertEquals(2, count($reports));
    $this->assertEquals("2010-12-31", $reports[0]->get_date_string());
    $this->assertEquals("2011-01-01", $reports[1]->get_date_string());
  }

  public function testFutureV8WeekMarkedOpen() {
    $parser = $this->loadHTMLFile('Parser_v8FutureWeekOpen.txt');
    $reports = $parser->parse_reports();
    foreach ($reports as $report) {
      $this->assertEquals(TZIntellitimeReport::STATE_OPEN, $report->state);
    }
  }

  private function loadHTMLFile($filename) {
    $full_name = dirname(__FILE__) . "/../tests/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    $parser = new IntellitimeWeekPageParserTestPage($contents);
    return new IntellitimeWeekPageParser($parser->getDoc());
  }
}

class IntellitimeWeekPageParserTestPage extends IntellitimePage {
  function getDoc() {
    return $this->doc;
  }
}