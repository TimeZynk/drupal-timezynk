<?php

class BulkUserParserTest extends PHPUnit_Framework_TestCase {
  function testReadExample1() {
    $headers = array_map('strtolower', array('EmpNo','UserId', 'Password', 'FirstName', 'Surname', 'MobilePhone',
                     'Email', 'BossFirstName', 'BossSurname', 'UserLastUpdate', 'BossUserId'));
    $expected_rows = array(
      $this->maprow($headers, array(34, 'testemp199', 'abc', 'Phil', 'Collins', '071234567', 'phil@collins.com', 'Klasse',  'Ab', 40652, 'AbBoss')),
      $this->maprow($headers, array(51, 'testemp200', 'cde', 'Dan', 'Olofsson', '079998992', NULL, 'Bosse', 'Cd', 40652, 'JohanZhe')),
    );

    $parser = new BulkUserParser($this->file('example.xlsx'));
    $rows = $parser->getRows();
    $this->assertEquals($expected_rows, $rows);
  }

  function testIllegalFileThrows() {
    try {
      $parser = new BulkUserParser($this->file('not found.abc'));
      $this->fail('expect exception');
    } catch (Exception $e) {
      $this->assertNotNull($e);
    }
  }

  private function file($name) {
    return (object)array(
      'filepath' => dirname(__FILE__) . "/$name",
      'filename' => $name,
      'filemime' => 'application/octet-stream',
    );
  }

  function maprow($headers, $row) {
    return (object)array_filter(array_combine($headers, $row));
  }
}
