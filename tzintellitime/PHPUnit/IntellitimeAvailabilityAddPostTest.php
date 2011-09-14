<?php

class IntellitimeAvailabilityAddPostTest extends PHPUnit_Framework_TestCase {
  function setUp() {
    $this->bot = $this->getMock('TZIntellitimeBot');

    $this->form = $this->getMockBuilder('IntellitimeForm')->disableOriginalConstructor()->getMock();
    $this->formAction = "Availability.aspx?MId=Availability";
    $this->expectedAction = "Availability/Availability.aspx?MId=Availability";
    $this->form->expects($this->once())
      ->method('getAction')
      ->will($this->returnValue($this->formAction));
    $this->expectedAvailability = new IntellitimeAvailability(date_make_date('2011-07-14'));
    $this->post = new IntellitimeAvailabilityAddPost($this->bot, $this->form, $this->expectedAvailability);
  }

  function testPostCallsBot() {
    $this->bot->expects($this->once())
      ->method('post')
      ->will($this->returnValue($this->readfile('availability-1-day.txt')));
    $this->post->post();
  }

  function testUsesCorrectActionInPost() {
    $this->bot->expects($this->once())
      ->method('post')
      ->with($this->expectedAction, $this->anything())
      ->will($this->returnValue($this->readfile('availability-1-day.txt')));

    $this->post->post();
  }

  function testUsesCorrectPostDataInPostForCurrentMonth() {
    $originalFormValues = array(
      'keep_field' => 'magic value',
    );
    $expectedPostData = $originalFormValues;
    $expectedPostData['__EVENTTARGET'] = 'm_availabilityCalendar:ThisMonth';
    $expectedPostData['__EVENTARGUMENT'] = '4212'; // Days since 2000-01-01

    $this->form->expects($this->once())
      ->method('getFormValues')
      ->will($this->returnValue($originalFormValues));

    $this->bot->expects($this->once())
      ->method('post')
      ->with($this->expectedAction, $expectedPostData)
      ->will($this->returnValue($this->readfile('availability-1-day.txt')));

    $this->post->post();
  }

  function testCreatesExpectedPageFromPost() {
    $this->form->expects($this->once())
      ->method('getFormValues')
      ->will($this->returnValue(array()));

    $this->bot->expects($this->once())
      ->method('post')
      ->will($this->returnValue($this->readfile('availability-1-day.txt')));

    $page = $this->post->post();
    $this->assertInstanceOf('IntellitimeAvailabilityPage', $page);
  }

  private function readfile($filename) {
    $full_name = dirname(__FILE__) . "/../tests/availability/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    return $contents;
  }

}