<?php

class IntellitimeWeekUnlockPostTest extends PHPUnit_Framework_TestCase {
  function setUp() {
    $this->server = $this->getMock('IntellitimeServer');
  }

  function testV9ChangingSingleDoneReportResultsInChangeWeekPost() {
    $page = $this->loadHTMLFile('intellitime-v9-timereport-single-done.txt');

    $reports = array(createMockITReport('mhbLP96iqH3iH05RYH%2fOlM4hbku5Eii3', '2011-01-25', '08:30', '17:30'));

    $postAction = 'TimeReport/TimeReport.aspx?DateInWeek=2011-01-28';
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8OT47aTwxMD47aTwxMT47aTwxMj47aTwxMz47PjtsPHQ8O2w8aTwwPjs+O2w8dDw7bDxpPDE+O2k8Mz47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8aHR0cDovL2lwd2ViLmludGVsbGlwbGFuLnNlL2t1bmRsb2dvLzQwOTQuanBnOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxKb2hhbiBIZWFuZGVyOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8NDs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjtpPDI+Oz47bDxwPCBbVmlzYSBhbGxhIHVwcGRyYWddIDswPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+O3A8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBMYWdlcmFyYmV0YXJlOzU5ODM+Oz4+O2w8aTwwPjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTw0Pjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwxPjs+PjtsPGk8MD47PjtsPHQ8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxkaXNhYmxlZDtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMztFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDNpSDA1UllIJTJmT2xNNGhia3U1RWlpMzs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcZTs+Ozs+Oz4+Oz4+Oz4+O3Q8O2w8aTwxMz47aTwxNT47aTwxNz47aTwxOT47aTwyMT47aTwyMz47aTwyNz47aTwyOT47aTwzMz47PjtsPHQ8dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+O3A8bDxpPDE+O2k8Mj47aTwzPjtpPDQ+O2k8NT47aTw2PjtpPDc+Oz47bDxwPG3DpSwgMjQvMDEgOzIwMTEtMDEtMjQ+O3A8dGksIDI1LzAxIDsyMDExLTAxLTI1PjtwPG9uLCAyNi8wMSA7MjAxMS0wMS0yNj47cDx0bywgMjcvMDEgOzIwMTEtMDEtMjc+O3A8ZnIsIDI4LzAxIDsyMDExLTAxLTI4PjtwPGzDtiwgMjkvMDEgOzIwMTEtMDEtMjk+O3A8c8O2LCAzMC8wMSA7MjAxMS0wMS0zMD47Pj47Pjs7Pjt0PHQ8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47PjtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0Pjs+O2w8cDxUZXN0ZsO2cmV0YWdldCBFZmZla3QsIFRydWNrZsO2cmFyZTs2MjAwPjtwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgTGFnZXJhcmJldGFyZTs1OTgzPjtwPC0tLTstMT47cDxcZTtfQUNfPjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxsPFZpc2libGU7PjtsPG88Zj47Pj47bDxpPDE+Oz47bDx0PDtsPGk8MT47PjtsPHQ8cDxwPGw8VGV4dDs+O2w8XGU7Pj47Pjs7Pjs+Pjs+Pjs+Pjt0PHA8cDxsPFRleHQ7PjtsPFVwcGRhdGVyYTs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDtWaXNpYmxlOz47bDxWZWNrYSBLbGFyO288Zj47Pj47Pjs7Pjt0PHA8cDxsPFRleHQ7VmlzaWJsZTs+O2w8w4RuZHJhIHZlY2thO288dD47Pj47Pjs7Pjs+Pjs+PjtsPE9sZFJvd3NSZXBlYXRlcjpfY3RsMDpDaGVja2JveERheURvbmU7RnVsbERheUNoZWNrQm94Oz4+')
        . '&DoPost=true&CustOrdersDropDown=0&OldRowsRepeater%3A_ctl0%3ACheckboxDayDone=on&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&OldRowsRepeater%3A_ctl0%3ABreakHidden=none&OldRowsRepeater%3A_ctl0%3AOverTimeHidden=none&AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&ChangeButton=%C3%84ndra+vecka';
    $postData = toPostHash($postString);
    $this->server->expects($this->once())
                 ->method('post')
                 ->with($postAction, $postData)
                 ->will($this->returnValue($this->readFile('intellitime-v9-timereport-single-done.txt')));
    $post = $page->getUnlockPost($reports, FALSE);
    $post->post();
  }

  function testV8ChangingSingleDoneReportResultsInChangeWeekPost() {
    $page = $this->loadHTMLFile('intellitime-v8-timereport-single-done.txt');

    $reports = array(createMockITReport('mhbLP96iqH2OzMt6qJkssA%3d%3d', '2011-01-25', '08:30', '17:30'));

    $postAction = 'TimeReport/TimeReport.aspx?DateInWeek=2011-01-28';
    $postString = '__EVENTTARGET=&__EVENTARGUMENT=&__VIEWSTATE=' .
      rawurlencode('dDwtMTg1NjE4NzIxO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8Nz47aTw4PjtpPDk+O2k8MTA+O2k8MTE+O2k8MTI+O2k8MTM+Oz47bDx0PDtsPGk8MD47PjtsPHQ8O2w8aTwxPjtpPDM+O2k8Nz47PjtsPHQ8cDxwPGw8SW1hZ2VVcmw7PjtsPH4vQ3VzdG9tZXJzL2ludGVsbGlwbGFuX2xvZ28uZ2lmOz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxKb25hcyAgU3VuZGluOz4+Oz47Oz47dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9JbWFnZXMvSW1nX0ludGVsbGlwbGFuTG9nb1doaXRlLmdpZjs+Pjs+Ozs+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8NDs+Pjs+Ozs+O3Q8dDw7cDxsPGk8MD47aTwxPjs+O2w8cDwgW1Zpc2EgYWxsYSB1cHBkcmFnXSA7MD47cDxXYWx0ZXIgJiBDTyAsIEtvY2ssIFNwZWNpYWx1cHBkcmFnIGVmOzIwMj47Pj47bDxpPDA+Oz4+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDE+Oz4+Ozs+O3Q8cDxsPFZpc2libGU7PjtsPG88dD47Pj47Oz47dDxwPGw8VmlzaWJsZTs+O2w8bzx0Pjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwxPjs+PjtsPGk8MD47PjtsPHQ8O2w8aTwzNz47PjtsPHQ8O2w8aTwwPjtpPDE+O2k8Mj47PjtsPHQ8QDxkaXNhYmxlZDtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDJPek10NnFKa3NzQSUzZCUzZDtFeHBlbnNlLmFzcHg/bWhiTFA5NmlxSDJPek10NnFKa3NzQSUzZCUzZDs+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDA+Oz4+Ozs+O3Q8QDxcPG9wdGlvblw+W0tsaWNrYSBow6RyXVw8L29wdGlvblw+Oz47Oz47Pj47Pj47Pj47dDw7bDxpPDEzPjtpPDE1PjtpPDE3PjtpPDE5PjtpPDIxPjtpPDIzPjtpPDI3PjtpPDI5PjtpPDMzPjs+O2w8dDx0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47cDxsPGk8MT47aTwyPjtpPDM+O2k8ND47aTw1PjtpPDY+O2k8Nz47PjtsPHA8bcOlLCAyNC8wMSA7MjAxMS0wMS0yND47cDx0aSwgMjUvMDEgOzIwMTEtMDEtMjU+O3A8b24sIDI2LzAxIDsyMDExLTAxLTI2PjtwPHRvLCAyNy8wMSA7MjAxMS0wMS0yNz47cDxmciwgMjgvMDEgOzIwMTEtMDEtMjg+O3A8bMO2LCAyOS8wMSA7MjAxMS0wMS0yOT47cDxzw7YsIDMwLzAxIDsyMDExLTAxLTMwPjs+Pjs+Ozs+O3Q8dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+O3A8bDxpPDE+O2k8Mj47aTwzPjs+O2w8cDxXYWx0ZXIgJiBDTyAsIEtvY2ssIFNwZWNpYWx1cHBkcmFnIGVmOzIwMj47cDwtLS07LTE+O3A8S29uc3VsdGVucyBsZWRpZ2EgZGFnIGVubC4gw7Z2ZXJlbnNrb21tZXQgc2NoZW1hLjtfQUNfTEVESUcgRU5MLiBTQ0hFTUE+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPGw8VmlzaWJsZTs+O2w8bzxmPjs+PjtsPGk8MT47PjtsPHQ8O2w8aTwxPjs+O2w8dDxwPHA8bDxUZXh0Oz47bDxcZTs+Pjs+Ozs+Oz4+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8VXBwZGF0ZXJhOz4+Oz47Oz47dDxwPHA8bDxUZXh0O1Zpc2libGU7PjtsPFZlY2thIEtsYXI7bzxmPjs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDtWaXNpYmxlOz47bDzDhG5kcmEgdmVja2E7bzx0Pjs+Pjs+Ozs+Oz4+Oz4+O2w8RnVsbERheUNoZWNrQm94Oz4+')
        . '&DoPost=true&CustOrdersDropDown=0&OldRowsRepeater%3A_ctl0%3ADateFromHidden=08%3A00&OldRowsRepeater%3A_ctl0%3ADateToHidden=17%3A00&AddDateDropDown=&AddRowDropDown=&AddTimeFromTextBox=&AddTimeToTextBox=&AddBreakTextBox=&AddExplicitOvertimeTextBox=&AddNoteTextBox=&ChangeButton=%C3%84ndra+vecka';
    $postData = toPostHash($postString);
    $this->server->expects($this->once())
                 ->method('post')
                 ->with($postAction, $postData)
                 ->will($this->returnValue($this->readFile('intellitime-v8-timereport-single-done.txt')));
    $post = $page->getUnlockPost($reports, TRUE);
    $this->assertTrue($post->getUnlockImmutable());
    $post->post();
  }

  private function loadHTMLFile($filename) {
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