<?php

class IntellitimeAvailabilityUpdatePostTest extends PHPUnit_Framework_TestCase {
  function setUp() {
    $this->server = $this->getMock('IntellitimeServer');

    $this->form = $this->getMock('IntellitimeForm');
    $this->formAction = "Availability.aspx?MId=Availability";
    $this->expectedAction = "Availability/Availability.aspx?MId=Availability";
    $this->form->expects($this->once())
      ->method('getAction')
      ->will($this->returnValue($this->formAction));

    $this->expectedFormId = 'my_ctl:444_test:_prefix';
    $this->expectedAvailabilities = array(
      new IntellitimeAvailability(date_make_date('2011-07-14'), $this->expectedFormId),
    );
    $this->expectedAvailabilities[0]->setDay(TRUE);
    $this->expectedAvailabilities[0]->setEvening(TRUE);
    $this->expectedAvailabilities[0]->setNight(TRUE);
    $this->post = new IntellitimeAvailabilityUpdatePost($this->server, $this->form, $this->expectedAvailabilities);
  }

  public function testCreatesFinalPage() {
    $this->server->expects($this->once())
      ->method('post')
      ->will($this->returnValue($this->readfile('availability-1-day.txt')));
    $page = $this->post->post();
    $this->assertInstanceOf('IntellitimeAvailabilityFinalPage', $page);
  }

  public function testCreatesCorrectPostForSingleUpdate() {
    $originalFormValues = array(
      'keep_field' => 'magic value',
    );
    $expectedAvailability = new IntellitimeAvailability(date_make_date('2011-07-14'));
    $expectedPostData = $originalFormValues;
    $expectedPostData[$this->expectedFormId . ':AvailabilityCheckBoxList:0'] = 'on';
    $expectedPostData[$this->expectedFormId . ':AvailabilityCheckBoxList:1'] = 'on';
    $expectedPostData[$this->expectedFormId . ':AvailabilityCheckBoxList:2'] = 'on';

    $this->form->expects($this->once())
      ->method('getFormValues')
      ->will($this->returnValue($originalFormValues));

    $this->server->expects($this->once())
      ->method('post')
      ->with($this->expectedAction, $expectedPostData)
      ->will($this->returnValue($this->readfile('availability-1-day.txt')));

    $this->post->post();
  }

  public function testUnsetsOldRowsFromPost() {
    $originalFormValues = array(
       'keep_field' => 'magic value',
       'm_availabilityItems:m_availableDaysRepeater:_ctl6:AvailabilityCheckBoxList:2' => 'on',
       'm_availabilityItems:m_availableDaysRepeater:_ctl6:AvailabilityHiddenIndex' => '336',
    );
    $expectedAvailability = new IntellitimeAvailability(date_make_date('2011-07-14'));
    $expectedPostData = $originalFormValues;
    unset($expectedPostData['m_availabilityItems:m_availableDaysRepeater:_ctl6:AvailabilityCheckBoxList:2']);
    $expectedPostData[$this->expectedFormId . ':AvailabilityCheckBoxList:0'] = 'on';
    $expectedPostData[$this->expectedFormId . ':AvailabilityCheckBoxList:1'] = 'on';
    $expectedPostData[$this->expectedFormId . ':AvailabilityCheckBoxList:2'] = 'on';

    $this->form->expects($this->once())
    ->method('getFormValues')
    ->will($this->returnValue($originalFormValues));

    $this->server->expects($this->once())
    ->method('post')
    ->with($this->expectedAction, $expectedPostData)
    ->will($this->returnValue($this->readfile('availability-1-day.txt')));

    $this->post->post();
  }

  private function readfile($filename) {
    $full_name = dirname(__FILE__) . "/../tests/availability/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    return $contents;
  }

}