<?php

class IntellitimeAuthenticatedPageTest extends PHPUnit_Framework_TestCase {
  public function testLoginCheckerCatchesFailedLogin() {
    try {
      $page = $this->build_from_page('intellitime-login-page.html');
      $this->fail('no exception caught');
    } catch (TZAuthenticationFailureException $e) {
      $this->assertNotNull($e);
    }
  }

  public function testLoginCheckerCatchesSuccessfulLogin() {
    $page = $this->build_from_page('intellitime-main-page.html');
    $this->assertNotNull($page);
  }

  private function build_from_page($filename) {
    $full_name = dirname(__FILE__) . "/../tests/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    return new IntellitimeAuthenticatedPage($contents);
  }
}