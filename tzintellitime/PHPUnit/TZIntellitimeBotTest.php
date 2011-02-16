<?php

class TZIntellitimeBotTest extends PHPUnit_Framework_TestCase {
  public function setUp() {
    $this->timezone = date_default_timezone(FALSE);
  }

  public function testCreateBot() {
    $curlInterface = $this->getMock('TZCurl');
    $url = 'http://localhost/demo/v2005/';

    $bot = new TZIntellitimeBot($curlInterface, $url);
    $this->assertInstanceOf('TZIntellitimeServerInterface', $bot);
  }

  public function testRefreshWeek() {
    $expectedDate = new DateTime('2011-01-25', $this->timezone);
    $curlInterface = $this->getMock('TZCurl');
    $base_url = 'http://localhost/demo/v2005/';
    $login_url = $base_url . 'Portal/Login.aspx?Gw27UDsdffd9TMdfgfgqqoQ%3d%3d';

    $curlInterface->expects($this->once())
        ->method('getLastEffectiveURL')
        ->will($this->returnValue($login_url));

    $curlInterface->expects($this->once())
        ->method('request')
        ->with($base_url . 'TimeReport/TimeReport.aspx?DateInWeek=2011-01-25', NULL)
        ->will($this->returnValue($this->loadHTMLFile('intellitime-v9-timereport-three-open.txt')));

    $bot = new TZIntellitimeBot($curlInterface, $login_url);
    $weekData = $bot->refreshWeek($expectedDate);
    $this->assertInstanceOf('TZIntellitimeWeekData', $weekData);
    $tzjobs = $weekData->getTZJobs();
    $this->assertEquals(1, count($tzjobs));
  }

  public function testRefreshWeekWithoutDate() {
    $expectedDate = tzbase_make_date();
    $curlInterface = $this->getMock('TZCurl');
    $base_url = 'http://localhost/demo/v2005/';
    $login_url = $base_url . 'Portal/Login.aspx?Gw27UDsdffd9TMdfgfgqqoQ%3d%3d';

    $curlInterface->expects($this->once())
        ->method('getLastEffectiveURL')
        ->will($this->returnValue($login_url));

    $curlInterface->expects($this->once())
        ->method('request')
        ->with($base_url . 'TimeReport/TimeReport.aspx?DateInWeek=' . $expectedDate->format("Y-m-d"), NULL)
        ->will($this->returnValue($this->loadHTMLFile('intellitime-v9-timereport-three-open.txt')));

    $bot = new TZIntellitimeBot($curlInterface, $login_url);
    $weekData = $bot->refreshWeek(NULL);
    $this->assertInstanceOf('TZIntellitimeWeekData', $weekData);
    $tzjobs = $weekData->getTZJobs();
    $this->assertEquals(1, count($tzjobs));
  }


  public function testNullPostData() {
    $curlInterface = $this->getMock('TZCurl');
    $base_url = 'http://localhost/demo/v2005/';
    $login_url = $base_url . 'Portal/Login.aspx?Gw27UDsdffd9TMdfgfgqqoQ%3d%3d';

    $bot = new TZIntellitimeBot($curlInterface, $login_url);
    try {
      $weekData = $bot->postWeek(NULL);
      $this->fail("Expected an InvalidArgumentException");
    } catch (InvalidArgumentException $e) {
      $this->assertEquals('Empty PostData object in TZIntellitimeBot::postWeek()', $e->getMessage());
    }
  }


  public function testPostData() {
    $curlInterface = $this->getMock('TZCurl');
    $base_url = 'http://localhost/demo/v2005/';
    $login_url = $base_url . 'Portal/Login.aspx?Gw27UDsdffd9TMdfgfgqqoQ%3d%3d';

    $curlInterface->expects($this->once())
        ->method('getLastEffectiveURL')
        ->will($this->returnValue($login_url));

    $postData = new TZIntellitimePostData();
    $postData->setPostAction('Post/Action/URL');
    $postData->setPostData(array(
      'Button' => 'Knapp',
      'Field' => 'Fält',
    ));


    $curlInterface->expects($this->once())
        ->method('request')
        ->with($base_url . $postData->getPostAction(), $postData->getPostData())
        ->will($this->returnValue($this->loadHTMLFile('intellitime-v9-timereport-three-open.txt')));

    $bot = new TZIntellitimeBot($curlInterface, $login_url);
    $weekData = $bot->postWeek($postData);
    $this->assertInstanceOf('TZIntellitimeWeekData', $weekData);
    $tzjobs = $weekData->getTZJobs();
    $this->assertEquals(1, count($tzjobs));
  }

  public function testLogin() {
    $curlInterface = $this->getMock('TZCurl');
    $base_url = 'http://localhost/demo/v2005/';
    $login_url = $base_url . 'Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d';

    $expectedUsername = 'myuser';
    $expectedPassword = 'mypassword';

    $postData = array(
      'TextBoxUserName' => $expectedUsername,
      'TextBoxPassword' => $expectedPassword,
      'ButtonLogin' => 'Logga in',
      '__VIEWSTATE' => 'dDwyNDA3MjczMzc7dDw7bDxpPDA+Oz47bDx0PDtsPGk8MT47aTw1PjtpPDE1PjtpPDE5Pjs+O2w8dDw7bDxpPDA+Oz47bDx0PDtsPGk8MT47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9DdXN0b21lcnMvaW50ZWxsaXBsYW5fbG9nby5naWY7Pj47Pjs7Pjt0PHA8cDxsPEltYWdlVXJsOz47bDx+L0ltYWdlcy9JbWdfSW50ZWxsaXBsYW5Mb2dvV2hpdGUuZ2lmOz4+Oz47Oz47Pj47Pj47dDxwPHA8bDxWaXNpYmxlOz47bDxvPGY+Oz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxMb2dnYSBpbjs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDs+O2w8R2zDtm10IGzDtnNlbm9yZGV0Pzs+Pjs+Ozs+Oz4+Oz4+Oz5ngNWIe5WIW3O3prUuG7wbptC3jg==',
    );

    $curlInterface->expects($this->at(0))
        ->method('request')
        ->with($login_url, NULL)
        ->will($this->returnValue($this->loadHTMLFile('intellitime-login-page.html')));

    $curlInterface->expects($this->at(1))
        ->method('getLastEffectiveURL')
        ->will($this->returnValue($login_url));

    $curlInterface->expects($this->at(2))
        ->method('request')
        ->with($login_url, $postData)
        ->will($this->returnValue($this->loadHTMLFile('intellitime-main-page.html')));

    $bot = new TZIntellitimeBot($curlInterface, $login_url);
    $loginName = $bot->login($expectedUsername, $expectedPassword);
    $this->assertEquals('Johan Heander', $loginName);
  }

  public function testLogout() {
    $curlInterface = $this->getMock('TZCurl');
    $base_url = 'http://localhost/demo/v2005/';
    $login_url = $base_url . 'Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d';

    $curlInterface->expects($this->once())
        ->method('request')
        ->with($base_url . 'Portal/LogOut.aspx?MId=LogOut', NULL)
        ->will($this->returnValue($this->loadHTMLFile('intellitime-login-page.html')));

    $curlInterface->expects($this->once())
        ->method('getLastEffectiveURL')
        ->will($this->returnValue($login_url));

    $bot = new TZIntellitimeBot($curlInterface, $login_url);
    $bot->logout();
  }

  public function testBuildURLAfterError() {
    $expectedDate = new DateTime('2011-01-25', $this->timezone);
    $curlInterface = $this->getMock('TZCurl');
    $base_url = 'http://localhost/demo/v2005/(adskm23234km2ksd823)/';

    $login_url = $base_url . 'Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d';
    $date_url = $base_url . 'TimeReport/TimeReport.aspx?DateInWeek=' . $expectedDate->format('Y-m-d');
    $error_url = $base_url . 'Error.aspx?aspxerrorpath=/IntelliplanWeb/v2005/TimeReport.aspx';

    $curlInterface->expects($this->once())
        ->method('request')
        ->with($date_url, NULL)
        ->will($this->returnValue($this->loadHTMLFile('intellitime-main-page.html')));

    $curlInterface->expects($this->once())
        ->method('getLastEffectiveURL')
        ->will($this->returnValue($error_url));

    $bot = new TZIntellitimeBot($curlInterface, $login_url);
    $bot->refreshWeek($expectedDate);
  }

  public function testBuildNormalURL() {
    $expectedDate = new DateTime('2011-01-25', $this->timezone);
    $curlInterface = $this->getMock('TZCurl');
    $base_url = 'http://localhost/demo/v2005/(adskm23234km2ksd823)/';

    $login_url = $base_url . 'Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d';
    $date_url = $base_url . 'TimeReport/TimeReport.aspx?DateInWeek=' . $expectedDate->format('Y-m-d');

    $curlInterface->expects($this->once())
        ->method('request')
        ->with($date_url, NULL)
        ->will($this->returnValue($this->loadHTMLFile('intellitime-main-page.html')));

    $curlInterface->expects($this->once())
        ->method('getLastEffectiveURL')
        ->will($this->returnValue($date_url));

    $bot = new TZIntellitimeBot($curlInterface, $login_url);
    $bot->refreshWeek($expectedDate);
  }

  private function loadHTMLFile($filename) {
    $full_name = dirname(__FILE__) . "/../tests/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    return $contents;
  }
}