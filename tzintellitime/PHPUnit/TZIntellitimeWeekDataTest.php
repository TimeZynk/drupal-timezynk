<?php

class TZIntellitimeWeekDataTest extends PHPUnit_Framework_TestCase {
  function setUp() {
    $this->account = (object)array(
      'name' => 'Kalle',
      'uid' => 63,
    );
    $this->server = $this->getMock('IntellitimeServer');
  }

  function testEmptyWeek() {
    $reports = array();

    $weekData = $this->loadHTMLFile('WeekData_v9SyncEmptyWeek.txt');
    $this->assertInstanceOf('TZIntellitimeWeekData', $weekData);

    $updatedReports = $weekData->updateReports($reports, $this->account);
    $this->assertEquals($reports, $updatedReports);

    $post = $weekData->buildPost($reports);
    $this->assertNull($post);

    $unfinishedWeeks = $weekData->getUnfinishedWeeks();
    $this->assertEquals(4, count($unfinishedWeeks));

    $assignments = $weekData->getAssignments();
    // All assignments are returned, even if they are not used
    $this->assertEquals(1, count($assignments));
  }

  function testNoParserThrows() {
    try {
      $weekData = new TZIntellitimeWeekData(NULL);
      $this->fail("Should not be able to pass NULL parsers..");
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

  function testV9CloseWeekWithWeekDoneForSingleReport() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-single-open.txt');
    $this->assertInstanceOf('TZIntellitimeWeekData', $weekData);

    $reports = array();
    $reports[] = createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30');

    $post = $weekData->buildPost($reports);
    $this->assertInstanceOf('IntellitimeWeekUpdatePost', $post);
    $this->assertTrue($post->getAllReportsDone());
  }

  function testV8CloseWeekWithWeekDoneForSingleReport() {
    $weekData = $this->loadHTMLFile('intellitime-v8-timereport-single-open.txt');
    $this->assertInstanceOf('TZIntellitimeWeekData', $weekData);

    $reports = array();
    $reports[] = createMockReport('mhbLP96iqH2OzMt6qJkssA%3d%3d', '2011-01-25', '08:30', '17:30');

    $post = $weekData->buildPost($reports);
    $this->assertInstanceOf('IntellitimeWeekUpdatePost', $post);
    $this->assertTrue($post->getAllReportsDone());
  }

  function testV9ChangingSingleDoneReportResultsInChangeWeekPost() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-single-done.txt');
    $this->assertInstanceOf('TZIntellitimeWeekData', $weekData);

    $reports = array();
    $reports[] = createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30');

    $post = $weekData->buildPost($reports);
    $this->assertInstanceOf('IntellitimeWeekUnlockPost', $post);
    $this->assertFalse($post->getUnlockImmutable());
  }

  function testV8ChangingSingleDoneReportResultsInChangeWeekPost() {
    $weekData = $this->loadHTMLFile('intellitime-v8-timereport-single-done.txt');
    $this->assertInstanceOf('TZIntellitimeWeekData', $weekData);

    $reports = array();
    $reports[] = createMockReport('mhbLP96iqH2OzMt6qJkssA%3d%3d', '2011-01-25', '08:30', '17:30');

    $post = $weekData->buildPost($reports);
    $this->assertInstanceOf('IntellitimeWeekUnlockPost', $post);
    $this->assertTrue($post->getUnlockImmutable());
  }

  function testV9NotFoundReportIsMarkedAsDeleted() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-single-open.txt');

    $reports = array(
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30'),
      createMockReport('mhbLP96iqHDOESNOTEXISTii3', '2011-01-26', '08:30', '17:30'),
    );

    $updatedReports = $weekData->updateReports($reports, $this->account);
    $this->assertEquals(2, count($updatedReports));
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[0]->flags);
    $this->assertEquals(TZFlags::DELETED, $updatedReports[1]->flags);
  }

  function testV8LockToUnlock() {
    /* In v8 you may have to first lock the whole week and then unlock it to be able
     * to edit a report that has been set to reported before. This can occur if
     * a week is first declared as "Done" and then another open report is added.
     */
    $weekData = $this->loadHTMLFile('intellitime-v8-timereport-two-done-one-open.txt');

    $reports = array(
      createMockReport('mhbLP96iqH0fyoh9MNS5ww%3d%3d', '2011-01-10', '08:05', '15:10', 15, 0),
      createMockReport('mhbLP96iqH2UC850iSNtFg%3d%3d', '2011-01-12', '06:55', '17:20', 35, 1),
      createMockReport('mhbLP96iqH25nFbnfXHiaw%3d%3d', '2011-01-13', '08:00', '16:30', 30, 0, TZFlags::CREATED),
    );

    $updatedReports = $weekData->updateReports($reports, $this->account);
    $this->assertEquals(3, count($updatedReports));
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[0]->flags);
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[1]->flags);
    $this->assertEquals(TZFlags::CREATED, $updatedReports[2]->flags);

    // First POST, expect a post that reports the week as Done.
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMTg1NjE4NzIxO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9DdXN0b21lcnMvaW50ZWxsaXBsYW5fbG9nby5naWY7Pj47Pjs7Pjt0PHA8cDxsPFRleHQ7PjtsPEpvbmFzICBTdW5kaW47Pj47Pjs7Pjt0PHA8cDxsPEltYWdlVXJsOz47bDx+L0ltYWdlcy9JbWdfSW50ZWxsaXBsYW5Mb2dvV2hpdGUuZ2lmOz4+Oz47Oz47Pj47Pj47dDxwPHA8bDxUZXh0Oz47bDwyOz4+Oz47Oz47dDx0PDtwPGw8aTwwPjtpPDE+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFdhbHRlciAmIENPICwgS29jaywgU3BlY2lhbHVwcGRyYWcgZWY7MjAyPjs+PjtsPGk8MD47Pj47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8Mz47Pj47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8Mz47Pj47bDxpPDA+O2k8MT47aTwyPjs+O2w8dDw7bDxpPDM3Pjs+O2w8dDw7bDxpPDA+O2k8MT47aTwyPjs+O2w8dDxAPGRpc2FibGVkO0V4cGVuc2UuYXNweD9taGJMUDk2aXFIMGZ5b2g5TU5TNXd3JTNkJTNkO0V4cGVuc2UuYXNweD9taGJMUDk2aXFIMGZ5b2g5TU5TNXd3JTNkJTNkOz47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8MD47Pj47Oz47dDxAPFw8b3B0aW9uXD5bS2xpY2thIGjDpHJdXDwvb3B0aW9uXD47Pjs7Pjs+Pjs+Pjt0PDtsPGk8Mzc+Oz47bDx0PDtsPGk8MD47aTwxPjtpPDI+Oz47bDx0PEA8ZGlzYWJsZWQ7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgyVUM4NTBpU050RmclM2QlM2Q7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgyVUM4NTBpU050RmclM2QlM2Q7Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwwPjs+Pjs7Pjt0PEA8XDxvcHRpb25cPltLbGlja2EgaMOkcl1cPC9vcHRpb25cPjs+Ozs+Oz4+Oz4+O3Q8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxcZTtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDI1bkZibmZYSGlhdyUzZCUzZDtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDI1bkZibmZYSGlhdyUzZCUzZDs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcPG9wdGlvblw+W0tsaWNrYSBow6RyXVw8L29wdGlvblw+Oz47Oz47Pj47Pj47Pj47dDw7bDxpPDEzPjtpPDE1Pjs+O2w8dDx0PDtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0PjtpPDU+O2k8Nj47aTw3Pjs+O2w8cDxtw6UsIDEwLzAxIDsyMDExLTAxLTEwPjtwPHRpLCAxMS8wMSA7MjAxMS0wMS0xMT47cDxvbiwgMTIvMDEgOzIwMTEtMDEtMTI+O3A8dG8sIDEzLzAxIDsyMDExLTAxLTEzPjtwPGZyLCAxNC8wMSA7MjAxMS0wMS0xND47cDxsw7YsIDE1LzAxIDsyMDExLTAxLTE1PjtwPHPDtiwgMTYvMDEgOzIwMTEtMDEtMTY+Oz4+Oz47Oz47dDx0PDtwPGw8aTwxPjtpPDI+O2k8Mz47PjtsPHA8V2FsdGVyICYgQ08gLCBLb2NrLCBTcGVjaWFsdXBwZHJhZyBlZjsyMDI+O3A8LS0tOy0xPjtwPEtvbnN1bHRlbnMgbGVkaWdhIGRhZyBlbmwuIMO2dmVyZW5za29tbWV0IHNjaGVtYS47X0FDX0xFRElHIEVOTC4gU0NIRU1BPjs+Pjs+Ozs+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8VXBwZGF0ZXJhOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxWZWNrYSBLbGFyOz4+Oz47Oz47dDxwPHA8bDxUZXh0O1Zpc2libGU7PjtsPMOEbmRyYSB2ZWNrYTtvPGY+Oz4+Oz47Oz47Pj47Pj47bDxPbGRSb3dzUmVwZWF0ZXI6X2N0bDI6Q2hlY2tCb3hEZWxldGU7RnVsbERheUNoZWNrQm94Oz4+')
        . '&DoPost=true&CustOrdersDropDown=0&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A05&OldRowsRepeater%3A_ctl0%3ADateToHidden=15%3A10&OldRowsRepeater%3A_ctl1%3ADateFromHidden=07%3A55&OldRowsRepeater%3A_ctl1%3ADateToHidden=16%3A20&OldRowsRepeater%3A_ctl2%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl2%3ADateToHidden=16%3A30&OldRowsRepeater%3A_ctl2%3ATextboxBreak=30&OldRowsRepeater%3A_ctl2%3ATextboxNote=&OldRowsRepeater%3A_ctl2%3ATextboxTimeFrom=08%3A00&OldRowsRepeater%3A_ctl2%3ATextboxTimeTo=16%3A30&AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&DoneButton=Vecka+Klar';
    $expectedPost = toPostHash($postString);

    $this->server->expects($this->at(0))
                 ->method('post')
                 ->with($this->anything(), $expectedPost)
                 ->will($this->returnValue($this->readFile('intellitime-v8-timereport-three-done.txt')));
    $post = $weekData->buildPost($updatedReports);
    $weekData = new TZIntellitimeWeekData($post->post());

    // Second POST, expect a unlock for the entire week
    $updatedReports = $weekData->postProcessPost($post, $updatedReports, $this->account);

    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMTg1NjE4NzIxO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9DdXN0b21lcnMvaW50ZWxsaXBsYW5fbG9nby5naWY7Pj47Pjs7Pjt0PHA8cDxsPFRleHQ7PjtsPEpvbmFzICBTdW5kaW47Pj47Pjs7Pjt0PHA8cDxsPEltYWdlVXJsOz47bDx+L0ltYWdlcy9JbWdfSW50ZWxsaXBsYW5Mb2dvV2hpdGUuZ2lmOz4+Oz47Oz47Pj47Pj47dDxwPHA8bDxUZXh0Oz47bDwyOz4+Oz47Oz47dDx0PDtwPGw8aTwwPjtpPDE+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFdhbHRlciAmIENPICwgS29jaywgU3BlY2lhbHVwcGRyYWcgZWY7MjAyPjs+PjtsPGk8MD47Pj47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8Mz47Pj47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8Mz47Pj47bDxpPDA+O2k8MT47aTwyPjs+O2w8dDw7bDxpPDM3Pjs+O2w8dDw7bDxpPDA+O2k8MT47aTwyPjs+O2w8dDxAPGRpc2FibGVkO0V4cGVuc2UuYXNweD9taGJMUDk2aXFIMGZ5b2g5TU5TNXd3JTNkJTNkO0V4cGVuc2UuYXNweD9taGJMUDk2aXFIMGZ5b2g5TU5TNXd3JTNkJTNkOz47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8MD47Pj47Oz47dDxAPFw8b3B0aW9uXD5bS2xpY2thIGjDpHJdXDwvb3B0aW9uXD47Pjs7Pjs+Pjs+Pjt0PDtsPGk8Mzc+Oz47bDx0PDtsPGk8MD47aTwxPjtpPDI+Oz47bDx0PEA8ZGlzYWJsZWQ7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgyVUM4NTBpU050RmclM2QlM2Q7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgyVUM4NTBpU050RmclM2QlM2Q7Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwwPjs+Pjs7Pjt0PEA8XDxvcHRpb25cPltLbGlja2EgaMOkcl1cPC9vcHRpb25cPjs+Ozs+Oz4+Oz4+O3Q8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxcZTtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDI1bkZibmZYSGlhdyUzZCUzZDtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDI1bkZibmZYSGlhdyUzZCUzZDs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcPG9wdGlvblw+W0tsaWNrYSBow6RyXVw8L29wdGlvblw+Oz47Oz47Pj47Pj47Pj47dDw7bDxpPDEzPjtpPDE1Pjs+O2w8dDx0PDtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0PjtpPDU+O2k8Nj47aTw3Pjs+O2w8cDxtw6UsIDEwLzAxIDsyMDExLTAxLTEwPjtwPHRpLCAxMS8wMSA7MjAxMS0wMS0xMT47cDxvbiwgMTIvMDEgOzIwMTEtMDEtMTI+O3A8dG8sIDEzLzAxIDsyMDExLTAxLTEzPjtwPGZyLCAxNC8wMSA7MjAxMS0wMS0xND47cDxsw7YsIDE1LzAxIDsyMDExLTAxLTE1PjtwPHPDtiwgMTYvMDEgOzIwMTEtMDEtMTY+Oz4+Oz47Oz47dDx0PDtwPGw8aTwxPjtpPDI+O2k8Mz47PjtsPHA8V2FsdGVyICYgQ08gLCBLb2NrLCBTcGVjaWFsdXBwZHJhZyBlZjsyMDI+O3A8LS0tOy0xPjtwPEtvbnN1bHRlbnMgbGVkaWdhIGRhZyBlbmwuIMO2dmVyZW5za29tbWV0IHNjaGVtYS47X0FDX0xFRElHIEVOTC4gU0NIRU1BPjs+Pjs+Ozs+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8VXBwZGF0ZXJhOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxWZWNrYSBLbGFyOz4+Oz47Oz47dDxwPHA8bDxUZXh0O1Zpc2libGU7PjtsPMOEbmRyYSB2ZWNrYTtvPGY+Oz4+Oz47Oz47Pj47Pj47bDxPbGRSb3dzUmVwZWF0ZXI6X2N0bDI6Q2hlY2tCb3hEZWxldGU7RnVsbERheUNoZWNrQm94Oz4+')
        . '&DoPost=true&CustOrdersDropDown=0&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A05&OldRowsRepeater%3A_ctl0%3ADateToHidden=15%3A10&OldRowsRepeater%3A_ctl1%3ADateFromHidden=07%3A55&OldRowsRepeater%3A_ctl1%3ADateToHidden=16%3A20&OldRowsRepeater%3A_ctl2%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl2%3ADateToHidden=16%3A30&AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&ChangeButton=%C3%84ndra+vecka';
    $expectedPost = toPostHash($postString);
    $this->server->expects($this->at(0))
                 ->method('post')
                 ->with($this->anything(), $expectedPost)
                 ->will($this->returnValue($this->readFile('intellitime-v8-timereport-three-open.txt')));
    $post = $weekData->buildPost($updatedReports);
    $weekData = new TZIntellitimeWeekData($post->post());

    // Third POST, update times and let the reports stay Open
    $updatedReports = $weekData->postProcessPost($post, $updatedReports, $this->account);

    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMTg1NjE4NzIxO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9DdXN0b21lcnMvaW50ZWxsaXBsYW5fbG9nby5naWY7Pj47Pjs7Pjt0PHA8cDxsPFRleHQ7PjtsPEpvbmFzICBTdW5kaW47Pj47Pjs7Pjt0PHA8cDxsPEltYWdlVXJsOz47bDx+L0ltYWdlcy9JbWdfSW50ZWxsaXBsYW5Mb2dvV2hpdGUuZ2lmOz4+Oz47Oz47Pj47Pj47dDxwPHA8bDxUZXh0Oz47bDwyOz4+Oz47Oz47dDx0PDtwPGw8aTwwPjtpPDE+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFdhbHRlciAmIENPICwgS29jaywgU3BlY2lhbHVwcGRyYWcgZWY7MjAyPjs+PjtsPGk8MD47Pj47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8Mz47Pj47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8Mz47Pj47bDxpPDA+O2k8MT47aTwyPjs+O2w8dDw7bDxpPDM3Pjs+O2w8dDw7bDxpPDA+O2k8MT47aTwyPjs+O2w8dDxAPGRpc2FibGVkO0V4cGVuc2UuYXNweD9taGJMUDk2aXFIMGZ5b2g5TU5TNXd3JTNkJTNkO0V4cGVuc2UuYXNweD9taGJMUDk2aXFIMGZ5b2g5TU5TNXd3JTNkJTNkOz47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8MD47Pj47Oz47dDxAPFw8b3B0aW9uXD5bS2xpY2thIGjDpHJdXDwvb3B0aW9uXD47Pjs7Pjs+Pjs+Pjt0PDtsPGk8Mzc+Oz47bDx0PDtsPGk8MD47aTwxPjtpPDI+Oz47bDx0PEA8ZGlzYWJsZWQ7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgyVUM4NTBpU050RmclM2QlM2Q7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgyVUM4NTBpU050RmclM2QlM2Q7Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwwPjs+Pjs7Pjt0PEA8XDxvcHRpb25cPltLbGlja2EgaMOkcl1cPC9vcHRpb25cPjs+Ozs+Oz4+Oz4+O3Q8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxcZTtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDI1bkZibmZYSGlhdyUzZCUzZDtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDI1bkZibmZYSGlhdyUzZCUzZDs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcPG9wdGlvblw+W0tsaWNrYSBow6RyXVw8L29wdGlvblw+Oz47Oz47Pj47Pj47Pj47dDw7bDxpPDEzPjtpPDE1Pjs+O2w8dDx0PDtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0PjtpPDU+O2k8Nj47aTw3Pjs+O2w8cDxtw6UsIDEwLzAxIDsyMDExLTAxLTEwPjtwPHRpLCAxMS8wMSA7MjAxMS0wMS0xMT47cDxvbiwgMTIvMDEgOzIwMTEtMDEtMTI+O3A8dG8sIDEzLzAxIDsyMDExLTAxLTEzPjtwPGZyLCAxNC8wMSA7MjAxMS0wMS0xND47cDxsw7YsIDE1LzAxIDsyMDExLTAxLTE1PjtwPHPDtiwgMTYvMDEgOzIwMTEtMDEtMTY+Oz4+Oz47Oz47dDx0PDtwPGw8aTwxPjtpPDI+O2k8Mz47PjtsPHA8V2FsdGVyICYgQ08gLCBLb2NrLCBTcGVjaWFsdXBwZHJhZyBlZjsyMDI+O3A8LS0tOy0xPjtwPEtvbnN1bHRlbnMgbGVkaWdhIGRhZyBlbmwuIMO2dmVyZW5za29tbWV0IHNjaGVtYS47X0FDX0xFRElHIEVOTC4gU0NIRU1BPjs+Pjs+Ozs+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8VXBwZGF0ZXJhOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxWZWNrYSBLbGFyOz4+Oz47Oz47dDxwPHA8bDxUZXh0O1Zpc2libGU7PjtsPMOEbmRyYSB2ZWNrYTtvPGY+Oz4+Oz47Oz47Pj47Pj47bDxPbGRSb3dzUmVwZWF0ZXI6X2N0bDI6Q2hlY2tCb3hEZWxldGU7RnVsbERheUNoZWNrQm94Oz4+')
        . '&DoPost=true&CustOrdersDropDown=0&OldRowsRepeater%3A_ctl0%3ATextboxBreak=15&OldRowsRepeater%3A_ctl0%3ATextboxNote=&OldRowsRepeater%3A_ctl0%3ATextboxTimeFrom=08%3A05&OldRowsRepeater%3A_ctl0%3ATextboxTimeTo=15%3A10&OldRowsRepeater%3A_ctl1%3ATextboxBreak=35&OldRowsRepeater%3A_ctl1%3ATextboxNote=&OldRowsRepeater%3A_ctl1%3ATextboxTimeFrom=06%3A55&OldRowsRepeater%3A_ctl1%3ATextboxTimeTo=17%3A20&OldRowsRepeater%3A_ctl2%3ATextboxBreak=30&OldRowsRepeater%3A_ctl2%3ATextboxNote=&OldRowsRepeater%3A_ctl2%3ATextboxTimeFrom=08%3A00&OldRowsRepeater%3A_ctl2%3ATextboxTimeTo=16%3A30&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A05&OldRowsRepeater%3A_ctl0%3ADateToHidden=15%3A10&OldRowsRepeater%3A_ctl1%3ADateFromHidden=07%3A55&OldRowsRepeater%3A_ctl1%3ADateToHidden=16%3A20&OldRowsRepeater%3A_ctl2%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl2%3ADateToHidden=16%3A30&AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&UpdateButton=Uppdatera';
    $expectedPost = toPostHash($postString);
    $this->server->expects($this->at(0))
                 ->method('post')
                 ->with($this->anything(), $expectedPost)
                 ->will($this->returnValue($this->readFile('intellitime-v8-timereport-three-open-updated.txt')));
    $post = $weekData->buildPost($updatedReports);
    $weekData = new TZIntellitimeWeekData($post->post());

    // Fourth time around, no POST this time
    $updatedReports = $weekData->postProcessPost($post, $updatedReports, $this->account);
    $post = $weekData->buildPost($updatedReports);
    $this->assertNull($post);

    // Run final update and check report states
    $updatedReports = $weekData->updateReports($updatedReports, $this->account, TRUE);
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[0]->flags);
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[1]->flags);
    $this->assertReportTimes('2011-01-12', '06:55', '17:20', 35, $updatedReports[1]);
    $this->assertEquals(TZFlags::CREATED, $updatedReports[2]->flags);
  }

  function testSimultaneousNewAndUpdated() {
    /* Changes an existing report and adds a new report at the same time.
     * Will need multiple posts to ensure update, first unlock, then update
     * the changed report, then add the new report and finally update the new
     * report.
     */
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-single-done.txt');
    $expectedAction = 'TimeReport/TimeReport.aspx?DateInWeek=2011-01-28';

    $reports = array(
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:05', '15:10', 15, 1),
      createMockReport(NULL, '2011-01-26', '08:00', '16:30', 30, 1),
    );
    $reports[1]->intellitime_jobid = 6200;

    $updatedReports = $weekData->updateReports($reports, $this->account);
    $this->assertEquals(2, count($updatedReports));
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[0]->flags);
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[1]->flags);

    // First POST, expect a unlock
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8aHR0cDovL2lwd2ViLmludGVsbGlwbGFuLnNlL2t1bmRsb2dvLzQwOTQuanBnOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxKb2hhbiBIZWFuZGVyOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8NDs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjtpPDI+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+O3A8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBMYWdlcmFyYmV0YXJlOzU5ODM+Oz4+O2w8aTwwPjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTw0Pjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwxPjs+PjtsPGk8MD47PjtsPHQ8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxkaXNhYmxlZDtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMztFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMzs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcZTs+Ozs+Oz4+Oz4+Oz4+O3Q8O2w8aTwxMz47aTwxNT47aTwxNz47aTwxOT47aTwyMT47aTwyMz47aTwyNz47aTwyOT47aTwzMz47PjtsPHQ8dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+O3A8bDxpPDE+O2k8Mj47aTwzPjtpPDQ+O2k8NT47aTw2PjtpPDc+Oz47bDxwPG3DpSwgMjQvMDEgOzIwMTEtMDEtMjQ+O3A8dGksIDI1LzAxIDsyMDExLTAxLTI1PjtwPG9uLCAyNi8wMSA7MjAxMS0wMS0yNj47cDx0bywgMjcvMDEgOzIwMTEtMDEtMjc+O3A8ZnIsIDI4LzAxIDsyMDExLTAxLTI4PjtwPGzDtiwgMjkvMDEgOzIwMTEtMDEtMjk+O3A8c8O2LCAzMC8wMSA7MjAxMS0wMS0zMD47Pj47Pjs7Pjt0PHQ8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47PjtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0Pjs+O2w8cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIFRydWNrZsO2cmFyZTs2MjAwPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgTGFnZXJhcmJldGFyZTs1OTgzPjtwPC0tLTstMT47cDxcZTtfQUNfPjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxsPFZpc2libGU7PjtsPG88Zj47Pj47bDxpPDE+Oz47bDx0PDtsPGk8MT47PjtsPHQ8cDxwPGw8VGV4dDs+O2w8XGU7Pj47Pjs7Pjs+Pjs+Pjs+Pjt0PHA8cDxsPFRleHQ7PjtsPFVwcGRhdGVyYTs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDtWaXNpYmxlOz47bDxWZWNrYSBLbGFyO288Zj47Pj47Pjs7Pjt0PHA8cDxsPFRleHQ7VmlzaWJsZTs+O2w8w4RuZHJhIHZlY2thO288dD47Pj47Pjs7Pjs+Pjs+PjtsPE9sZFJvd3NSZXBlYXRlcjpfY3RsMDpDaGVja2JveERheURvbmU7RnVsbERheUNoZWNrQm94Oz4+')
        . '&DoPost=true&CustOrdersDropDown=0&OldRowsRepeater%3A_ctl0%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl0%3ABreakHidden=none&OldRowsRepeater%3A_ctl0%3AOverTimeHidden=none&AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&ChangeButton=%C3%84ndra+vecka';
    $expectedPost = toPostHash($postString);

    $this->server->expects($this->at(0))
                 ->method('post')
                 ->with($expectedAction, $expectedPost)
                 ->will($this->returnValue($this->readFile('intellitime-v9-timereport-single-open.txt')));
    $post = $weekData->buildPost($updatedReports);
    $this->assertInstanceOf('IntellitimeWeekUnlockPost', $post);
    $weekData = new TZIntellitimeWeekData($post->post());
    $updatedReports = $weekData->postProcessPost($post, $updatedReports, $this->account);

    // Second POST, expect a post to update the old report
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8aHR0cDovL2lwd2ViLmludGVsbGlwbGFuLnNlL2t1bmRsb2dvLzQwOTQuanBnOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxKb2hhbiBIZWFuZGVyOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8NDs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjtpPDI+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+O3A8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBMYWdlcmFyYmV0YXJlOzU5ODM+Oz4+O2w8aTwwPjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTw1Pjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwxPjs+PjtsPGk8MD47PjtsPHQ8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxcZTtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMztFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMzs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcPG9wdGlvblw+W0tsaWNrYSBow6RyXVw8L29wdGlvblw+Oz47Oz47Pj47Pj47Pj47dDw7bDxpPDEzPjtpPDE1Pjs+O2w8dDx0PDtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0PjtpPDU+O2k8Nj47aTw3Pjs+O2w8cDxtw6UsIDI0LzAxIDsyMDExLTAxLTI0PjtwPHRpLCAyNS8wMSA7MjAxMS0wMS0yNT47cDxvbiwgMjYvMDEgOzIwMTEtMDEtMjY+O3A8dG8sIDI3LzAxIDsyMDExLTAxLTI3PjtwPGZyLCAyOC8wMSA7MjAxMS0wMS0yOD47cDxsw7YsIDI5LzAxIDsyMDExLTAxLTI5PjtwPHPDtiwgMzAvMDEgOzIwMTEtMDEtMzA+Oz4+Oz47Oz47dDx0PDtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0Pjs+O2w8cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIFRydWNrZsO2cmFyZTs2MjAwPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgTGFnZXJhcmJldGFyZTs1OTgzPjtwPC0tLTstMT47cDxcZTtfQUNfPjs+Pjs+Ozs+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8VXBwZGF0ZXJhOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxWZWNrYSBLbGFyOz4+Oz47Oz47dDxwPHA8bDxUZXh0O1Zpc2libGU7PjtsPMOEbmRyYSB2ZWNrYTtvPGY+Oz4+Oz47Oz47Pj47Pj47bDxPbGRSb3dzUmVwZWF0ZXI6X2N0bDA6Q2hlY2tib3hEYXlEb25lO09sZFJvd3NSZXBlYXRlcjpfY3RsMDpDaGVja0JveERlbGV0ZTtGdWxsRGF5Q2hlY2tCb3g7Pj4=')
        . '&DoPost=true&CustOrdersDropDown=0&OldRowsRepeater%3A_ctl0%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl0%3ATextboxTimeFrom=08%3A05&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ATextboxTimeTo=15%3A10&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl0%3ATextboxNote=&OldRowsRepeater%3A_ctl0%3ATextboxBreak=15&OldRowsRepeater%3A_ctl0%3ABreakHidden=none&OldRowsRepeater%3A_ctl0%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl0%3AOverTimeHidden=none&AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&DoneButton=Vecka+Klar';
    $expectedPost = toPostHash($postString);
    $this->server->expects($this->at(0))
                 ->method('post')
                 ->with($expectedAction, $expectedPost)
                 ->will($this->returnValue($this->readFile('intellitime-v9-timereport-single-done.txt')));
    $post = $weekData->buildPost($updatedReports);
    $this->assertInstanceOf('IntellitimeWeekUpdatePost', $post);
    $weekData = new TZIntellitimeWeekData($post->post());
    $updatedReports = $weekData->postProcessPost($post, $updatedReports, $this->account);

    // Third POST, insert the new report
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8aHR0cDovL2lwd2ViLmludGVsbGlwbGFuLnNlL2t1bmRsb2dvLzQwOTQuanBnOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxKb2hhbiBIZWFuZGVyOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8NDs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjtpPDI+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+O3A8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBMYWdlcmFyYmV0YXJlOzU5ODM+Oz4+O2w8aTwwPjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTw0Pjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwxPjs+PjtsPGk8MD47PjtsPHQ8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxkaXNhYmxlZDtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMztFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMzs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcZTs+Ozs+Oz4+Oz4+Oz4+O3Q8O2w8aTwxMz47aTwxNT47aTwxNz47aTwxOT47aTwyMT47aTwyMz47aTwyNz47aTwyOT47aTwzMz47PjtsPHQ8dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+O3A8bDxpPDE+O2k8Mj47aTwzPjtpPDQ+O2k8NT47aTw2PjtpPDc+Oz47bDxwPG3DpSwgMjQvMDEgOzIwMTEtMDEtMjQ+O3A8dGksIDI1LzAxIDsyMDExLTAxLTI1PjtwPG9uLCAyNi8wMSA7MjAxMS0wMS0yNj47cDx0bywgMjcvMDEgOzIwMTEtMDEtMjc+O3A8ZnIsIDI4LzAxIDsyMDExLTAxLTI4PjtwPGzDtiwgMjkvMDEgOzIwMTEtMDEtMjk+O3A8c8O2LCAzMC8wMSA7MjAxMS0wMS0zMD47Pj47Pjs7Pjt0PHQ8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47PjtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0Pjs+O2w8cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIFRydWNrZsO2cmFyZTs2MjAwPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgTGFnZXJhcmJldGFyZTs1OTgzPjtwPC0tLTstMT47cDxcZTtfQUNfPjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxsPFZpc2libGU7PjtsPG88Zj47Pj47bDxpPDE+Oz47bDx0PDtsPGk8MT47PjtsPHQ8cDxwPGw8VGV4dDs+O2w8XGU7Pj47Pjs7Pjs+Pjs+Pjs+Pjt0PHA8cDxsPFRleHQ7PjtsPFVwcGRhdGVyYTs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDtWaXNpYmxlOz47bDxWZWNrYSBLbGFyO288Zj47Pj47Pjs7Pjt0PHA8cDxsPFRleHQ7VmlzaWJsZTs+O2w8w4RuZHJhIHZlY2thO288dD47Pj47Pjs7Pjs+Pjs+PjtsPE9sZFJvd3NSZXBlYXRlcjpfY3RsMDpDaGVja2JveERheURvbmU7RnVsbERheUNoZWNrQm94Oz4+')
        . '&DoPost=true&CustOrdersDropDown=0&OldRowsRepeater%3A_ctl0%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl0%3ABreakHidden=none&OldRowsRepeater%3A_ctl0%3AOverTimeHidden=none&AddDateDropDown=2011-01-26&AddRowDropDown=6200&AddTimeFromTextBox=08%3A00&AddTimeToTextBox=16%3A30&AddBreakTextBox=30&AddExplicitOvertimeTextBox=&AddNoteTextBox=&UpdateButton=Uppdatera';
    $expectedPost = toPostHash($postString);
    $this->server->expects($this->at(0))
                 ->method('post')
                 ->with($expectedAction, $expectedPost)
                 ->will($this->returnValue($this->readFile('intellitime-v9-timereport-one-done-one-open.txt')));
    $post = $weekData->buildPost($updatedReports);
    $this->assertInstanceOf('IntellitimeWeekInsertPost', $post);
    $weekData = new TZIntellitimeWeekData($post->post());
    $updatedReports = $weekData->postProcessPost($post, $updatedReports, $this->account);
    $this->assertThat($updatedReports[1]->intellitime_id, $this->stringContains('mhbLP96iqH3NYRRYH%2fOlM4hbku5Eii3'));

    // Fourth POST, unlock the old report
    $this->server->expects($this->at(0))
                 ->method('post')
                 ->with($expectedAction, $this->anything())
                 ->will($this->returnValue($this->readFile('intellitime-v9-timereport-two-open.txt')));
    $post = $weekData->buildPost($updatedReports);
    $this->assertInstanceOf('IntellitimeWeekUnlockPost', $post);
    $weekData = new TZIntellitimeWeekData($post->post());
    $updatedReports = $weekData->postProcessPost($post, $updatedReports, $this->account);

    // Fifth POST, update and lock both reports
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8aHR0cDovL2lwd2ViLmludGVsbGlwbGFuLnNlL2t1bmRsb2dvLzQwOTQuanBnOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxKb2hhbiBIZWFuZGVyOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8NDs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjtpPDI+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+O3A8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBMYWdlcmFyYmV0YXJlOzU5ODM+Oz4+O2w8aTwwPjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTw0Pjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwxPjs+PjtsPGk8MD47PjtsPHQ8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxkaXNhYmxlZDtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMztFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMzs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcZTs+Ozs+Oz4+Oz4+Oz4+O3Q8O2w8aTwxMz47aTwxNT47aTwxNz47aTwxOT47aTwyMT47aTwyMz47aTwyNz47aTwyOT47aTwzMz47PjtsPHQ8dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+O3A8bDxpPDE+O2k8Mj47aTwzPjtpPDQ+O2k8NT47aTw2PjtpPDc+Oz47bDxwPG3DpSwgMjQvMDEgOzIwMTEtMDEtMjQ+O3A8dGksIDI1LzAxIDsyMDExLTAxLTI1PjtwPG9uLCAyNi8wMSA7MjAxMS0wMS0yNj47cDx0bywgMjcvMDEgOzIwMTEtMDEtMjc+O3A8ZnIsIDI4LzAxIDsyMDExLTAxLTI4PjtwPGzDtiwgMjkvMDEgOzIwMTEtMDEtMjk+O3A8c8O2LCAzMC8wMSA7MjAxMS0wMS0zMD47Pj47Pjs7Pjt0PHQ8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47PjtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0Pjs+O2w8cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIFRydWNrZsO2cmFyZTs2MjAwPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgTGFnZXJhcmJldGFyZTs1OTgzPjtwPC0tLTstMT47cDxcZTtfQUNfPjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxsPFZpc2libGU7PjtsPG88Zj47Pj47bDxpPDE+Oz47bDx0PDtsPGk8MT47PjtsPHQ8cDxwPGw8VGV4dDs+O2w8XGU7Pj47Pjs7Pjs+Pjs+Pjs+Pjt0PHA8cDxsPFRleHQ7PjtsPFVwcGRhdGVyYTs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDtWaXNpYmxlOz47bDxWZWNrYSBLbGFyO288Zj47Pj47Pjs7Pjt0PHA8cDxsPFRleHQ7VmlzaWJsZTs+O2w8w4RuZHJhIHZlY2thO288dD47Pj47Pjs7Pjs+Pjs+PjtsPE9sZFJvd3NSZXBlYXRlcjpfY3RsMDpDaGVja2JveERheURvbmU7RnVsbERheUNoZWNrQm94Oz4+')
        . '&DoPost=true&CustOrdersDropDown=0&'
        . 'OldRowsRepeater%3A_ctl0%3ABreakHidden=none&OldRowsRepeater%3A_ctl0%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl0%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl0%3ATextboxBreak=15&OldRowsRepeater%3A_ctl0%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl0%3ATextboxNote=&OldRowsRepeater%3A_ctl0%3ATextboxTimeFrom=08%3A05&OldRowsRepeater%3A_ctl0%3ATextboxTimeTo=15%3A10&'
        . 'OldRowsRepeater%3A_ctl1%3ABreakHidden=none&OldRowsRepeater%3A_ctl1%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl1%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl1%3ADateToHidden=16%3A30&OldRowsRepeater%3A_ctl1%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl1%3ATextboxBreak=30&OldRowsRepeater%3A_ctl1%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl1%3ATextboxNote=&OldRowsRepeater%3A_ctl1%3ATextboxTimeFrom=08%3A00&OldRowsRepeater%3A_ctl1%3ATextboxTimeTo=16%3A30&'
        . 'AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&DoneButton=Vecka+Klar';
    $expectedPost = toPostHash($postString);
    $this->server->expects($this->at(0))
                 ->method('post')
                 ->with($expectedAction, $expectedPost)
                 ->will($this->returnValue($this->readFile('intellitime-v9-timereport-two-done.txt')));
    $post = $weekData->buildPost($updatedReports);
    $this->assertInstanceOf('IntellitimeWeekUpdatePost', $post);
    $weekData = new TZIntellitimeWeekData($post->post());
    $updatedReports = $weekData->postProcessPost($post, $updatedReports, $this->account);

    // Fifth round, no POST this time
    $post = $weekData->buildPost($updatedReports);
    $this->assertNull($post);

    // Run final update and check report states
    $updatedReports = $weekData->updateReports($updatedReports, $this->account, TRUE);
    $this->assertEquals(2, count($updatedReports));
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[0]->flags);
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[1]->flags);
    $this->assertReportTimes('2011-01-26', '08:00', '16:30', 30, $updatedReports[1]);
    $this->assertThat($updatedReports[1]->intellitime_id, $this->stringContains('mhbLP96iqH3NYRRYH%2fOlM4hbku5Eii3'));
  }

  function testDeletedNewAndUpdated() {
    /* Changes an existing report, deletes one report and adds a new report
     * at the same time. Will need multiple posts to ensure update, first unlock,
     * then update the changed report, then add the new report and finally update
     * the new report.
     */
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-two-done.txt');
    $expectedAction = 'TimeReport/TimeReport.aspx?DateInWeek=2011-01-28';

    $reports = array(
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:05', '15:10', 15, 1, TZFlags::REPORTED),
      createMockReport('mhbLP96iqH3NYRRYH%2fOlM4hbku5Eii3', '2011-01-26', '08:00', '16:30', 30, 1, TZFlags::DELETED),
      createMockReport(NULL, '2011-01-26', '08:00', '16:30', 30, 1, TZFlags::REPORTED),
    );
    $reports[2]->intellitime_jobid = 6200;

    $updatedReports = $weekData->updateReports($reports, $this->account);
    $this->assertEquals(3, count($updatedReports));
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[0]->flags);
    $this->assertEquals(TZFlags::DELETED, $updatedReports[1]->flags);
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[2]->flags);

    // First POST, expect a unlock
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8aHR0cDovL2lwd2ViLmludGVsbGlwbGFuLnNlL2t1bmRsb2dvLzQwOTQuanBnOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxKb2hhbiBIZWFuZGVyOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8NDs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjtpPDI+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+O3A8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBMYWdlcmFyYmV0YXJlOzU5ODM+Oz4+O2w8aTwwPjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTw0Pjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwxPjs+PjtsPGk8MD47PjtsPHQ8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxkaXNhYmxlZDtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMztFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMzs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcZTs+Ozs+Oz4+Oz4+Oz4+O3Q8O2w8aTwxMz47aTwxNT47aTwxNz47aTwxOT47aTwyMT47aTwyMz47aTwyNz47aTwyOT47aTwzMz47PjtsPHQ8dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+O3A8bDxpPDE+O2k8Mj47aTwzPjtpPDQ+O2k8NT47aTw2PjtpPDc+Oz47bDxwPG3DpSwgMjQvMDEgOzIwMTEtMDEtMjQ+O3A8dGksIDI1LzAxIDsyMDExLTAxLTI1PjtwPG9uLCAyNi8wMSA7MjAxMS0wMS0yNj47cDx0bywgMjcvMDEgOzIwMTEtMDEtMjc+O3A8ZnIsIDI4LzAxIDsyMDExLTAxLTI4PjtwPGzDtiwgMjkvMDEgOzIwMTEtMDEtMjk+O3A8c8O2LCAzMC8wMSA7MjAxMS0wMS0zMD47Pj47Pjs7Pjt0PHQ8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47PjtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0Pjs+O2w8cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIFRydWNrZsO2cmFyZTs2MjAwPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgTGFnZXJhcmJldGFyZTs1OTgzPjtwPC0tLTstMT47cDxcZTtfQUNfPjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxsPFZpc2libGU7PjtsPG88Zj47Pj47bDxpPDE+Oz47bDx0PDtsPGk8MT47PjtsPHQ8cDxwPGw8VGV4dDs+O2w8XGU7Pj47Pjs7Pjs+Pjs+Pjs+Pjt0PHA8cDxsPFRleHQ7PjtsPFVwcGRhdGVyYTs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDtWaXNpYmxlOz47bDxWZWNrYSBLbGFyO288Zj47Pj47Pjs7Pjt0PHA8cDxsPFRleHQ7VmlzaWJsZTs+O2w8w4RuZHJhIHZlY2thO288dD47Pj47Pjs7Pjs+Pjs+PjtsPE9sZFJvd3NSZXBlYXRlcjpfY3RsMDpDaGVja2JveERheURvbmU7RnVsbERheUNoZWNrQm94Oz4+')
        . '&DoPost=true&CustOrdersDropDown=0'
        . '&OldRowsRepeater%3A_ctl0%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl0%3ABreakHidden=none&OldRowsRepeater%3A_ctl0%3AOverTimeHidden=none'
        . '&OldRowsRepeater%3A_ctl1%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl1%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl1%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl1%3ABreakHidden=none&OldRowsRepeater%3A_ctl1%3AOverTimeHidden=none'
        . '&AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&ChangeButton=%C3%84ndra+vecka';
    $expectedPost = toPostHash($postString);

    $this->server->expects($this->at(0))
                 ->method('post')
                 ->with($expectedAction, $expectedPost)
                 ->will($this->returnValue($this->readFile('intellitime-v9-timereport-two-open.txt')));
    $post = $weekData->buildPost($updatedReports);
    $this->assertInstanceOf('IntellitimeWeekUnlockPost', $post);
    $weekData = new TZIntellitimeWeekData($post->post());
    $updatedReports = $weekData->postProcessPost($post, $updatedReports, $this->account);

    // Second POST, expect a post to delete the second report
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8aHR0cDovL2lwd2ViLmludGVsbGlwbGFuLnNlL2t1bmRsb2dvLzQwOTQuanBnOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxKb2hhbiBIZWFuZGVyOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8NDs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjtpPDI+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+O3A8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBMYWdlcmFyYmV0YXJlOzU5ODM+Oz4+O2w8aTwwPjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTw0Pjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwxPjs+PjtsPGk8MD47PjtsPHQ8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxkaXNhYmxlZDtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMztFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMzs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcZTs+Ozs+Oz4+Oz4+Oz4+O3Q8O2w8aTwxMz47aTwxNT47aTwxNz47aTwxOT47aTwyMT47aTwyMz47aTwyNz47aTwyOT47aTwzMz47PjtsPHQ8dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+O3A8bDxpPDE+O2k8Mj47aTwzPjtpPDQ+O2k8NT47aTw2PjtpPDc+Oz47bDxwPG3DpSwgMjQvMDEgOzIwMTEtMDEtMjQ+O3A8dGksIDI1LzAxIDsyMDExLTAxLTI1PjtwPG9uLCAyNi8wMSA7MjAxMS0wMS0yNj47cDx0bywgMjcvMDEgOzIwMTEtMDEtMjc+O3A8ZnIsIDI4LzAxIDsyMDExLTAxLTI4PjtwPGzDtiwgMjkvMDEgOzIwMTEtMDEtMjk+O3A8c8O2LCAzMC8wMSA7MjAxMS0wMS0zMD47Pj47Pjs7Pjt0PHQ8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47PjtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0Pjs+O2w8cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIFRydWNrZsO2cmFyZTs2MjAwPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgTGFnZXJhcmJldGFyZTs1OTgzPjtwPC0tLTstMT47cDxcZTtfQUNfPjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxsPFZpc2libGU7PjtsPG88Zj47Pj47bDxpPDE+Oz47bDx0PDtsPGk8MT47PjtsPHQ8cDxwPGw8VGV4dDs+O2w8XGU7Pj47Pjs7Pjs+Pjs+Pjs+Pjt0PHA8cDxsPFRleHQ7PjtsPFVwcGRhdGVyYTs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDtWaXNpYmxlOz47bDxWZWNrYSBLbGFyO288Zj47Pj47Pjs7Pjt0PHA8cDxsPFRleHQ7VmlzaWJsZTs+O2w8w4RuZHJhIHZlY2thO288dD47Pj47Pjs7Pjs+Pjs+PjtsPE9sZFJvd3NSZXBlYXRlcjpfY3RsMDpDaGVja2JveERheURvbmU7RnVsbERheUNoZWNrQm94Oz4+')
        . '&DoPost=true&CustOrdersDropDown=0'
        . '&OldRowsRepeater%3A_ctl0%3ATextboxTimeFrom=08%3A00&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ATextboxTimeTo=17%3A00&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl0%3ATextboxNote=&OldRowsRepeater%3A_ctl0%3ATextboxBreak=60&OldRowsRepeater%3A_ctl0%3ABreakHidden=none&OldRowsRepeater%3A_ctl0%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl0%3AOverTimeHidden=none'
        . '&OldRowsRepeater%3A_ctl1%3ACheckBoxDelete=on&OldRowsRepeater%3A_ctl1%3ATextboxTimeFrom=08%3A00&OldRowsRepeater%3A_ctl1%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl1%3ATextboxTimeTo=16%3A30&OldRowsRepeater%3A_ctl1%3ADateToHidden=16%3A30&OldRowsRepeater%3A_ctl1%3ATextboxNote=&OldRowsRepeater%3A_ctl1%3ATextboxBreak=30&OldRowsRepeater%3A_ctl1%3ABreakHidden=none&OldRowsRepeater%3A_ctl1%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl1%3AOverTimeHidden=none'
        . '&AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&UpdateButton=Uppdatera';
    $expectedPost = toPostHash($postString);
    $this->server->expects($this->at(0))
                 ->method('post')
                 ->with($expectedAction, $expectedPost)
                 ->will($this->returnValue($this->readFile('intellitime-v9-timereport-single-open.txt')));
    $post = $weekData->buildPost($updatedReports);
    $this->assertInstanceOf('IntellitimeWeekDeletePost', $post);
    $weekData = new TZIntellitimeWeekData($post->post());
    $updatedReports = $weekData->postProcessPost($post, $updatedReports, $this->account);

    // Third POST, expect a post to update the first report
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8aHR0cDovL2lwd2ViLmludGVsbGlwbGFuLnNlL2t1bmRsb2dvLzQwOTQuanBnOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxKb2hhbiBIZWFuZGVyOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8NDs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjtpPDI+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+O3A8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBMYWdlcmFyYmV0YXJlOzU5ODM+Oz4+O2w8aTwwPjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTw1Pjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwxPjs+PjtsPGk8MD47PjtsPHQ8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxcZTtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMztFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMzs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcPG9wdGlvblw+W0tsaWNrYSBow6RyXVw8L29wdGlvblw+Oz47Oz47Pj47Pj47Pj47dDw7bDxpPDEzPjtpPDE1Pjs+O2w8dDx0PDtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0PjtpPDU+O2k8Nj47aTw3Pjs+O2w8cDxtw6UsIDI0LzAxIDsyMDExLTAxLTI0PjtwPHRpLCAyNS8wMSA7MjAxMS0wMS0yNT47cDxvbiwgMjYvMDEgOzIwMTEtMDEtMjY+O3A8dG8sIDI3LzAxIDsyMDExLTAxLTI3PjtwPGZyLCAyOC8wMSA7MjAxMS0wMS0yOD47cDxsw7YsIDI5LzAxIDsyMDExLTAxLTI5PjtwPHPDtiwgMzAvMDEgOzIwMTEtMDEtMzA+Oz4+Oz47Oz47dDx0PDtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0Pjs+O2w8cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIFRydWNrZsO2cmFyZTs2MjAwPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgTGFnZXJhcmJldGFyZTs1OTgzPjtwPC0tLTstMT47cDxcZTtfQUNfPjs+Pjs+Ozs+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8VXBwZGF0ZXJhOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxWZWNrYSBLbGFyOz4+Oz47Oz47dDxwPHA8bDxUZXh0O1Zpc2libGU7PjtsPMOEbmRyYSB2ZWNrYTtvPGY+Oz4+Oz47Oz47Pj47Pj47bDxPbGRSb3dzUmVwZWF0ZXI6X2N0bDA6Q2hlY2tib3hEYXlEb25lO09sZFJvd3NSZXBlYXRlcjpfY3RsMDpDaGVja0JveERlbGV0ZTtGdWxsRGF5Q2hlY2tCb3g7Pj4=')
        . '&DoPost=true&CustOrdersDropDown=0&OldRowsRepeater%3A_ctl0%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl0%3ATextboxTimeFrom=08%3A05&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ATextboxTimeTo=15%3A10&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl0%3ATextboxNote=&OldRowsRepeater%3A_ctl0%3ATextboxBreak=15&OldRowsRepeater%3A_ctl0%3ABreakHidden=none&OldRowsRepeater%3A_ctl0%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl0%3AOverTimeHidden=none&AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&DoneButton=Vecka+Klar';
    $expectedPost = toPostHash($postString);
    $this->server->expects($this->at(0))
                 ->method('post')
                 ->with($expectedAction, $expectedPost)
                 ->will($this->returnValue($this->readFile('intellitime-v9-timereport-single-done.txt')));
    $post = $weekData->buildPost($updatedReports);
    $this->assertInstanceOf('IntellitimeWeekUpdatePost', $post);
    $weekData = new TZIntellitimeWeekData($post->post());
    $updatedReports = $weekData->postProcessPost($post, $updatedReports, $this->account);

    // Fourth POST, insert the new report
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8aHR0cDovL2lwd2ViLmludGVsbGlwbGFuLnNlL2t1bmRsb2dvLzQwOTQuanBnOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxKb2hhbiBIZWFuZGVyOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8NDs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjtpPDI+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+O3A8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBMYWdlcmFyYmV0YXJlOzU5ODM+Oz4+O2w8aTwwPjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTw0Pjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwxPjs+PjtsPGk8MD47PjtsPHQ8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxkaXNhYmxlZDtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMztFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMzs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcZTs+Ozs+Oz4+Oz4+Oz4+O3Q8O2w8aTwxMz47aTwxNT47aTwxNz47aTwxOT47aTwyMT47aTwyMz47aTwyNz47aTwyOT47aTwzMz47PjtsPHQ8dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+O3A8bDxpPDE+O2k8Mj47aTwzPjtpPDQ+O2k8NT47aTw2PjtpPDc+Oz47bDxwPG3DpSwgMjQvMDEgOzIwMTEtMDEtMjQ+O3A8dGksIDI1LzAxIDsyMDExLTAxLTI1PjtwPG9uLCAyNi8wMSA7MjAxMS0wMS0yNj47cDx0bywgMjcvMDEgOzIwMTEtMDEtMjc+O3A8ZnIsIDI4LzAxIDsyMDExLTAxLTI4PjtwPGzDtiwgMjkvMDEgOzIwMTEtMDEtMjk+O3A8c8O2LCAzMC8wMSA7MjAxMS0wMS0zMD47Pj47Pjs7Pjt0PHQ8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47PjtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0Pjs+O2w8cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIFRydWNrZsO2cmFyZTs2MjAwPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgTGFnZXJhcmJldGFyZTs1OTgzPjtwPC0tLTstMT47cDxcZTtfQUNfPjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxsPFZpc2libGU7PjtsPG88Zj47Pj47bDxpPDE+Oz47bDx0PDtsPGk8MT47PjtsPHQ8cDxwPGw8VGV4dDs+O2w8XGU7Pj47Pjs7Pjs+Pjs+Pjs+Pjt0PHA8cDxsPFRleHQ7PjtsPFVwcGRhdGVyYTs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDtWaXNpYmxlOz47bDxWZWNrYSBLbGFyO288Zj47Pj47Pjs7Pjt0PHA8cDxsPFRleHQ7VmlzaWJsZTs+O2w8w4RuZHJhIHZlY2thO288dD47Pj47Pjs7Pjs+Pjs+PjtsPE9sZFJvd3NSZXBlYXRlcjpfY3RsMDpDaGVja2JveERheURvbmU7RnVsbERheUNoZWNrQm94Oz4+')
        . '&DoPost=true&CustOrdersDropDown=0'
        . '&OldRowsRepeater%3A_ctl0%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl0%3ABreakHidden=none&OldRowsRepeater%3A_ctl0%3AOverTimeHidden=none'
        . '&AddDateDropDown=2011-01-26&AddRowDropDown=6200&AddTimeFromTextBox=08%3A00&AddTimeToTextBox=16%3A30&AddBreakTextBox=30&AddExplicitOvertimeTextBox=&AddNoteTextBox=&UpdateButton=Uppdatera';
    $expectedPost = toPostHash($postString);
    $this->server->expects($this->at(0))
                 ->method('post')
                 ->with($expectedAction, $expectedPost)
                 ->will($this->returnValue(
                          str_replace(
                            'mhbLP96iqH3NYRRYH%2fOlM4hbku5Eii3',
                            'mhbLP96iqH3ANOTHER%2fOlM4hbku5Eii3',
                            $this->readFile('intellitime-v9-timereport-one-done-one-open.txt')
                          )));
    $post = $weekData->buildPost($updatedReports);
    $this->assertInstanceOf('IntellitimeWeekInsertPost', $post);
    $weekData = new TZIntellitimeWeekData($post->post());
    $updatedReports = $weekData->postProcessPost($post, $updatedReports, $this->account);
    $this->assertThat($updatedReports[2]->intellitime_id, $this->stringContains('mhbLP96iqH3ANOTHER%2fOlM4hbku5Eii3'));

    // Fifth POST, unlock the old report
    $this->server->expects($this->at(0))
                 ->method('post')
                 ->with($expectedAction, $this->anything())
                 ->will($this->returnValue(
                          str_replace(
                            'mhbLP96iqH3NYRRYH%2fOlM4hbku5Eii3',
                            'mhbLP96iqH3ANOTHER%2fOlM4hbku5Eii3',
                            $this->readFile('intellitime-v9-timereport-two-open.txt')
                          )));
    $post = $weekData->buildPost($updatedReports);
    $this->assertInstanceOf('IntellitimeWeekUnlockPost', $post);
    $weekData = new TZIntellitimeWeekData($post->post());
    $updatedReports = $weekData->postProcessPost($post, $updatedReports, $this->account);

    // Sixth POST, update and lock both reports
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8aHR0cDovL2lwd2ViLmludGVsbGlwbGFuLnNlL2t1bmRsb2dvLzQwOTQuanBnOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxKb2hhbiBIZWFuZGVyOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8NDs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjtpPDI+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+O3A8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBMYWdlcmFyYmV0YXJlOzU5ODM+Oz4+O2w8aTwwPjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTw0Pjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwxPjs+PjtsPGk8MD47PjtsPHQ8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxkaXNhYmxlZDtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMztFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMzs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcZTs+Ozs+Oz4+Oz4+Oz4+O3Q8O2w8aTwxMz47aTwxNT47aTwxNz47aTwxOT47aTwyMT47aTwyMz47aTwyNz47aTwyOT47aTwzMz47PjtsPHQ8dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+O3A8bDxpPDE+O2k8Mj47aTwzPjtpPDQ+O2k8NT47aTw2PjtpPDc+Oz47bDxwPG3DpSwgMjQvMDEgOzIwMTEtMDEtMjQ+O3A8dGksIDI1LzAxIDsyMDExLTAxLTI1PjtwPG9uLCAyNi8wMSA7MjAxMS0wMS0yNj47cDx0bywgMjcvMDEgOzIwMTEtMDEtMjc+O3A8ZnIsIDI4LzAxIDsyMDExLTAxLTI4PjtwPGzDtiwgMjkvMDEgOzIwMTEtMDEtMjk+O3A8c8O2LCAzMC8wMSA7MjAxMS0wMS0zMD47Pj47Pjs7Pjt0PHQ8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47PjtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0Pjs+O2w8cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIFRydWNrZsO2cmFyZTs2MjAwPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgTGFnZXJhcmJldGFyZTs1OTgzPjtwPC0tLTstMT47cDxcZTtfQUNfPjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxsPFZpc2libGU7PjtsPG88Zj47Pj47bDxpPDE+Oz47bDx0PDtsPGk8MT47PjtsPHQ8cDxwPGw8VGV4dDs+O2w8XGU7Pj47Pjs7Pjs+Pjs+Pjs+Pjt0PHA8cDxsPFRleHQ7PjtsPFVwcGRhdGVyYTs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDtWaXNpYmxlOz47bDxWZWNrYSBLbGFyO288Zj47Pj47Pjs7Pjt0PHA8cDxsPFRleHQ7VmlzaWJsZTs+O2w8w4RuZHJhIHZlY2thO288dD47Pj47Pjs7Pjs+Pjs+PjtsPE9sZFJvd3NSZXBlYXRlcjpfY3RsMDpDaGVja2JveERheURvbmU7RnVsbERheUNoZWNrQm94Oz4+')
        . '&DoPost=true&CustOrdersDropDown=0&'
        . 'OldRowsRepeater%3A_ctl0%3ABreakHidden=none&OldRowsRepeater%3A_ctl0%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl0%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl0%3ATextboxBreak=15&OldRowsRepeater%3A_ctl0%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl0%3ATextboxNote=&OldRowsRepeater%3A_ctl0%3ATextboxTimeFrom=08%3A05&OldRowsRepeater%3A_ctl0%3ATextboxTimeTo=15%3A10&'
        . 'OldRowsRepeater%3A_ctl1%3ABreakHidden=none&OldRowsRepeater%3A_ctl1%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl1%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl1%3ADateToHidden=16%3A30&OldRowsRepeater%3A_ctl1%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl1%3ATextboxBreak=30&OldRowsRepeater%3A_ctl1%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl1%3ATextboxNote=&OldRowsRepeater%3A_ctl1%3ATextboxTimeFrom=08%3A00&OldRowsRepeater%3A_ctl1%3ATextboxTimeTo=16%3A30&'
        . 'AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&DoneButton=Vecka+Klar';
    $expectedPost = toPostHash($postString);
    $this->server->expects($this->at(0))
                 ->method('post')
                 ->with($expectedAction, $expectedPost)
                 ->will($this->returnValue(
                          str_replace(
                            'mhbLP96iqH3NYRRYH%2fOlM4hbku5Eii3',
                            'mhbLP96iqH3ANOTHER%2fOlM4hbku5Eii3',
                            $this->readFile('intellitime-v9-timereport-two-done.txt')
                          )));
    $post = $weekData->buildPost($updatedReports);
    $this->assertInstanceOf('IntellitimeWeekUpdatePost', $post);
    $weekData = new TZIntellitimeWeekData($post->post());
    $updatedReports = $weekData->postProcessPost($post, $updatedReports, $this->account);

    // Seventh round, no POST this time
    $post = $weekData->buildPost($updatedReports);
    $this->assertNull($post);

    // Run final update and check report states
    $updatedReports = $weekData->updateReports($updatedReports, $this->account, TRUE);
    $this->assertEquals(3, count($updatedReports));
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[0]->flags);
    $this->assertEquals(0, $updatedReports[0]->intellitime_local_changes);
    $this->assertEquals(TZFlags::DELETED, $updatedReports[1]->flags);
    $this->assertEquals(0, $updatedReports[1]->intellitime_local_changes);
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[2]->flags);
    $this->assertReportTimes('2011-01-26', '08:00', '16:30', 30, $updatedReports[2]);
    $this->assertThat($updatedReports[2]->intellitime_id, $this->stringContains('mhbLP96iqH3ANOTHER%2fOlM4hbku5Eii3'));
    $this->assertEquals(0, $updatedReports[2]->intellitime_local_changes);
  }

  function testV9AdminAbsenceAndRegularReportsCanBeMarkedDoneForWholeWeek() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-one-absence-two-done-one-open.txt');

    $reports = array(
      createMockReport('mhbLP96iqH3ZMQKVB6I6Qc4hbku5Eii3', '2011-01-10', '08:00', '17:00'),
      createMockReport('mhbLP96iqH0993oUoX8OKM4hbku5Eii3', '2011-01-11', '08:00', '17:00'),
      createMockReport('mhbLP96iqH2xpPwR07B5Nc4hbku5Eii3', '2011-01-12', '08:15', '17:15'),
      createMockReport('mhbLP96iqH1AhkChU548NM4hbku5Eii3', '2011-01-13', '08:00', '17:30'),
    );
    $reports[0]->intellitime_local_changes = 0;
    $reports[1]->intellitime_local_changes = 0;
    $reports[3]->intellitime_local_changes = 0;

    $updatedReports = $weekData->updateReports($reports, $this->account);
    foreach($updatedReports as $report) {
      $this->assertEquals(TZFlags::REPORTED, $report->flags);
    }

    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8aHR0cDovL2lwd2ViLmludGVsbGlwbGFuLnNlL2t1bmRsb2dvLzQwOTQuanBnOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxNaWthZWwgT2hsc29uOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8Mjs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjtpPDI+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgTGFnZXJhcmJldGFyZTs1OTgzPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+Oz4+O2w8aTwwPjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTw0Pjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTw0Pjs+PjtsPGk8MD47aTwxPjtpPDI+O2k8Mz47PjtsPHQ8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxkaXNhYmxlZDtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNaTVFLVkI2STZRYzRoYmt1NUVpaTM7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgzWk1RS1ZCNkk2UWM0aGJrdTVFaWkzOz47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8MD47Pj47Oz47dDxAPFxlOz47Oz47Pj47Pj47dDw7bDxpPDM3Pjs+O2w8dDw7bDxpPDA+O2k8MT47aTwyPjs+O2w8dDxAPGRpc2FibGVkO0V4cGVuc2UuYXNweD9taGJMUDk2aXFIMDk5M29Vb1g4T0tNNGhia3U1RWlpMztFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDA5OTNvVW9YOE9LTTRoYmt1NUVpaTM7Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwwPjs+Pjs7Pjt0PEA8XDxvcHRpb25cPltLbGlja2EgaMOkcl1cPC9vcHRpb25cPjs+Ozs+Oz4+Oz4+O3Q8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxcZTtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDJ4cFB3UjA3QjVOYzRoYmt1NUVpaTM7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgyeHBQd1IwN0I1TmM0aGJrdTVFaWkzOz47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8MD47Pj47Oz47dDxAPFw8b3B0aW9uXD5bS2xpY2thIGjDpHJdXDwvb3B0aW9uXD47Pjs7Pjs+Pjs+Pjt0PDtsPGk8Mzc+Oz47bDx0PDtsPGk8MD47aTwxPjtpPDI+Oz47bDx0PEA8ZGlzYWJsZWQ7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgxQWhrQ2hVNTQ4Tk00aGJrdTVFaWkzO0V4cGVuc2UuYXNweD9taGJMUDk2aXFIMUFoa0NoVTU0OE5NNGhia3U1RWlpMzs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcZTs+Ozs+Oz4+Oz4+Oz4+O3Q8O2w8aTwxMz47aTwxNT47aTwxNz47aTwxOT47aTwyMT47aTwyMz47aTwyNz47aTwyOT47aTwzMz47PjtsPHQ8dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+O3A8bDxpPDE+O2k8Mj47aTwzPjtpPDQ+O2k8NT47aTw2PjtpPDc+Oz47bDxwPG3DpSwgMTAvMDEgOzIwMTEtMDEtMTA+O3A8dGksIDExLzAxIDsyMDExLTAxLTExPjtwPG9uLCAxMi8wMSA7MjAxMS0wMS0xMj47cDx0bywgMTMvMDEgOzIwMTEtMDEtMTM+O3A8ZnIsIDE0LzAxIDsyMDExLTAxLTE0PjtwPGzDtiwgMTUvMDEgOzIwMTEtMDEtMTU+O3A8c8O2LCAxNi8wMSA7MjAxMS0wMS0xNj47Pj47Pjs7Pjt0PHQ8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47PjtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0Pjs+O2w8cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIExhZ2VyYXJiZXRhcmU7NTk4Mz47cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIFRydWNrZsO2cmFyZTs2MjAwPjtwPC0tLTstMT47cDxcZTtfQUNfPjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxsPFZpc2libGU7PjtsPG88Zj47Pj47bDxpPDE+Oz47bDx0PDtsPGk8MT47PjtsPHQ8cDxwPGw8VGV4dDs+O2w8XGU7Pj47Pjs7Pjs+Pjs+Pjs+Pjt0PHA8cDxsPFRleHQ7PjtsPFVwcGRhdGVyYTs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDtWaXNpYmxlOz47bDxWZWNrYSBLbGFyO288dD47Pj47Pjs7Pjt0PHA8cDxsPFRleHQ7VmlzaWJsZTs+O2w8w4RuZHJhIHZlY2thO288Zj47Pj47Pjs7Pjs+Pjs+PjtsPE9sZFJvd3NSZXBlYXRlcjpfY3RsMDpDaGVja2JveERheURvbmU7T2xkUm93c1JlcGVhdGVyOl9jdGwyOkNoZWNrYm94RGF5RG9uZTtPbGRSb3dzUmVwZWF0ZXI6X2N0bDI6Q2hlY2tCb3hEZWxldGU7T2xkUm93c1JlcGVhdGVyOl9jdGwzOkNoZWNrYm94RGF5RG9uZTtGdWxsRGF5Q2hlY2tCb3g7Pj4=')
        . '&DoPost=true&CustOrdersDropDown=0&OldRowsRepeater%3A_ctl0%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl0%3ABreakHidden=none&OldRowsRepeater%3A_ctl0%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl1%3ADateFromHidden=08%3A15&OldRowsRepeater%3A_ctl1%3ADateToHidden=17%3A30&OldRowsRepeater%3A_ctl1%3ABreakHidden=none&OldRowsRepeater%3A_ctl1%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl2%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl2%3ATextboxTimeFrom=08%3A15&OldRowsRepeater%3A_ctl2%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl2%3ATextboxTimeTo=17%3A15&OldRowsRepeater%3A_ctl2%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl2%3ATextboxBreak=60&OldRowsRepeater%3A_ctl2%3ABreakHidden=none&OldRowsRepeater%3A_ctl2%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl2%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl2%3ATextboxNote=&OldRowsRepeater%3A_ctl3%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl3%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl3%3ADateToHidden=17%3A30&OldRowsRepeater%3A_ctl3%3ABreakHidden=none&OldRowsRepeater%3A_ctl3%3AOverTimeHidden=none&AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&DoneButton=Vecka+Klar';
    $postData = toPostHash($postString);
    $this->server->expects($this->once())
                 ->method('post')
                 ->with($this->anything(), $postData)
                 ->will($this->returnValue($this->readFile('intellitime-v9-timereport-one-absence-two-done-one-open.txt')));
    $post = $weekData->buildPost($updatedReports);
    $this->assertInstanceOf('IntellitimeWeekUpdatePost', $post);
    $this->assertTrue($post->getAllReportsDone());
    $page = $post->post();
    $this->assertInstanceOf('IntellitimeWeekPageUpdatedFinal', $page);
  }

  function testV9HaveCheckboxesWillUnlockWhenChanging() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-two-done-one-open.txt');

    $reports = array();
    $reports[] = createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30');

    $postAction = 'TimeReport/TimeReport.aspx?DateInWeek=2011-01-24';
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8aHR0cDovL2lwd2ViLmludGVsbGlwbGFuLnNlL2t1bmRsb2dvLzQwOTQuanBnOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxKb2hhbiBIZWFuZGVyOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8NDs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjtpPDI+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+O3A8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBMYWdlcmFyYmV0YXJlOzU5ODM+Oz4+O2w8aTwwPjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTw1Pjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwzPjs+PjtsPGk8MD47aTwxPjtpPDI+Oz47bDx0PDtsPGk8Mzc+Oz47bDx0PDtsPGk8MD47aTwxPjtpPDI+Oz47bDx0PEA8ZGlzYWJsZWQ7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgyWm12UG1BNCUyZlowTTRoYmt1NUVpaTM7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgyWm12UG1BNCUyZlowTTRoYmt1NUVpaTM7Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwwPjs+Pjs7Pjt0PEA8XGU7Pjs7Pjs+Pjs+Pjt0PDtsPGk8Mzc+Oz47bDx0PDtsPGk8MD47aTwxPjtpPDI+Oz47bDx0PEA8ZGlzYWJsZWQ7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgzaUgwNVJZSCUyZk9sTTRoYmt1NUVpaTM7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgzaUgwNVJZSCUyZk9sTTRoYmt1NUVpaTM7Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwwPjs+Pjs7Pjt0PEA8XGU7Pjs7Pjs+Pjs+Pjt0PDtsPGk8Mzc+Oz47bDx0PDtsPGk8MD47aTwxPjtpPDI+Oz47bDx0PEA8XGU7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgwNHhWdmtXQ0ZsODg0aGJrdTVFaWkzO0V4cGVuc2UuYXNweD9taGJMUDk2aXFIMDR4VnZrV0NGbDg4NGhia3U1RWlpMzs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcPG9wdGlvblw+W0tsaWNrYSBow6RyXVw8L29wdGlvblw+Oz47Oz47Pj47Pj47Pj47dDw7bDxpPDEzPjtpPDE1PjtpPDE3PjtpPDE5PjtpPDIxPjtpPDIzPjtpPDI3PjtpPDI5PjtpPDMzPjs+O2w8dDx0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47cDxsPGk8MT47aTwyPjtpPDM+O2k8ND47aTw1PjtpPDY+O2k8Nz47PjtsPHA8bcOlLCAyNC8wMSA7MjAxMS0wMS0yND47cDx0aSwgMjUvMDEgOzIwMTEtMDEtMjU+O3A8b24sIDI2LzAxIDsyMDExLTAxLTI2PjtwPHRvLCAyNy8wMSA7MjAxMS0wMS0yNz47cDxmciwgMjgvMDEgOzIwMTEtMDEtMjg+O3A8bMO2LCAyOS8wMSA7MjAxMS0wMS0yOT47cDxzw7YsIDMwLzAxIDsyMDExLTAxLTMwPjs+Pjs+Ozs+O3Q8dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+O3A8bDxpPDE+O2k8Mj47aTwzPjtpPDQ+Oz47bDxwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+O3A8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBMYWdlcmFyYmV0YXJlOzU5ODM+O3A8LS0tOy0xPjtwPFxlO19BQ18+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPGw8VmlzaWJsZTs+O2w8bzxmPjs+PjtsPGk8MT47PjtsPHQ8O2w8aTwxPjs+O2w8dDxwPHA8bDxUZXh0Oz47bDxcZTs+Pjs+Ozs+Oz4+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8VXBwZGF0ZXJhOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxWZWNrYSBLbGFyOz4+Oz47Oz47dDxwPHA8bDxUZXh0O1Zpc2libGU7PjtsPMOEbmRyYSB2ZWNrYTtvPGY+Oz4+Oz47Oz47Pj47Pj47bDxPbGRSb3dzUmVwZWF0ZXI6X2N0bDA6Q2hlY2tib3hEYXlEb25lO09sZFJvd3NSZXBlYXRlcjpfY3RsMTpDaGVja2JveERheURvbmU7T2xkUm93c1JlcGVhdGVyOl9jdGwyOkNoZWNrYm94RGF5RG9uZTtPbGRSb3dzUmVwZWF0ZXI6X2N0bDI6Q2hlY2tCb3hEZWxldGU7RnVsbERheUNoZWNrQm94Oz4+')
        . '&DoPost=true&CustOrdersDropDown=0&OldRowsRepeater%3A_ctl0%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl0%3ABreakHidden=none&OldRowsRepeater%3A_ctl0%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl1%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl1%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl1%3ABreakHidden=none&OldRowsRepeater%3A_ctl1%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl2%3ATextboxTimeFrom=08%3A00&OldRowsRepeater%3A_ctl2%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl2%3ATextboxTimeTo=17%3A00&OldRowsRepeater%3A_ctl2%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl2%3ATextboxBreak=60&OldRowsRepeater%3A_ctl2%3ABreakHidden=none&OldRowsRepeater%3A_ctl2%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl2%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl2%3ATextboxNote=&AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&UpdateButton=Uppdatera';

    $postData = toPostHash($postString);
    $this->server->expects($this->once())
                 ->method('post')
                 ->with($postAction, $postData)
                 ->will($this->returnValue($this->readFile('intellitime-v9-timereport-two-done-one-open.txt')));
    $post = $weekData->buildPost($reports);
    $page = $post->post();
  }

  function testV9HaveCheckboxesWillMarkAllDoneReports() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-one-done-two-open.txt');

    $reports = array();
    $reports[] = createMockReport('mhbLP96iqH2ZmvPmA4%2fZ0M4hbku5Eii3', '2011-01-24', '08:00', '17:00');
    $reports[] = createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30');
    $reports[] = createMockReport('mhbLP96iqH04xVvkWCFl884hbku5Eii3', '2011-01-26', '08:00', '17:00');
    $reports[0]->intellitime_local_changes = 0;
    $reports[2]->intellitime_local_changes = 0;
    $reports[2]->flags = TZFlags::CREATED;

    $postAction = 'TimeReport/TimeReport.aspx?DateInWeek=2011-01-24';
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8aHR0cDovL2lwd2ViLmludGVsbGlwbGFuLnNlL2t1bmRsb2dvLzQwOTQuanBnOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxKb2hhbiBIZWFuZGVyOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8NDs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjtpPDI+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+O3A8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBMYWdlcmFyYmV0YXJlOzU5ODM+Oz4+O2w8aTwwPjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTw1Pjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwzPjs+PjtsPGk8MD47aTwxPjtpPDI+Oz47bDx0PDtsPGk8Mzc+Oz47bDx0PDtsPGk8MD47aTwxPjtpPDI+Oz47bDx0PEA8ZGlzYWJsZWQ7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgyWm12UG1BNCUyZlowTTRoYmt1NUVpaTM7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgyWm12UG1BNCUyZlowTTRoYmt1NUVpaTM7Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwwPjs+Pjs7Pjt0PEA8XGU7Pjs7Pjs+Pjs+Pjt0PDtsPGk8Mzc+Oz47bDx0PDtsPGk8MD47aTwxPjtpPDI+Oz47bDx0PEA8XGU7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgzaUgwNVJZSCUyZk9sTTRoYmt1NUVpaTM7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgzaUgwNVJZSCUyZk9sTTRoYmt1NUVpaTM7Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwwPjs+Pjs7Pjt0PEA8XDxvcHRpb25cPltLbGlja2EgaMOkcl1cPC9vcHRpb25cPjs+Ozs+Oz4+Oz4+O3Q8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxcZTtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDA0eFZ2a1dDRmw4ODRoYmt1NUVpaTM7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgwNHhWdmtXQ0ZsODg0aGJrdTVFaWkzOz47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8MD47Pj47Oz47dDxAPFw8b3B0aW9uXD5bS2xpY2thIGjDpHJdXDwvb3B0aW9uXD47Pjs7Pjs+Pjs+Pjs+Pjt0PDtsPGk8MTM+O2k8MTU+O2k8MTc+O2k8MTk+O2k8MjE+O2k8MjM+O2k8Mjc+O2k8Mjk+O2k8MzM+Oz47bDx0PHQ8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47PjtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0PjtpPDU+O2k8Nj47aTw3Pjs+O2w8cDxtw6UsIDI0LzAxIDsyMDExLTAxLTI0PjtwPHRpLCAyNS8wMSA7MjAxMS0wMS0yNT47cDxvbiwgMjYvMDEgOzIwMTEtMDEtMjY+O3A8dG8sIDI3LzAxIDsyMDExLTAxLTI3PjtwPGZyLCAyOC8wMSA7MjAxMS0wMS0yOD47cDxsw7YsIDI5LzAxIDsyMDExLTAxLTI5PjtwPHPDtiwgMzAvMDEgOzIwMTEtMDEtMzA+Oz4+Oz47Oz47dDx0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47cDxsPGk8MT47aTwyPjtpPDM+O2k8ND47PjtsPHA8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBUcnVja2bDtnJhcmU7NjIwMD47cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIExhZ2VyYXJiZXRhcmU7NTk4Mz47cDwtLS07LTE+O3A8XGU7X0FDXz47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8bDxWaXNpYmxlOz47bDxvPGY+Oz4+O2w8aTwxPjs+O2w8dDw7bDxpPDE+Oz47bDx0PHA8cDxsPFRleHQ7PjtsPFxlOz4+Oz47Oz47Pj47Pj47Pj47dDxwPHA8bDxUZXh0Oz47bDxVcHBkYXRlcmE7Pj47Pjs7Pjt0PHA8cDxsPFRleHQ7PjtsPFZlY2thIEtsYXI7Pj47Pjs7Pjt0PHA8cDxsPFRleHQ7VmlzaWJsZTs+O2w8w4RuZHJhIHZlY2thO288Zj47Pj47Pjs7Pjs+Pjs+PjtsPE9sZFJvd3NSZXBlYXRlcjpfY3RsMDpDaGVja2JveERheURvbmU7T2xkUm93c1JlcGVhdGVyOl9jdGwxOkNoZWNrYm94RGF5RG9uZTtPbGRSb3dzUmVwZWF0ZXI6X2N0bDE6Q2hlY2tCb3hEZWxldGU7T2xkUm93c1JlcGVhdGVyOl9jdGwyOkNoZWNrYm94RGF5RG9uZTtPbGRSb3dzUmVwZWF0ZXI6X2N0bDI6Q2hlY2tCb3hEZWxldGU7RnVsbERheUNoZWNrQm94Oz4+')
        . '&DoPost=true&CustOrdersDropDown=0&OldRowsRepeater%3A_ctl0%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl0%3ABreakHidden=none&OldRowsRepeater%3A_ctl0%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl1%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl1%3ATextboxTimeFrom=08%3A30&OldRowsRepeater%3A_ctl1%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl1%3ATextboxTimeTo=17%3A30&OldRowsRepeater%3A_ctl1%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl1%3ATextboxBreak=60&OldRowsRepeater%3A_ctl1%3ABreakHidden=none&OldRowsRepeater%3A_ctl1%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl1%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl1%3ATextboxNote=&OldRowsRepeater%3A_ctl2%3ATextboxTimeFrom=08%3A00&OldRowsRepeater%3A_ctl2%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl2%3ATextboxTimeTo=17%3A00&OldRowsRepeater%3A_ctl2%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl2%3ATextboxBreak=60&OldRowsRepeater%3A_ctl2%3ABreakHidden=none&OldRowsRepeater%3A_ctl2%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl2%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl2%3ATextboxNote=&AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&UpdateButton=Uppdatera';

    $postData = toPostHash($postString);
    $this->server->expects($this->once())
                 ->method('post')
                 ->with($postAction, $postData)
                 ->will($this->returnValue($this->readFile('intellitime-v9-timereport-one-done-two-open.txt')));
    $post = $weekData->buildPost($reports);
    $page = $post->post();
  }

  function testV9POSTAfterChangeWeek() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-three-open.txt');

    $reports = array();
    $reports[] = createMockReport('mhbLP96iqH2ZmvPmA4%2fZ0M4hbku5Eii3', '2011-01-24', '08:00', '17:00');
    $reports[] = createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:00', '17:00');
    $reports[] = createMockReport('mhbLP96iqH04xVvkWCFl884hbku5Eii3', '2011-01-26', '08:00', '17:00');
    $reports[0]->intellitime_local_changes = 0;
    $reports[1]->intellitime_local_changes = 0;
    $reports[2]->intellitime_local_changes = 1;
    $reports[2]->flags = TZFlags::CREATED;

    $postAction = 'TimeReport/TimeReport.aspx?DateInWeek=2011-01-24';
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8aHR0cDovL2lwd2ViLmludGVsbGlwbGFuLnNlL2t1bmRsb2dvLzQwOTQuanBnOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxKb2hhbiBIZWFuZGVyOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8NDs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjs+O2w8cDwgW1Zpc2EgYWxsYSB1cHBkcmFnXSA7MD47cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIFRydWNrZsO2cmFyZTs2MjAwPjs+PjtsPGk8MD47Pj47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8NT47Pj47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8Mz47Pj47bDxpPDA+O2k8MT47aTwyPjs+O2w8dDw7bDxpPDM3Pjs+O2w8dDw7bDxpPDA+O2k8MT47aTwyPjs+O2w8dDxAPFxlO0V4cGVuc2UuYXNweD9taGJMUDk2aXFIMlptdlBtQTQlMmZaME00aGJrdTVFaWkzO0V4cGVuc2UuYXNweD9taGJMUDk2aXFIMlptdlBtQTQlMmZaME00aGJrdTVFaWkzOz47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8MD47Pj47Oz47dDxAPFw8b3B0aW9uXD5bS2xpY2thIGjDpHJdXDwvb3B0aW9uXD47Pjs7Pjs+Pjs+Pjt0PDtsPGk8Mzc+Oz47bDx0PDtsPGk8MD47aTwxPjtpPDI+Oz47bDx0PEA8XGU7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgzaUgwNVJZSCUyZk9sTTRoYmt1NUVpaTM7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgzaUgwNVJZSCUyZk9sTTRoYmt1NUVpaTM7Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwwPjs+Pjs7Pjt0PEA8XDxvcHRpb25cPltLbGlja2EgaMOkcl1cPC9vcHRpb25cPjs+Ozs+Oz4+Oz4+O3Q8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxcZTtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDA0eFZ2a1dDRmw4ODRoYmt1NUVpaTM7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgwNHhWdmtXQ0ZsODg0aGJrdTVFaWkzOz47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8MD47Pj47Oz47dDxAPFw8b3B0aW9uXD5bS2xpY2thIGjDpHJdXDwvb3B0aW9uXD47Pjs7Pjs+Pjs+Pjs+Pjt0PDtsPGk8MTM+O2k8MTU+O2k8MTc+O2k8MTk+O2k8MjE+O2k8MjM+O2k8Mjc+O2k8Mjk+O2k8MzM+Oz47bDx0PHQ8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47PjtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0PjtpPDU+O2k8Nj47aTw3Pjs+O2w8cDxtw6UsIDI0LzAxIDsyMDExLTAxLTI0PjtwPHRpLCAyNS8wMSA7MjAxMS0wMS0yNT47cDxvbiwgMjYvMDEgOzIwMTEtMDEtMjY+O3A8dG8sIDI3LzAxIDsyMDExLTAxLTI3PjtwPGZyLCAyOC8wMSA7MjAxMS0wMS0yOD47cDxsw7YsIDI5LzAxIDsyMDExLTAxLTI5PjtwPHPDtiwgMzAvMDEgOzIwMTEtMDEtMzA+Oz4+Oz47Oz47dDx0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47cDxsPGk8MT47aTwyPjtpPDM+Oz47bDxwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+O3A8LS0tOy0xPjtwPFxlO19BQ18+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPGw8VmlzaWJsZTs+O2w8bzxmPjs+PjtsPGk8MT47PjtsPHQ8O2w8aTwxPjs+O2w8dDxwPHA8bDxUZXh0Oz47bDxcZTs+Pjs+Ozs+Oz4+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8VXBwZGF0ZXJhOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxWZWNrYSBLbGFyOz4+Oz47Oz47dDxwPHA8bDxUZXh0O1Zpc2libGU7PjtsPMOEbmRyYSB2ZWNrYTtvPGY+Oz4+Oz47Oz47Pj47Pj47bDxPbGRSb3dzUmVwZWF0ZXI6X2N0bDA6Q2hlY2tib3hEYXlEb25lO09sZFJvd3NSZXBlYXRlcjpfY3RsMDpDaGVja0JveERlbGV0ZTtPbGRSb3dzUmVwZWF0ZXI6X2N0bDE6Q2hlY2tib3hEYXlEb25lO09sZFJvd3NSZXBlYXRlcjpfY3RsMTpDaGVja0JveERlbGV0ZTtPbGRSb3dzUmVwZWF0ZXI6X2N0bDI6Q2hlY2tib3hEYXlEb25lO09sZFJvd3NSZXBlYXRlcjpfY3RsMjpDaGVja0JveERlbGV0ZTtGdWxsRGF5Q2hlY2tCb3g7Pj4=')
        . '&DoPost=true&CustOrdersDropDown=0&OldRowsRepeater%3A_ctl0%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl0%3ATextboxTimeFrom=08%3A00&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ATextboxTimeTo=17%3A00&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl0%3ATextboxBreak=60&OldRowsRepeater%3A_ctl0%3ABreakHidden=none&OldRowsRepeater%3A_ctl0%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl0%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl0%3ATextboxNote=&OldRowsRepeater%3A_ctl1%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl1%3ATextboxTimeFrom=08%3A00&OldRowsRepeater%3A_ctl1%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl1%3ATextboxTimeTo=17%3A00&OldRowsRepeater%3A_ctl1%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl1%3ATextboxBreak=60&OldRowsRepeater%3A_ctl1%3ABreakHidden=none&OldRowsRepeater%3A_ctl1%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl1%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl1%3ATextboxNote=&OldRowsRepeater%3A_ctl2%3ATextboxTimeFrom=08%3A00&OldRowsRepeater%3A_ctl2%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl2%3ATextboxTimeTo=17%3A00&OldRowsRepeater%3A_ctl2%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl2%3ATextboxBreak=60&OldRowsRepeater%3A_ctl2%3ABreakHidden=none&OldRowsRepeater%3A_ctl2%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl2%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl2%3ATextboxNote=&AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&UpdateButton=Uppdatera';

    $postData = toPostHash($postString);
    $this->server->expects($this->once())
                 ->method('post')
                 ->with($postAction, $postData)
                 ->will($this->returnValue($this->readFile('intellitime-v9-timereport-three-open.txt')));
    $post = $weekData->buildPost($reports);
    $page = $post->post();
  }

  function testV9DetectNewReport() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-single-open.txt');

    $reports = array();
    $account = (object)array(
      'name' => 'Kalle',
      'uid' => 63,
    );
    $updatedReports = $weekData->updateReports($reports, $account);
    $this->assertEquals(0, count($reports));
    $this->assertEquals(1, count($updatedReports));

    $this->assertEquals(TZIntellitimeReport::STATE_OPEN, $updatedReports[0]->intellitime_last_state);

    $postData = $weekData->buildPost($updatedReports);
    $this->assertNull($postData);
  }

  function testV8DetectNewReport() {
    $weekData = $this->loadHTMLFile('intellitime-v8-timereport-single-open.txt');

    $reports = array();
    $account = (object)array(
      'name' => 'Kalle',
      'uid' => 63,
    );
    $updatedReports = $weekData->updateReports($reports, $account);
    $this->assertEquals(0, count($reports));
    $this->assertEquals(1, count($updatedReports));

    $this->assertEquals(TZIntellitimeReport::STATE_OPEN, $updatedReports[0]->intellitime_last_state);

    $postData = $weekData->buildPost($updatedReports);
    $this->assertNull($postData);
  }

  function testV8TwoDoneOneOpen() {
    $weekData = $this->loadHTMLFile('intellitime-v8-timereport-two-done-one-open.txt');

    $reports = array();
    $account = (object)array(
      'name' => 'Kalle',
      'uid' => 63,
    );
    $updatedReports = $weekData->updateReports($reports, $account);
    $this->assertEquals(3, count($updatedReports));

    $this->assertEquals(TZFlags::REPORTED, $updatedReports[0]->flags);
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[1]->flags);
    $this->assertEquals(TZFlags::CREATED, $updatedReports[2]->flags);
  }

  function testV9OneDoneTwoOpenTwoNew() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-one-done-two-open.txt');

    $reports = array();
    $reports[] = createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30');

    $postData = $weekData->buildPost($reports);

    $postAction = 'TimeReport/TimeReport.aspx?DateInWeek=2011-01-24';
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8aHR0cDovL2lwd2ViLmludGVsbGlwbGFuLnNlL2t1bmRsb2dvLzQwOTQuanBnOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxKb2hhbiBIZWFuZGVyOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8NDs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjtpPDI+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+O3A8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBMYWdlcmFyYmV0YXJlOzU5ODM+Oz4+O2w8aTwwPjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTw1Pjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwzPjs+PjtsPGk8MD47aTwxPjtpPDI+Oz47bDx0PDtsPGk8Mzc+Oz47bDx0PDtsPGk8MD47aTwxPjtpPDI+Oz47bDx0PEA8ZGlzYWJsZWQ7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgyWm12UG1BNCUyZlowTTRoYmt1NUVpaTM7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgyWm12UG1BNCUyZlowTTRoYmt1NUVpaTM7Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwwPjs+Pjs7Pjt0PEA8XGU7Pjs7Pjs+Pjs+Pjt0PDtsPGk8Mzc+Oz47bDx0PDtsPGk8MD47aTwxPjtpPDI+Oz47bDx0PEA8XGU7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgzaUgwNVJZSCUyZk9sTTRoYmt1NUVpaTM7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgzaUgwNVJZSCUyZk9sTTRoYmt1NUVpaTM7Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwwPjs+Pjs7Pjt0PEA8XDxvcHRpb25cPltLbGlja2EgaMOkcl1cPC9vcHRpb25cPjs+Ozs+Oz4+Oz4+O3Q8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxcZTtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDA0eFZ2a1dDRmw4ODRoYmt1NUVpaTM7RXhwZW5zZS5hc3B4P21oYkxQOTZpcUgwNHhWdmtXQ0ZsODg0aGJrdTVFaWkzOz47Oz47dDxwPGw8XyFJdGVtQ291bnQ7PjtsPGk8MD47Pj47Oz47dDxAPFw8b3B0aW9uXD5bS2xpY2thIGjDpHJdXDwvb3B0aW9uXD47Pjs7Pjs+Pjs+Pjs+Pjt0PDtsPGk8MTM+O2k8MTU+O2k8MTc+O2k8MTk+O2k8MjE+O2k8MjM+O2k8Mjc+O2k8Mjk+O2k8MzM+Oz47bDx0PHQ8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47PjtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0PjtpPDU+O2k8Nj47aTw3Pjs+O2w8cDxtw6UsIDI0LzAxIDsyMDExLTAxLTI0PjtwPHRpLCAyNS8wMSA7MjAxMS0wMS0yNT47cDxvbiwgMjYvMDEgOzIwMTEtMDEtMjY+O3A8dG8sIDI3LzAxIDsyMDExLTAxLTI3PjtwPGZyLCAyOC8wMSA7MjAxMS0wMS0yOD47cDxsw7YsIDI5LzAxIDsyMDExLTAxLTI5PjtwPHPDtiwgMzAvMDEgOzIwMTEtMDEtMzA+Oz4+Oz47Oz47dDx0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47cDxsPGk8MT47aTwyPjtpPDM+O2k8ND47PjtsPHA8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBUcnVja2bDtnJhcmU7NjIwMD47cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIExhZ2VyYXJiZXRhcmU7NTk4Mz47cDwtLS07LTE+O3A8XGU7X0FDXz47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8bDxWaXNpYmxlOz47bDxvPGY+Oz4+O2w8aTwxPjs+O2w8dDw7bDxpPDE+Oz47bDx0PHA8cDxsPFRleHQ7PjtsPFxlOz4+Oz47Oz47Pj47Pj47Pj47dDxwPHA8bDxUZXh0Oz47bDxVcHBkYXRlcmE7Pj47Pjs7Pjt0PHA8cDxsPFRleHQ7PjtsPFZlY2thIEtsYXI7Pj47Pjs7Pjt0PHA8cDxsPFRleHQ7VmlzaWJsZTs+O2w8w4RuZHJhIHZlY2thO288Zj47Pj47Pjs7Pjs+Pjs+PjtsPE9sZFJvd3NSZXBlYXRlcjpfY3RsMDpDaGVja2JveERheURvbmU7T2xkUm93c1JlcGVhdGVyOl9jdGwxOkNoZWNrYm94RGF5RG9uZTtPbGRSb3dzUmVwZWF0ZXI6X2N0bDE6Q2hlY2tCb3hEZWxldGU7T2xkUm93c1JlcGVhdGVyOl9jdGwyOkNoZWNrYm94RGF5RG9uZTtPbGRSb3dzUmVwZWF0ZXI6X2N0bDI6Q2hlY2tCb3hEZWxldGU7RnVsbERheUNoZWNrQm94Oz4+')
        . '&DoPost=true&CustOrdersDropDown=0&OldRowsRepeater%3A_ctl0%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl0%3ABreakHidden=none&OldRowsRepeater%3A_ctl0%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl1%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl1%3ATextboxTimeFrom=08%3A30&OldRowsRepeater%3A_ctl1%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl1%3ATextboxTimeTo=17%3A30&OldRowsRepeater%3A_ctl1%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl1%3ATextboxBreak=60&OldRowsRepeater%3A_ctl1%3ABreakHidden=none&OldRowsRepeater%3A_ctl1%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl1%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl1%3ATextboxNote=&OldRowsRepeater%3A_ctl2%3ATextboxTimeFrom=08%3A00&OldRowsRepeater%3A_ctl2%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl2%3ATextboxTimeTo=17%3A00&OldRowsRepeater%3A_ctl2%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl2%3ATextboxBreak=60&OldRowsRepeater%3A_ctl2%3ABreakHidden=none&OldRowsRepeater%3A_ctl2%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl2%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl2%3ATextboxNote=&AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&UpdateButton=Uppdatera';

    $postData = toPostHash($postString);
    $this->server->expects($this->once())
                 ->method('post')
                 ->with($postAction, $postData)
                 ->will($this->returnValue($this->readFile('intellitime-v9-timereport-one-done-two-open.txt')));
    $post = $weekData->buildPost($reports);
    $page = $post->post();
  }

  function testUpdatedV9SingleOpen() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-single-open.txt');

    $reports = array();
    $reports[] = createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30');
    $reports[0]->intellitime_local_changes = 0;

    $account = (object)array(
      'name' => 'Kalle',
      'uid' => 63,
    );

    $updatedReports = $weekData->updateReports($reports, $account);
    $this->assertEquals(1, count($reports));
    $this->assertEquals(1, count($updatedReports));
    $this->assertEquals(TZFlags::CREATED, $updatedReports[0]->flags);

    $begindate = tzbase_make_date($updatedReports[0]->begintime);
    $this->assertEquals('08:00', $begindate->format('H:i'));
    $this->assertEquals('2011-01-25', $begindate->format('Y-m-d'));
    $enddate = tzbase_make_date($updatedReports[0]->endtime);
    $this->assertEquals('17:00', $enddate->format('H:i'));

    $postData = $weekData->buildPost($updatedReports);
    $this->assertNull($postData);
  }

  function testUpdatedV9SingleOpenWithLocalChanges() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-single-open.txt');

    $reports = array();
    $reports[] = createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30');

    $account = (object)array(
      'name' => 'Kalle',
      'uid' => 63,
    );

    $updatedReports = $weekData->updateReports($reports, $account);
    $this->assertEquals(1, count($reports));
    $this->assertEquals(1, count($updatedReports));
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[0]->flags);

    $begindate = tzbase_make_date($updatedReports[0]->begintime);
    $this->assertEquals('08:30', $begindate->format('H:i'));
    $this->assertEquals('2011-01-25', $begindate->format('Y-m-d'));
    $enddate = tzbase_make_date($updatedReports[0]->endtime);
    $this->assertEquals('17:30', $enddate->format('H:i'));

    $postData = $weekData->buildPost($updatedReports);
    $this->assertNotNull($postData);
  }

  function testUpdatedV8SingleOpen() {
    $weekData = $this->loadHTMLFile('intellitime-v8-timereport-single-open.txt');

    $reports = array();
    $reports[] = createMockReport('mhbLP96iqH2OzMt6qJkssA%3d%3d', '2011-01-25', '08:30', '17:30');
    $reports[0]->intellitime_local_changes = 0;

    $account = (object)array(
      'name' => 'Kalle',
      'uid' => 63,
    );

    $updatedReports = $weekData->updateReports($reports, $account);
    $this->assertEquals(1, count($reports));
    $this->assertEquals(1, count($updatedReports));
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[0]->flags);

    $begindate = tzbase_make_date($updatedReports[0]->begintime);
    $this->assertEquals('08:00', $begindate->format('H:i'));
    $this->assertEquals('2011-01-25', $begindate->format('Y-m-d'));
    $enddate = tzbase_make_date($updatedReports[0]->endtime);
    $this->assertEquals('17:00', $enddate->format('H:i'));

    $postData = $weekData->buildPost($updatedReports);
    $this->assertNull($postData);
  }

  function testUpdatedV8SingleRegressToOpen() {
    $weekData = $this->loadHTMLFile('intellitime-v8-timereport-single-open.txt');

    $reports = array();
    $reports[] = createMockReport('mhbLP96iqH2OzMt6qJkssA%3d%3d', '2011-01-25', '08:30', '17:30');
    $reports[0]->intellitime_local_changes = 0;
    $reports[0]->intellitime_last_state = TZIntellitimeReport::STATE_REPORTED;

    $account = (object)array(
      'name' => 'Kalle',
      'uid' => 63,
    );

    $updatedReports = $weekData->updateReports($reports, $account);
    $this->assertEquals(1, count($reports));
    $this->assertEquals(1, count($updatedReports));
    $this->assertEquals(TZFlags::CREATED, $updatedReports[0]->flags);
    $this->assertEquals(TZIntellitimeReport::STATE_OPEN, $updatedReports[0]->intellitime_last_state);

    $begindate = tzbase_make_date($updatedReports[0]->begintime);
    $this->assertEquals('08:00', $begindate->format('H:i'));
    $this->assertEquals('2011-01-25', $begindate->format('Y-m-d'));
    $enddate = tzbase_make_date($updatedReports[0]->endtime);
    $this->assertEquals('17:00', $enddate->format('H:i'));

    $postData = $weekData->buildPost($updatedReports);
    $this->assertNull($postData);
  }

  function testUpdatedV8SingleOpenWithLocalChanges() {
    $weekData = $this->loadHTMLFile('intellitime-v8-timereport-single-open.txt');

    $reports = array();
    $reports[] = createMockReport('mhbLP96iqH2OzMt6qJkssA%3d%3d', '2011-01-25', '08:30', '17:30');

    $account = (object)array(
      'name' => 'Kalle',
      'uid' => 63,
    );

    $updatedReports = $weekData->updateReports($reports, $account);
    $this->assertEquals(1, count($reports));
    $this->assertEquals(1, count($updatedReports));
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[0]->flags);

    $begindate = tzbase_make_date($updatedReports[0]->begintime);
    $this->assertEquals('08:30', $begindate->format('H:i'));
    $this->assertEquals('2011-01-25', $begindate->format('Y-m-d'));
    $enddate = tzbase_make_date($updatedReports[0]->endtime);
    $this->assertEquals('17:30', $enddate->format('H:i'));

    $postData = $weekData->buildPost($updatedReports);
    $this->assertNotNull($postData);
  }

  function testUpdatedV9FromTwoLocked() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-two-locked-one-done.txt');

    $reports = array();
    $reports[] = createMockReport('mhbLP96iqH1henDWvmtJBc4hbku5Eii3', '2010-09-08', '08:30', '17:30');
    $reports[] = createMockReport('mhbLP96iqH0mntj1zVlC484hbku5Eii3', '2010-09-09', '08:30', '17:30');
    $reports[] = createMockReport('mhbLP96iqH2II35gn6Dx4M4hbku5Eii3', '2010-09-11', '08:30', '17:30');
    $reports[1]->intellitime_local_changes = 0;

    $account = (object)array(
      'name' => 'Kalle',
      'uid' => 63,
    );

    $updatedReports = $weekData->updateReports($reports, $account);
    $this->assertEquals(3, count($reports));
    $this->assertEquals(3, count($updatedReports));

    // Check first row, LOCKED 07:00 -> 16:00 -60
    $this->assertEquals(TZFlags::LOCKED, $updatedReports[0]->flags);
    $this->assertEquals(TZIntellitimeReport::STATE_LOCKED, $updatedReports[0]->intellitime_last_state);
    $this->assertReportTimes('2010-09-08', '07:00', '16:00', 60, $updatedReports[0]);

    // Second row, LOCKED 07:05 -> 16:00 -60
    $this->assertEquals(TZFlags::LOCKED, $updatedReports[1]->flags);
    $this->assertEquals(TZIntellitimeReport::STATE_LOCKED, $updatedReports[1]->intellitime_last_state);
    $this->assertReportTimes('2010-09-09', '07:05', '16:00', 60, $updatedReports[1]);

    // Third row, REPORTED 08:30 -> 17:30 -60
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[2]->flags);
    /* This report has local changes and since it is a mock and has not yet been synced its last_state is null */
    $this->assertFalse(isset($updatedReports[2]->intellitime_last_state));
    $this->assertReportTimes('2010-09-11', '08:30', '17:30', 60, $updatedReports[2]);

    $postData = $weekData->buildPost($updatedReports);
    $this->assertNotNull($postData);
  }

  function testReportsHaveJobIDReferences() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-administrator-absence-all-marked-done.txt');

    $reports = array(
      createMockReport('mhbLP96iqH3ZMQKVB6I6Qc4hbku5Eii3', '2011-01-10', '08:00', '17:00', 60),
      createMockReport('mhbLP96iqH0993oUoX8OKM4hbku5Eii3', '2011-01-11', '08:15', '17:30', 60),
      createMockReport('mhbLP96iqH2xpPwR07B5Nc4hbku5Eii3', '2011-01-12', '08:00', '17:10', 60),
      createMockReport('mhbLP96iqH1AhkChU548NM4hbku5Eii3', '2011-01-13', '08:00', '17:30', 30),
    );

    $updatedReports = $weekData->updateReports($reports, $this->account, TRUE);

    $this->assertEquals('5983', $updatedReports[0]->intellitime_jobid);
    $this->assertEquals('PLACEHOLDER_ID_' . md5('Tjänstledig <6 dgr'), $updatedReports[1]->intellitime_jobid);
    $this->assertEquals('5983', $updatedReports[2]->intellitime_jobid);
    $this->assertEquals('6200', $updatedReports[3]->intellitime_jobid);
  }

  public function testListAssignmentsHaveID() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-shortened-jobtitles.txt');
    $assignments = $weekData->getAssignments();

    $this->assertEquals(33, count($assignments));

    $this->assertEquals('Axis Commun, Lagerarbeta, Q6032, spec', $assignments[1]->title);
    $this->assertEquals('5093', $assignments[1]->id);

    $this->assertEquals('Axis Communicatio, Lagerarbeta, P5534', $assignments[2]->title);
    $this->assertEquals('6056', $assignments[2]->id);

    $this->assertEquals('Axis Communic, Lagerarbeta, "Heating"', $assignments[3]->title);
    $this->assertEquals('5194', $assignments[3]->id);

    $this->assertEquals('Sjukfrånvaro kollektivare månadsanställd', $assignments[18]->title);
    $this->assertEquals('_AC_Sjuk arb (mån.lön)', $assignments[18]->id);
  }

  public function testListAssignmentsNoID() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-one-absence-two-done-one-open.txt');
    $assignments = $weekData->getAssignments();

    $this->assertEquals(3, count($assignments));

    $this->assertEquals('Testföretaget Effekt, Lagerarbetare', $assignments[0]->title);
    $this->assertEquals('5983', $assignments[0]->id);

    $this->assertEquals('Testföretaget Effekt, Truckförare', $assignments[1]->title);
    $this->assertEquals('6200', $assignments[1]->id);

    $this->assertEquals('Tjänstledig <6 dgr', $assignments[2]->title);
    $this->assertEquals('PLACEHOLDER_ID_' . md5('Tjänstledig <6 dgr'), $assignments[2]->id);
  }

  public function testV8ParseUnfinishedWeeks() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-one-absence-two-done-one-open.txt');
    $unfinishedWeeks = $weekData->getUnfinishedWeeks();
    $this->assertEquals(4, count($unfinishedWeeks));
    $this->assertEquals('2011W02', $unfinishedWeeks[0]->format('o\WW'));
    $this->assertEquals('2011W03', $unfinishedWeeks[1]->format('o\WW'));
    $this->assertEquals('2011W04', $unfinishedWeeks[2]->format('o\WW'));
    $this->assertEquals('2011W05', $unfinishedWeeks[3]->format('o\WW'));
  }

  public function testUpdateLastStatesWithDeletedReportPassesReportUnchanged() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-one-absence-two-done-one-open.txt');
    $tzreports = array(
      createMockReport("klasdanmlksd", "2011-01-10", "08:00", "12:00"),
    );

    $post = $weekData->buildPost($tzreports);

    $updatedReports = $weekData->postProcessPost($post, $tzreports, $this->account);
    $this->assertEquals($tzreports, $updatedReports);
  }

  function testDuplicatesKeepsLocalChanges() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-single-open.txt');

    $reports = array(
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30', 30, 0),
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30', 62, 1),
    );

    $account = (object)array(
      'name' => 'Kalle',
      'uid' => 63,
    );

    $updatedReports = $weekData->updateReports($reports, $account);
    $this->assertEquals(2, count($reports));
    $this->assertEquals(2, count($updatedReports));
    $this->assertEquals(TZFlags::REPORTED, $updatedReports[0]->flags);
    $this->assertTrue(FALSE !== strpos($updatedReports[0]->intellitime_id, 'mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3'));
    $this->assertEquals(62*60, $updatedReports[0]->breakduration);
    $this->assertEquals(TZFlags::DELETED, $updatedReports[1]->flags);
    $this->assertEquals(0, $updatedReports[1]->intellitime_local_changes);

    $postData = $weekData->buildPost($updatedReports);
    $this->assertNotNull($postData);
  }

  function testDuplicatesWithoutChangesKeepsFirst() {
    $weekData = $this->loadHTMLFile('intellitime-v9-timereport-single-open.txt');

    $reports = array(
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30', 60, 0),
      createMockReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30', 61, 0),
    );

    $account = (object)array(
      'name' => 'Kalle',
      'uid' => 63,
    );

    $updatedReports = $weekData->updateReports($reports, $account);
    $this->assertEquals(2, count($reports));
    $this->assertEquals(2, count($updatedReports));
    $this->assertEquals(TZFlags::CREATED, $updatedReports[0]->flags);
    $this->assertTrue(FALSE !== strpos($updatedReports[0]->intellitime_id, 'mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3'));
    $this->assertEquals(60*60, $updatedReports[0]->breakduration);
    $this->assertEquals(TZFlags::DELETED, $updatedReports[1]->flags);
    $this->assertEquals(0, $updatedReports[1]->intellitime_local_changes);
  }

  public function testThrowOnErrorPage() {
    try {
      $weekData = $this->loadHTMLFile('WeekData_ThrowOnErrorPage.txt');
      $this->fail('Expected exception');
    } catch(TZIntellitimeErrorPageException $e) {
      $this->assertNotNull($e);
    }
  }

  private function assertReportTimes($date, $starttime, $endtime, $breakminutes, $tzreport) {
    $begindate = tzbase_make_date($tzreport->begintime);
    $this->assertEquals($starttime, $begindate->format('H:i'));
    $this->assertEquals($date, $begindate->format('Y-m-d'));
    $enddate = tzbase_make_date($tzreport->endtime);
    $this->assertEquals($endtime, $enddate->format('H:i'));
    $this->assertEquals($breakminutes*60, $tzreport->breakduration);
  }

  private function assertPostDataEqual($expected, $actual) {
    $expectedArray = explode('&', $expected);
    sort($expectedArray);
    ksort($actual);
    $actualString = http_build_query($actual, '', "\n");
    $expectedString = implode("\n", $expectedArray);
    $this->assertEquals($expectedString, $actualString);
  }

  private function loadHTMLFile($filename) {
    $page = new IntellitimeWeekPage($this->readFile($filename), $this->server);
    return new TZIntellitimeWeekData($page);
  }

  private function readFile($filename) {
    $full_name = dirname(__FILE__) . "/../tests/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    return $contents;
  }
}
