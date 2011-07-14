<?php

class IntellitimePageTest extends PHPUnit_Framework_TestCase {
  public function setUp() {
  }

  public function testWhenBuildingFromFormatErrorPage_ItShouldThrowErrorPageException() {
    try {
      $this->build_from_page('errors/System.FormatException.txt');
      $this->fail('expected exception');
    } catch(TZIntellitimeErrorPageException $e) {
      $this->assertEquals("One of the identified items was in an invalid format.", $e->getMessage());
      $this->assertEquals('System.FormatException', $e->getIntellitimeType());
    }
  }

  public function testWhenBuildingFromSQLErrorPage_ItShouldThrowErrorPageException() {
    try {
      $this->build_from_page('errors/System.Data.SqlClient.SqlException.txt');
      $this->fail('expected exception');
    } catch(TZIntellitimeErrorPageException $e) {
      $this->assertEquals("Cannot open database \"DB0116\" requested by the login. The login failed. Login failed for user 'USR_DB0116'.", $e->getMessage());
      $this->assertEquals('System.Data.SqlClient.SqlException', $e->getIntellitimeType());
    }
  }

  private function build_from_page($filename) {
    $full_name = dirname(__FILE__) . "/../tests/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    return new IntellitimePage($contents);
  }
}