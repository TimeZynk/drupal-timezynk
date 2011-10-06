<?php

class IntellitimeWeekInsertPostTest extends PHPUnit_Framework_TestCase {
  public function testBuildNewReportPostPreservesOtherFields() {
    $page = $this->loadHTMLFile('intellitime-timereport-with-comments.html');

    $nid = 199282;
    $itreport = new TZIntellitimeReport();
    $itreport->state = TZIntellitimeReport::STATE_REPORTED;
    $itreport->year = 2010;
    $itreport->month = 9;
    $itreport->day = 3;
    $itreport->jobid = '5983';
    $itreport->begin = '09:31';
    $itreport->end = '17:43';
    $itreport->break_duration_minutes = 30;
    $itreport->comment = 'I did all I could!';

    $form = $page->getForm();
    $expected_action = 'TimeReport/' . $form->getAction();
    $expected_data = $form->getFormValues();
    unset($expected_data['DoneButton']);
    $expected_data['AddDateDropDown'] = '2010-09-03';
    $expected_data['AddRowDropDown'] = $itreport->jobid;
    $expected_data['AddTimeFromTextBox'] = $itreport->begin;
    $expected_data['AddTimeToTextBox'] = $itreport->end;
    $expected_data['AddNoteTextBox'] = $itreport->comment;
    $expected_data['AddBreakTextBox'] = $itreport->break_duration_minutes;


    $this->server->expects($this->once())
                 ->method('post')
                 ->with($expected_action, $expected_data)
                 ->will($this->returnValue($this->readFile('intellitime-timereport-with-comments.html')));

    $post = $page->getInsertPost($nid, $itreport);
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