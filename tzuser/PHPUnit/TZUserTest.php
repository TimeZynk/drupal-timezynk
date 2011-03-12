<?php

class TZuserTest extends PHPUnit_Framework_TestCase {
  function testValidPhoneNumber() {
    $this->assertEquals('0708113896', tzuser_validate_phone_number("070-811 38 96"));
    $this->assertEquals('0708113896', tzuser_validate_phone_number("070 811 38 96"));
    $this->assertEquals('0708113896', tzuser_validate_phone_number("0708113896"));
    $this->assertEquals('46708113896', tzuser_validate_phone_number("+46(70)811-38-96"));
    $this->assertEquals('0708113896', tzuser_validate_phone_number("070/8-113-896"));
    $this->assertEquals('0708113896', tzuser_validate_phone_number("0708 11 38 96"));
    $this->assertEquals('0708113896', tzuser_validate_phone_number("\t0708\t1138\t96\n"));
    $this->assertEquals('0708113896', tzuser_validate_phone_number("  0708   1138 96     "));
  }

  function testInvalidPhoneNumber() {
    $this->assertEquals('', tzuser_validate_phone_number(""));
    $this->assertEquals('', tzuser_validate_phone_number("\t\n"));
    $this->assertEquals('', tzuser_validate_phone_number("ABCDE"));
    $this->assertEquals('', tzuser_validate_phone_number("Lorem Ipsum 0708 dolor 113896"));
    $this->assertEquals('', tzuser_validate_phone_number("12345"));
  }
}