<?php

class TZReportQueryBuilderTest extends PHPUnit_Framework_TestCase {
  
  function testSetEndtimeWithNoEndtimeThrows() {
    try {
      $queryBuilder = new TZReportQueryBuilder(TZFLAGS::DELETED);
      $queryBuilder->setEndtimeBefore();
      $this->fail("Expected InvalidArgumentException when missing date");
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

}