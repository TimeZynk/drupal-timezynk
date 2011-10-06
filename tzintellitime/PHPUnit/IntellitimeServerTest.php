<?php

class IntellitimeServerTest extends PHPUnit_Framework_TestCase {
  public function setUp() {
    $this->timezone = date_default_timezone(FALSE);
    $this->base_url = 'http://localhost/demo/v2005/';
    $this->curl = $this->getMock('TZCurl');
  }

  public function testCreateServer() {
    $server = new IntellitimeServer($this->curl);
    $this->assertTrue(is_callable(array($server, 'get')));
    $this->assertTrue(is_callable(array($server, 'post')));
  }

  public function testNullPostData() {
    $action = 'Portal/Login.aspx?Gw27UDsdffd9TMdfgfgqqoQ%3d%3d';

    $server = new IntellitimeServer($this->curl);
    try {
      $weekData = $server->post($action, NULL);
      $this->fail("Expected an InvalidArgumentException");
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

  public function testEmptyPostAction() {
    $action = 'Portal/Login.aspx?Gw27UDsdffd9TMdfgfgqqoQ%3d%3d';

    $postData = array(
      'Button' => 'Knapp',
      'Field' => 'Fält',
    );

    $server = new IntellitimeServer($this->curl);
    try {
      $weekData = $server->post("", $postData);
      $this->fail("Expected an InvalidArgumentException");
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }


  public function testPostData() {
    $action = 'Portal/Login.aspx?Gw27UDsdffd9TMdfgfgqqoQ%3d%3d';
    $login_url = $this->base_url . $action;

    $postData = array(
      'Button' => 'Knapp',
      'Field' => 'Fält',
    );

    $expectedResponse = $this->loadHTMLFile('intellitime-v9-timereport-three-open.txt');
    $this->curl->expects($this->once())
               ->method('getLastEffectiveURL')
               ->will($this->returnValue($login_url));
    $this->curl->expects($this->once())
        ->method('request')
        ->with($this->base_url . $action, $postData)
        ->will($this->returnValue($expectedResponse));

    $server = new IntellitimeServer($this->curl);
    $response = $server->post($action, $postData);
    $this->assertEquals($expectedResponse, $response);
  }

  public function testBuildURLAfterError() {
    $expectedDate = new DateTime('2011-01-25', $this->timezone);
    $this->base_url = 'http://localhost/demo/v2005/(adskm23234km2ksd823)/';

    $error_url = $this->base_url . 'Error.aspx?aspxerrorpath=/IntelliplanWeb/v2005/TimeReport.aspx';
    $date_action = 'TimeReport/TimeReport.aspx?DateInWeek=' . $expectedDate->format('Y-m-d');

    $this->curl->expects($this->once())
        ->method('getLastEffectiveURL')
        ->will($this->returnValue($error_url));

    $this->curl->expects($this->once())
        ->method('request')
        ->with($this->base_url . $date_action, NULL)
        ->will($this->returnValue($this->loadHTMLFile('intellitime-main-page.html')));

    $server = new IntellitimeServer($this->curl);
    $server->get($date_action);
  }

  public function testBuildNormalURL() {
    $expectedDate = new DateTime('2011-01-25', $this->timezone);
    $this->base_url = 'http://localhost/demo/v2005/(adskm23234km2ksd823)/';

    $date_action = 'TimeReport/TimeReport.aspx?DateInWeek=' . $expectedDate->format('Y-m-d');

    $this->curl->expects($this->once())
        ->method('getLastEffectiveURL')
        ->will($this->returnValue($this->base_url . $date_action));

    $this->curl->expects($this->once())
        ->method('request')
        ->with($this->base_url . $date_action, NULL)
        ->will($this->returnValue($this->loadHTMLFile('intellitime-main-page.html')));

    $server = new IntellitimeServer($this->curl);
    $server->get($date_action);
  }

  private function loadHTMLFile($filename) {
    $full_name = dirname(__FILE__) . "/../tests/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    return $contents;
  }
}
