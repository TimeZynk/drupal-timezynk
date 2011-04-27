<?php

class CsvUserListParserTest extends PHPUnit_Framework_TestCase {
  function setUp() {
    $this->example1csv = dirname(__FILE__) . '/example1.csv';
    $this->example2csv = dirname(__FILE__) . '/example2.csv';
  }

  function testCreateFromFile() {
    $parser = new CsvUserListParser($this->example1csv);
    $this->assertNotNull($parser);
  }

  function testNonExistingFileThrows() {
    try {
      $parser = new CsvUserListParser('this_file_does_not_exist.csv');
      $this->fail('expected throw');
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

  function testOpenFileWithMissingColumns() {
    try {
      $parser = new CsvUserListParser($this->example2csv);
      $this->fail('expected throw');
    } catch (MissingColumnException $e) {
      $this->assertNotNull($e);
    }
  }

  function testParseSingleRow() {
    $parser = new CsvUserListParser($this->example1csv);
    $row = $parser->getNextRow();
    $this->assertEquals($row->EmpNo, '1234');
    $this->assertEquals($row->UserId, 'testemp199');
    $this->assertEquals($row->Password, 'pass!');
    $this->assertEquals($row->MobilePhone, '076656555');
    $this->assertEquals($row->BossFirstName, 'Boss');
    $this->assertEquals($row->BossSurname, 'Bossson');
    $this->assertEquals($row->Email, '');
  }

  function testParseAllRows() {
    $expected_number_of_rows = 4;

    $parser = new CsvUserListParser($this->example1csv);
    $number_of_rows = 0;
    while ($row = $parser->getNextRow()) {
      $this->assertNotNull($row->UserId);
      $this->assertNotNull($row->MobilePhone);
      $number_of_rows += 1;
    }
    $this->assertEquals($number_of_rows, $expected_number_of_rows);
  }
}
