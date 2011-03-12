<?php

class BeepSendTest extends PHPUnit_Framework_TestCase {
  public function setUp() {
    $this->httpHelper = $this->getMock('HttpHelper');
    $this->beepSend = new BeepSend('myuser', 'mypassword', $this->httpHelper);
  }

  public function testBasicMessage() {
    $expectedTo = '46733623516';
    $expectedFrom = 'TimeZynk';
    $expectedId = '05929360012992463561049446734434150';
    $expectedUrl = "https://connect.beepsend.com/gateway.php?user=myuser&pass=mypassword&to=$expectedTo&from=$expectedFrom&message=Med+%E5%E4%F6%C5%C4%D6";
    $this->httpHelper->expects($this->once())
        ->method('get')
        ->with($expectedUrl)
        ->will($this->returnValue((object)array(
          'code' => 200,
          'data' => " $expectedId\r\n",
        )));

    $id = $this->beepSend->send($expectedFrom, $expectedTo, 'Med åäöÅÄÖ');
    $this->assertEquals($expectedId, $id);
  }

  public function testRequestErrorMessage() {
    $expectedError = 'Severe failure!';
    $expectedTo = '46733623516';
    $expectedFrom = 'TimeZynk';
    $expectedUrl = "https://connect.beepsend.com/gateway.php?user=myuser&pass=mypassword&to=$expectedTo&from=$expectedFrom&message=Med+%E5%E4%F6%C5%C4%D6";
    $this->httpHelper->expects($this->once())
        ->method('get')
        ->with($expectedUrl)
        ->will($this->returnValue((object)array(
          'error' => $expectedError,
        )));

    try {
      $this->beepSend->send($expectedFrom, $expectedTo, 'Med åäöÅÄÖ');
      $this->fail('Expect exception');
    } catch (SmsBackendException $e) {
      $this->assertEquals($expectedError, $e->getMessage());
    }
  }

  public function testNot200StatusMessage() {
    $expectedError = 'Message malformed!';
    $expectedTo = '46733623516';
    $expectedFrom = 'TimeZynk';
    $expectedUrl = "https://connect.beepsend.com/gateway.php?user=myuser&pass=mypassword&to=$expectedTo&from=$expectedFrom&message=Med+%E5%E4%F6%C5%C4%D6";
    $this->httpHelper->expects($this->once())
        ->method('get')
        ->with($expectedUrl)
        ->will($this->returnValue((object)array(
          'code' => 500,
          'data' => $expectedError
        )));

    try {
      $this->beepSend->send($expectedFrom, $expectedTo, 'Med åäöÅÄÖ');
      $this->fail('Expect exception');
    } catch (SmsBackendException $e) {
      $this->assertEquals('500: ' . $expectedError, $e->getMessage());
    }
  }

  public function testBeepSendErrorMessage() {
    $expectedError = 'E1032';
    $expectedTo = '46733623516';
    $expectedFrom = 'TimeZynk';
    $expectedUrl = "https://connect.beepsend.com/gateway.php?user=myuser&pass=mypassword&to=$expectedTo&from=$expectedFrom&message=Med+%E5%E4%F6%C5%C4%D6";
    $this->httpHelper->expects($this->once())
        ->method('get')
        ->with($expectedUrl)
        ->will($this->returnValue((object)array(
          'code' => 200,
          'data' => " $expectedError\r\n"
        )));

    try {
      $this->beepSend->send($expectedFrom, $expectedTo, 'Med åäöÅÄÖ');
      $this->fail('Expect exception');
    } catch (SmsBackendException $e) {
      $this->assertEquals('BeepSend error code ' . $expectedError, $e->getMessage());
    }
  }
}