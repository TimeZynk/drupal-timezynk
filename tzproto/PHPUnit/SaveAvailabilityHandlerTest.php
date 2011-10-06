<?php

class SaveAvailabilityHandlerTest extends PHPUnit_Framework_TestCase {
  function setUp() {
    $this->store = $this->getMockBuilder('AvailabilityStore')
                        ->disableOriginalConstructor()
                        ->getMock();
    $this->account = new stdClass();
    $this->handler = new SaveAvailabilityHandler(0, $this->account, $this->store);
    $this->command = new TZSaveAvailabilityCmd();
    $this->result = new TZResult();
  }

  function testCreatedSuccessfully() {
    /*
     * Cannot test handle() at the moment since it is using user_access
     * which we in that case need to mock since drupal is not initialized
     * when running the tests.
     */
    $this->assertNotNull($this->handler);
  }
}
