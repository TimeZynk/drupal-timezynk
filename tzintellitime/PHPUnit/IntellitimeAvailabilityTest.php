<?php

class IntellitimeAvailabilityTest extends PHPUnit_Framework_TestCase {

  /**
   * @var DateTime
   */
  private $expected_date;
  /**
   * @var IntellitimeAvailability
   */
  private $availability;

  public function setUp() {
    $this->expected_date = date_make_date('2011-07-14T00:00');
    $this->availability = new IntellitimeAvailability();
  }

  function testCanSetGetDate() {
    $this->availability->setDate($this->expected_date);
    $this->assertEquals($this->availability->getDate()->format('U'), $this->expected_date->format('U'));
  }

  function testGetDateDoesNotReturnSameObjectAsSet() {
    $this->availability->setDate($this->expected_date);
    $this->assertNotSame($this->availability->getDate(), $this->expected_date);
  }

}