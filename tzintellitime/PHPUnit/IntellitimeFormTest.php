<?php

class IntellitimeFormTest extends PHPUnit_Framework_TestCase {
  public function setUp() {
  }

  public function testWhenBuildingFromLoginPage_ItShouldParseCorrectFormAction() {
    $form = $this->build_from_page('intellitime-login-page.html');
    $this->assertEquals('Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d', $form->getAction());
  }

  private function build_from_page($filename) {
    $full_name = dirname(__FILE__) . "/../tests/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    $page = new IntellitimePage($contents);
    return $page->getForm();
  }
}