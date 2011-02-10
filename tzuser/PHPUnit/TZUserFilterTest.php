<?php

class TZUserFilterTest extends PHPUnit_Framework_TestCase {
  function testFilterInclusive() {
    $filter = new TZUserFilter('include', array(1,2,3,4));
    $this->assertTrue($filter->isIncluded(1));
    $this->assertTrue($filter->isIncluded(2));
    $this->assertTrue($filter->isIncluded(3));
    $this->assertTrue($filter->isIncluded(4));
    $this->assertFalse($filter->isIncluded(6));
  }

  function testFilterExclusive() {
    $filter = new TZUserFilter('exclude', array(1,2,3,4));
    $this->assertFalse($filter->isIncluded(1));
    $this->assertFalse($filter->isIncluded(2));
    $this->assertFalse($filter->isIncluded(3));
    $this->assertFalse($filter->isIncluded(4));
    $this->assertTrue($filter->isIncluded(6));
  }

  function testEmptyFilterInclusive() {
    $filter = new TZUserFilter('include', array());
    $this->assertFalse($filter->isIncluded(1));
    $this->assertFalse($filter->isIncluded(2));
    $this->assertFalse($filter->isIncluded(3));
    $this->assertFalse($filter->isIncluded(4));
    $this->assertFalse($filter->isIncluded(6));
  }

  function testEmptyFilterExclusive() {
    $filter = new TZUserFilter('exclude', array());
    $this->assertTrue($filter->isIncluded(1));
    $this->assertTrue($filter->isIncluded(2));
    $this->assertTrue($filter->isIncluded(3));
    $this->assertTrue($filter->isIncluded(4));
    $this->assertTrue($filter->isIncluded(6));
  }
}

