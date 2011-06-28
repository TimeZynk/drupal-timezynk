<?php

class TZIntellitimeParserTest extends PHPUnit_Framework_TestCase {

  private function loadHTMLFile($filename) {
    $full_name = dirname(__FILE__) . "/../tests/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    return new TZIntellitimeParser($contents);
  }

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

  public function testBuildLoginPost() {
    $login_view_state = "dDwyNDA3MjczMzc7dDw7bDxpPDA+Oz47bDx0PDtsPGk8MT47aTw1PjtpPDE1PjtpPDE5Pjs+O2w8dDw7bDxpPDA+Oz47bDx0PDtsPGk8MT47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9DdXN0b21lcnMvaW50ZWxsaXBsYW5fbG9nby5naWY7Pj47Pjs7Pjt0PHA8cDxsPEltYWdlVXJsOz47bDx+L0ltYWdlcy9JbWdfSW50ZWxsaXBsYW5Mb2dvV2hpdGUuZ2lmOz4+Oz47Oz47Pj47Pj47dDxwPHA8bDxWaXNpYmxlOz47bDxvPGY+Oz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxMb2dnYSBpbjs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDs+O2w8R2zDtm10IGzDtnNlbm9yZGV0Pzs+Pjs+Ozs+Oz4+Oz4+Oz5ngNWIe5WIW3O3prUuG7wbptC3jg==";
    $parser = $this->loadHTMLFile("intellitime-login-page.html");
    $post = $parser->build_login_post("monkeyname", "monkeypass");
    $this->assertEquals('Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d', $post['action'], "correct action parsed");
    $this->assertEquals($login_view_state, $post['data']['__VIEWSTATE'], "Found viewstate");
    $this->assertEquals("monkeyname", $post['data']['TextBoxUserName'], "username correct");
    $this->assertEquals("monkeypass", $post['data']['TextBoxPassword'], "password correct");
    $this->assertEquals("Logga in", $post['data']['ButtonLogin'], "parsed submit button");
  }

  public function testLoginCheckerCatchesFailedLogin() {
    $parser = $this->loadHTMLFile('intellitime-login-page.html');
    $this->assertFalse($parser->is_valid_login(), "detected failed login");
  }

  public function testLoginCheckerCatchesSuccessfulLogin() {
    $parser = $this->loadHTMLFile('intellitime-main-page.html');
    $this->assertTrue($parser->is_valid_login(), "detected successful login");
  }

  public function testBuildInputWithComments() {
    $parser = $this->loadHTMLFile('intellitime-timereport-with-comments.html');
    $reports = $parser->parse_reports();
    $itreport = $reports[0];

    $itreport->id = 'mhbLP96iqH3xps0bh7%2fZv84hbku5Eii3';
    $itreport->begin = '09:31';
    $itreport->end = '17:43';
    $itreport->comment = 'I did all I could!';

    $result = $parser->build_update_reports_post(array($itreport));
    $this->assertNotNull($result, "expect not NULL");
    $this->assertEquals($itreport->comment, $result['OldRowsRepeater:_ctl0:TextboxNote']);
    $this->assertEquals($itreport->begin, $result['OldRowsRepeater:_ctl0:TextboxTimeFrom']);
    $this->assertEquals($itreport->end, $result['OldRowsRepeater:_ctl0:TextboxTimeTo']);
    $this->assertEquals($itreport->break_duration_minutes, $result['OldRowsRepeater:_ctl0:TextboxBreak']);
  }

  public function testBuildInputWithoutComments() {
    $parser = $this->loadHTMLFile('intellitime-timereport-page.html');
    $reports = $parser->parse_reports();
    $itreport = $reports[4];

    $itreport->id = 'mhbLP96iqH2aeWvSjjSDX84hbku5Eii3';
    $itreport->begin = '09:31';
    $itreport->end = '17:43';
    $itreport->comment = 'I did all I could!';

    $result = $parser->build_update_reports_post(array($itreport));
    $this->assertNotNull($result, "expect not NULL");
    $this->assertFalse(isset($result['OldRowsRepeater:_ctl4:TextboxNote']));
    $this->assertEquals($itreport->begin, $result['OldRowsRepeater:_ctl4:TextboxTimeFrom']);
    $this->assertEquals($itreport->end, $result['OldRowsRepeater:_ctl4:TextboxTimeTo']);
    $this->assertEquals($itreport->break_duration_minutes, $result['OldRowsRepeater:_ctl4:TextboxBreak']);
  }

  public function testParseAssignments() {
    $parser = $this->loadHTMLFile('intellitime-timereport-page.html');
    $assignments = $parser->parse_assignments();
    $this->assertEquals(29, count($assignments), "expects 29 assignments");
    $count_assignments = 0;
    $count_absence = 0;
    foreach ($assignments as $assignment) {
      if ($assignment->type == TZIntellitimeAssignment::TYPE_ASSIGNMENT) {
        $count_assignments += 1;
        $this->assertEquals("5983", $assignment->id, "assignment code 5983");
      } else {
        $count_absence += 1;
        $this->assertEquals("_AC_", substr($assignment->id, 0, 4));
      }
    }
    $this->assertEquals(1, $count_assignments);
    $this->assertEquals(28, $count_absence);
  }

  public function testParseAssignmentsUTF8Encoding() {
    $parser = $this->loadHTMLFile('intellitime-timereport-page.html');
    $assignments = $parser->parse_assignments();
    $this->assertEquals("Testföretaget Effekt, Lagerarbetare", $assignments[0]->title, "expect UTF-8 encoding");
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

  public function testBuildSimplePost() {
    $parser = $this->loadHTMLFile('intellitime-v8-timereport-w1102-not-done.txt');
    $reports = $parser->parse_reports();
    $post = $parser->build_update_reports_post($reports, FALSE);
    $this->assertNotNull($post);
    $this->assertEquals('Uppdatera', $post['UpdateButton']);
  }

  public function testThrowsWhenPostingUnknownReport() {
    $parser = $this->loadHTMLFile('intellitime-v8-timereport-w1102-not-done.txt');
    $reports = $parser->parse_reports();
    $reports[0]->id = 'NONEXISTANT';
    try {
      $post = $parser->build_update_reports_post($reports, FALSE);
      $this->fail('Should have thrown exception');
    } catch(TZIntellitimeReportRowNotFound $e) {
      // PASS
      $this->assertNotNull($e);
    }
  }

  public function testV9HandleMultipleYearsInSameWeek() {
    $parser = $this->loadHTMLFile('Parser_v9HandleMultipleYearsInSameWeek.txt');
    $reports = $parser->parse_reports();
    $this->assertEquals(2, count($reports));
    $this->assertEquals("2010-12-31", $reports[0]->get_date_string());
    $this->assertEquals("2011-01-01", $reports[1]->get_date_string());
  }

  public function testCrapForBreakfast() {
    try {
      $parser = new TZIntellitimeParser("");
      $this->fail("Expected exception when feeding crap to parser.");
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

  public function testNoActionPresent() {
    $parser = new TZIntellitimeParser("<html><head/><body>apa</body></hmtl>");
    $this->assertNull($parser->parse_form_action());
  }

  public function testParseErrorString() {
    $parser = $this->loadHTMLFile('WeekData_ThrowOnErrorPage.txt');
    $error = $parser->parse_page_error();
    $this->assertEquals('Unexpected error', $error);
  }

  public function testParseUserNameFromMainPage() {
    $parser = $this->loadHTMLFile('intellitime-main-page.html');
    $username = $parser->parse_username();
    $this->assertEquals('Johan Heander', $username);
  }

  public function testParseUserNameFromTimereportPage() {
    $parser = $this->loadHTMLFile('intellitime-v9-timereport-three-open.txt');
    $username = $parser->parse_username();
    $this->assertEquals('Johan Heander', $username);
  }

  public function testFutureV8WeekMarkedOpen() {
    $parser = $this->loadHTMLFile('Parser_v8FutureWeekOpen.txt');
    $reports = $parser->parse_reports();
    foreach ($reports as $report) {
      $this->assertEquals(TZIntellitimeReport::STATE_OPEN, $report->state);
    }
  }
}
