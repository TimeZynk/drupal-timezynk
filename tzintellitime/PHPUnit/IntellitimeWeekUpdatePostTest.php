<?php

class IntellitimeWeekUpdatePostTest extends PHPUnit_Framework_TestCase {

  public function testBuildInputWithComments() {
    $page = $this->loadHTMLFile('intellitime-timereport-with-comments.html');
    $reports = $page->getReports();
    $itreport = $reports[0];

    $itreport->id = 'mhbLP96iqH3xps0bh7%2fZv84hbku5Eii3';
    $itreport->begin = '09:31';
    $itreport->end = '17:43';
    $itreport->comment = 'I did all I could!';

    $expected_action = 'TimeReport/TimeReport.aspx?DateInWeek=2010-11-08';
    $expected_data = $page->getForm()->getFormValues();
    unset($expected_data['DoneButton']);
    $expected_data['OldRowsRepeater:_ctl0:TextboxNote'] = $itreport->comment;
    $expected_data['OldRowsRepeater:_ctl0:TextboxTimeFrom'] = $itreport->begin;
    $expected_data['OldRowsRepeater:_ctl0:TextboxTimeTo'] = $itreport->end;
    $expected_data['OldRowsRepeater:_ctl0:TextboxBreak'] = $itreport->break_duration_minutes;

    $this->server->expects($this->once())
                 ->method('post')
                 ->with($expected_action, $expected_data)
                 ->will($this->returnValue($this->readFile('intellitime-timereport-with-comments.html')));

    $post = $page->getUpdatePost(array($itreport), FALSE, FALSE);
    $new_page = $post->post();
  }

  public function testBuildInputWithoutComments() {
    $page = $this->loadHTMLFile('intellitime-timereport-page.html');
    $reports = $page->getReports();
    $itreport = $reports[4];

    $itreport->id = 'mhbLP96iqH2aeWvSjjSDX84hbku5Eii3';
    $itreport->begin = '09:31';
    $itreport->end = '17:43';
    $itreport->comment = 'I did all I could!';

    $form = $page->getForm();
    $expected_action = 'TimeReport/' . $form->getAction();
    $expected_data = $form->getFormValues();
    unset($expected_data['OldRowsRepeater:_ctl4:TextboxNote']);
    unset($expected_data['DoneButton']);
    $expected_data['OldRowsRepeater:_ctl4:TextboxTimeFrom'] = $itreport->begin;
    $expected_data['OldRowsRepeater:_ctl4:TextboxTimeTo'] = $itreport->end;
    $expected_data['OldRowsRepeater:_ctl4:TextboxBreak'] = $itreport->break_duration_minutes;

    $this->server->expects($this->once())
                 ->method('post')
                 ->with($expected_action, $expected_data)
                 ->will($this->returnValue($this->readFile('intellitime-timereport-with-comments.html')));

    $post = $page->getUpdatePost(array($itreport), FALSE, FALSE);
    $post->post();
  }

  public function testBuildSimplePost() {
    $page = $this->loadHTMLFile('intellitime-v8-timereport-w1102-not-done.txt');
    $reports = $page->getReports();
    $post = $page->getUpdatePost($reports, FALSE, FALSE);
    $this->assertInstanceOf('IntellitimeWeekUpdatePost', $post);
  }

  public function testThrowsWhenPostingUnknownReport() {
    $page = $this->loadHTMLFile('intellitime-v8-timereport-w1102-not-done.txt');
    $reports = $page->getReports();
    $reports[0]->id = 'NONEXISTANT';
    $post = $page->getUpdatePost($reports, FALSE, FALSE);
    try {
      $post->post();
      $this->fail('Should have thrown exception');
    } catch(TZIntellitimeReportRowNotFound $e) {
      $this->assertNotNull($e);
    }
  }

  function testV9CloseWeekWithWeekDoneForSingleReport() {
    $page = $this->loadHTMLFile('intellitime-v9-timereport-single-open.txt');

    $reports = array(createMockITReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30'));

    $post = $page->getUpdatePost($reports, TRUE, FALSE);
    $this->assertInstanceOf('IntellitimeWeekUpdatePost', $post);
    $this->assertTrue($post->getAllReportsDone());

    $postAction = 'TimeReport/TimeReport.aspx?DateInWeek=2011-01-28';
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8aHR0cDovL2lwd2ViLmludGVsbGlwbGFuLnNlL2t1bmRsb2dvLzQwOTQuanBnOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxKb2hhbiBIZWFuZGVyOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8NDs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjtpPDI+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+O3A8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBMYWdlcmFyYmV0YXJlOzU5ODM+Oz4+O2w8aTwwPjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTw1Pjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwxPjs+PjtsPGk8MD47PjtsPHQ8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxcZTtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMztFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMzs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcPG9wdGlvblw+W0tsaWNrYSBow6RyXVw8L29wdGlvblw+Oz47Oz47Pj47Pj47Pj47dDw7bDxpPDEzPjtpPDE1Pjs+O2w8dDx0PDtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0PjtpPDU+O2k8Nj47aTw3Pjs+O2w8cDxtw6UsIDI0LzAxIDsyMDExLTAxLTI0PjtwPHRpLCAyNS8wMSA7MjAxMS0wMS0yNT47cDxvbiwgMjYvMDEgOzIwMTEtMDEtMjY+O3A8dG8sIDI3LzAxIDsyMDExLTAxLTI3PjtwPGZyLCAyOC8wMSA7MjAxMS0wMS0yOD47cDxsw7YsIDI5LzAxIDsyMDExLTAxLTI5PjtwPHPDtiwgMzAvMDEgOzIwMTEtMDEtMzA+Oz4+Oz47Oz47dDx0PDtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0Pjs+O2w8cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIFRydWNrZsO2cmFyZTs2MjAwPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgTGFnZXJhcmJldGFyZTs1OTgzPjtwPC0tLTstMT47cDxcZTtfQUNfPjs+Pjs+Ozs+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8VXBwZGF0ZXJhOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxWZWNrYSBLbGFyOz4+Oz47Oz47dDxwPHA8bDxUZXh0O1Zpc2libGU7PjtsPMOEbmRyYSB2ZWNrYTtvPGY+Oz4+Oz47Oz47Pj47Pj47bDxPbGRSb3dzUmVwZWF0ZXI6X2N0bDA6Q2hlY2tib3hEYXlEb25lO09sZFJvd3NSZXBlYXRlcjpfY3RsMDpDaGVja0JveERlbGV0ZTtGdWxsRGF5Q2hlY2tCb3g7Pj4=') . '&DoPost=true&CustOrdersDropDown=0&OldRowsRepeater%3A_ctl0%3ATextboxTimeFrom=08%3A30&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ATextboxTimeTo=17%3A30&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl0%3ATextboxBreak=60&OldRowsRepeater%3A_ctl0%3ABreakHidden=none&OldRowsRepeater%3A_ctl0%3ATextboxExplicitOvertime=0&OldRowsRepeater%3A_ctl0%3AOverTimeHidden=none&OldRowsRepeater%3A_ctl0%3ATextboxNote=&OldRowsRepeater%3A_ctl0%3ACheckboxDayDone=on&AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&DoneButton=Vecka+Klar';
    $postData = toPostHash($postString);
    $this->server->expects($this->once())
                 ->method('post')
                 ->with($postAction, $postData)
                 ->will($this->returnValue($this->readFile('intellitime-v9-timereport-single-open.txt')));

    $post->post();
  }

  function testV8CloseWeekWithWeekDoneForSingleReport() {
    $page = $this->loadHTMLFile('intellitime-v8-timereport-single-open.txt');

    $reports = array(createMockITReport('mhbLP96iqH2OzMt6qJkssA%3d%3d', '2011-01-25', '08:30', '17:30'));

    $post = $page->getUpdatePost($reports, TRUE, FALSE);
    $this->assertInstanceOf('IntellitimeWeekUpdatePost', $post);
    $this->assertTrue($post->getAllReportsDone());

    $postAction = 'TimeReport/TimeReport.aspx?DateInWeek=2011-01-28';
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMTg1NjE4NzIxO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8Nz47aTw4PjtpPDk+O2k8MTA+O2k8MTE+O2k8MTI+O2k8MTM+Oz47bDx0PDtsPGk8MD47PjtsPHQ8O2w8aTwxPjtpPDM+O2k8Nz47PjtsPHQ8cDxwPGw8SW1hZ2VVcmw7PjtsPH4vQ3VzdG9tZXJzL2ludGVsbGlwbGFuX2xvZ28uZ2lmOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxKb25hcyAgU3VuZGluOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8NDs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjs+O2w8cDwgW1Zpc2EgYWxsYSB1cHBkcmFnXSA7MD47cDxXYWx0ZXIgJiBDTyAsIEtvY2ssIFNwZWNpYWx1cHBkcmFnIGVmOzIwMj47Pj47bDxpPDA+Oz4+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDI+Oz4+Ozs+O3Q8cDxsPFZpc2libGU7PjtsPG88dD47Pj47Oz47dDxwPGw8VmlzaWJsZTs+O2w8bzx0Pjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwxPjs+PjtsPGk8MD47PjtsPHQ8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxcZTtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDJPek10NnFKa3NzQSUzZCUzZDtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDJPek10NnFKa3NzQSUzZCUzZDs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcPG9wdGlvblw+W0tsaWNrYSBow6RyXVw8L29wdGlvblw+Oz47Oz47Pj47Pj47Pj47dDw7bDxpPDEzPjtpPDE1PjtpPDE3PjtpPDE5PjtpPDIxPjtpPDIzPjtpPDI3PjtpPDI5PjtpPDMzPjs+O2w8dDx0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47cDxsPGk8MT47aTwyPjtpPDM+O2k8ND47aTw1PjtpPDY+O2k8Nz47PjtsPHA8bcOlLCAyNC8wMSA7MjAxMS0wMS0yND47cDx0aSwgMjUvMDEgOzIwMTEtMDEtMjU+O3A8b24sIDI2LzAxIDsyMDExLTAxLTI2PjtwPHRvLCAyNy8wMSA7MjAxMS0wMS0yNz47cDxmciwgMjgvMDEgOzIwMTEtMDEtMjg+O3A8bMO2LCAyOS8wMSA7MjAxMS0wMS0yOT47cDxzw7YsIDMwLzAxIDsyMDExLTAxLTMwPjs+Pjs+Ozs+O3Q8dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+O3A8bDxpPDE+O2k8Mj47aTwzPjs+O2w8cDxXYWx0ZXIgJiBDTyAsIEtvY2ssIFNwZWNpYWx1cHBkcmFnIGVmOzIwMj47cDwtLS07LTE+O3A8S29uc3VsdGVucyBsZWRpZ2EgZGFnIGVubC4gw7Z2ZXJlbnNrb21tZXQgc2NoZW1hLjtfQUNfTEVESUcgRU5MLiBTQ0hFTUE+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPGw8VmlzaWJsZTs+O2w8bzxmPjs+PjtsPGk8MT47PjtsPHQ8O2w8aTwxPjs+O2w8dDxwPHA8bDxUZXh0Oz47bDxcZTs+Pjs+Ozs+Oz4+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8VXBwZGF0ZXJhOz4+Oz47Oz47dDxwPHA8bDxUZXh0O1Zpc2libGU7PjtsPFZlY2thIEtsYXI7bzx0Pjs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDtWaXNpYmxlOz47bDzDhG5kcmEgdmVja2E7bzxmPjs+Pjs+Ozs+Oz4+Oz4+O2w8T2xkUm93c1JlcGVhdGVyOl9jdGwwOkNoZWNrQm94RGVsZXRlO0Z1bGxEYXlDaGVja0JveDs+Pg==')
        . '&DoPost=true&CustOrdersDropDown=0&OldRowsRepeater%3A_ctl0%3ATextboxTimeFrom=08%3A30&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ATextboxTimeTo=17%3A30&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl0%3ATextboxBreak=60&OldRowsRepeater%3A_ctl0%3ATextboxNote=&AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&DoneButton=Vecka+Klar';
    $postData = toPostHash($postString);
    $this->server->expects($this->once())
                 ->method('post')
                 ->with($postAction, $postData)
                 ->will($this->returnValue($this->readFile('intellitime-v8-timereport-single-open.txt')));

    $post->post();
  }

  private function loadHTMLFile($filename) {
    $this->server = $this->getMock('IntellitimeServer');
    return new IntellitimeWeekPage($this->readFile($filename), $this->server);
  }

  private function readFile($filename) {
    $full_name = dirname(__FILE__) . "/../tests/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    return $contents;
  }
}