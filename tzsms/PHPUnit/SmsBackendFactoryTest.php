<?php

class SmsBackendFactoryTest extends PHPUnit_Framework_TestCase {
  function testCreateLogOnly() {
    $factory = new SmsBackendFactory('log');
    $backend = $factory->create();
    $this->assertTrue($backend instanceof LogOnlyBackend);
  }

  function testCreateBeepSend() {
    $factory = new SmsBackendFactory('beepsend');
    $backend = $factory->create();
    $this->assertTrue($backend instanceof BeepSend);
  }

  function testThrowOnIllegalBackend() {
    try {
      $factory = new SmsBackendFactory('illegal');
      $this->fail('Expect exception');
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }
}
