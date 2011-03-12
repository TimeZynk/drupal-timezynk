<?php

class TZSmsTest extends PHPUnit_Framework_TestCase {
  function testCountryCodeLeadingZero() {
    $this->assertEquals('46733623516', tzsms_set_country_code('0733623516', 46));
    $this->assertEquals('46708113896', tzsms_set_country_code('0708113896', 46));
  }

  function testCountryCodeNoLeadingZero() {
    $this->assertEquals('891012299225', tzsms_set_country_code('1012299225', 89));
    $this->assertEquals('341029992992', tzsms_set_country_code('1029992992', 34));
  }
}