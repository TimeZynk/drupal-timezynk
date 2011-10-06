<?php

class IntellitimeLoginPageTest extends PHPUnit_Framework_TestCase {
  public function setUp() {
    $this->server = $this->getMock('IntellitimeServer');
  }

  public function testBuildsLoginPost() {
    $expected_username = 'abc';
    $expected_password = 'aaaoaoo11';
    $page = $this->build_from_page('intellitime-login-page.html');
    $post = $page->getPost($expected_username, $expected_password);
    $this->assertNotNull($post);
  }

  public function testLogin() {
    $base_url = 'http://localhost/demo/v2005/';
    $action = 'Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d';
    $login_url = $base_url . $action;
    $expectedUsername = 'myuser';
    $expectedPassword = 'mypassword';

    $this->server->expects($this->at(0))
        ->method('get')
        ->with($login_url)
        ->will($this->returnValue($this->read_file('intellitime-login-page.html')));

    $expected_post_data = array(
      'TextBoxUserName' => $expectedUsername,
      'TextBoxPassword' => $expectedPassword,
      'ButtonLogin' => 'Logga in',
      '__VIEWSTATE' => 'dDwyNDA3MjczMzc7dDw7bDxpPDA+Oz47bDx0PDtsPGk8MT47aTw1PjtpPDE1PjtpPDE5Pjs+O2w8dDw7bDxpPDA+Oz47bDx0PDtsPGk8MT47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9DdXN0b21lcnMvaW50ZWxsaXBsYW5fbG9nby5naWY7Pj47Pjs7Pjt0PHA8cDxsPEltYWdlVXJsOz47bDx+L0ltYWdlcy9JbWdfSW50ZWxsaXBsYW5Mb2dvV2hpdGUuZ2lmOz4+Oz47Oz47Pj47Pj47dDxwPHA8bDxWaXNpYmxlOz47bDxvPGY+Oz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxMb2dnYSBpbjs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDs+O2w8R2zDtm10IGzDtnNlbm9yZGV0Pzs+Pjs+Ozs+Oz4+Oz4+Oz5ngNWIe5WIW3O3prUuG7wbptC3jg==',
    );

    $this->server->expects($this->at(1))
        ->method('post')
        ->with($action, $expected_post_data)
        ->will($this->returnValue($this->read_file('intellitime-main-page.html')));

    $loginName = IntellitimeLoginPage::login($this->server, $login_url, $expectedUsername, $expectedPassword);
    $this->assertEquals('Johan Heander', $loginName);
  }

  public function testErrorPageOnLogin() {
    $base_url = 'http://localhost/demo/v2005/';
    $action = 'Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d';
    $login_url = $base_url . $action;
    $expectedUsername = 'myuser';
    $expectedPassword = 'mypassword';

    $this->server->expects($this->at(0))
        ->method('get')
        ->with($login_url)
        ->will($this->returnValue($this->read_file('intellitime-login-page.html')));

    $expected_post_data = array(
      'TextBoxUserName' => $expectedUsername,
      'TextBoxPassword' => $expectedPassword,
      'ButtonLogin' => 'Logga in',
      '__VIEWSTATE' => 'dDwyNDA3MjczMzc7dDw7bDxpPDA+Oz47bDx0PDtsPGk8MT47aTw1PjtpPDE1PjtpPDE5Pjs+O2w8dDw7bDxpPDA+Oz47bDx0PDtsPGk8MT47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9DdXN0b21lcnMvaW50ZWxsaXBsYW5fbG9nby5naWY7Pj47Pjs7Pjt0PHA8cDxsPEltYWdlVXJsOz47bDx+L0ltYWdlcy9JbWdfSW50ZWxsaXBsYW5Mb2dvV2hpdGUuZ2lmOz4+Oz47Oz47Pj47Pj47dDxwPHA8bDxWaXNpYmxlOz47bDxvPGY+Oz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxMb2dnYSBpbjs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDs+O2w8R2zDtm10IGzDtnNlbm9yZGV0Pzs+Pjs+Ozs+Oz4+Oz4+Oz5ngNWIe5WIW3O3prUuG7wbptC3jg==',
    );

    $this->server->expects($this->at(1))
        ->method('post')
        ->with($action, $expected_post_data)
        ->will($this->returnValue($this->read_file('WeekData_ThrowOnErrorPage.txt')));

    try {
      IntellitimeLoginPage::login($this->server, $login_url, $expectedUsername, $expectedPassword);
      $this->fail('expects exception');
    } catch(TZIntellitimeErrorPageException $e) {
      $this->assertNotNull($e, 'caught page exception');
    }
  }

  public function testInvalidLogin() {
    $base_url = 'http://localhost/demo/v2005/';
    $action = 'Portal/Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d';
    $login_url = $base_url . $action;
    $expectedUsername = 'myuser';
    $expectedPassword = 'mypassword';

    $this->server->expects($this->at(0))
        ->method('get')
        ->with($login_url)
        ->will($this->returnValue($this->read_file('intellitime-login-page.html')));

    $expected_post_data = array(
      'TextBoxUserName' => $expectedUsername,
      'TextBoxPassword' => $expectedPassword,
      'ButtonLogin' => 'Logga in',
      '__VIEWSTATE' => 'dDwyNDA3MjczMzc7dDw7bDxpPDA+Oz47bDx0PDtsPGk8MT47aTw1PjtpPDE1PjtpPDE5Pjs+O2w8dDw7bDxpPDA+Oz47bDx0PDtsPGk8MT47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9DdXN0b21lcnMvaW50ZWxsaXBsYW5fbG9nby5naWY7Pj47Pjs7Pjt0PHA8cDxsPEltYWdlVXJsOz47bDx+L0ltYWdlcy9JbWdfSW50ZWxsaXBsYW5Mb2dvV2hpdGUuZ2lmOz4+Oz47Oz47Pj47Pj47dDxwPHA8bDxWaXNpYmxlOz47bDxvPGY+Oz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxMb2dnYSBpbjs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDs+O2w8R2zDtm10IGzDtnNlbm9yZGV0Pzs+Pjs+Ozs+Oz4+Oz4+Oz5ngNWIe5WIW3O3prUuG7wbptC3jg==',
    );

    $this->server->expects($this->at(1))
        ->method('post')
        ->with($action, $expected_post_data)
        ->will($this->returnValue($this->read_file('intellitime-login-page.html')));

    try {
      IntellitimeLoginPage::login($this->server, $login_url, $expectedUsername, $expectedPassword);
      $this->fail('expects exception');
    } catch(TZAuthenticationFailureException $e) {
      $this->assertNotNull($e, 'caught auth exception');
    }
  }

  private function build_from_page($filename) {
    return new IntellitimeLoginPage($this->read_file($filename), $this->server);
  }

  private function read_file($filename) {
    $read = read_all_function(dirname(__FILE__) . '/../tests');
    return $read($filename);
  }
}

class IntellitimeMainPageTest extends PHPUnit_Framework_TestCase {
  public function setUp() {
    $this->read_file = read_all_function(dirname(__FILE__) . '../tests');
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

  private function build_from_page($filename) {
    return new IntellitimeMainPage($this->read_file($filename));
  }
}
