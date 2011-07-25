<?php

class TZCompositeLoggerTest extends PHPUnit_Framework_TestCase {

  public function setUp() {
    $this->subject = new TZCompositeLogger();
  }

  public function testCanAddCompositeLoggerToCompositeLogger() {
    $child = new TZCompositeLogger();
    $this->subject->add($child);
    $this->assertSame($child, reset($this->subject->getChildren()));
  }

  public function testCanAddTZSmsLoggerToCompositeLogger() {
    $child = new TZSmsLogger();
    $this->subject->add($child);
    $this->assertSame($child, reset($this->subject->getChildren()));
  }

  public function testCannotAddNonTZIntellitimeLoggerDerivedChildren() {
    try {
      $this->subject->add((object) array('a' => 'b'));
      $this->fail();
    } catch (InvalidArgumentException $i) {
      $this->assertEquals("Must be of type TZIntellitimeLogger", $i->getMessage());
    }
  }

  public function testPropagatesExceptionLogs() {
    $expectedMessage = "a nice message";
    $expectedException = new Exception("message", 203);
    $child = $this->getMock('TZCompositeLogger');

    $child->expects($this->once())
      ->method('logException')
      ->with($expectedMessage, $expectedException);

    $this->subject->add($child);
    $this->subject->logException($expectedMessage, $expectedException);
  }

  public function testPropagatesDataLogs() {
    $expectedMessage = "a nice message";
    $expectedData = array('any_sort_of_data' => 'should be acceptable');
    $child = $this->getMock('TZCompositeLogger');

    $child->expects($this->once())
    ->method('logData')
    ->with($expectedMessage, $expectedData);

    $this->subject->add($child);
    $this->subject->logData($expectedMessage, $expectedData);
  }

  public function testForwardsLogLevel() {
    $expectedLogLevel = TZIntellitimeLogger::EMERGENCY;
    $child = $this->getMock('TZCompositeLogger');

    $child->expects($this->once())
      ->method('logData')
      ->with($this->anything(), $this->anything(), $expectedLogLevel);

    $child->expects($this->once())
      ->method('logException')
      ->with($this->anything(), $this->anything(), $expectedLogLevel);

    $this->subject->add($child);
    $this->subject->logData(NULL, NULL, $expectedLogLevel);
    $this->subject->logException(NULL, NULL, $expectedLogLevel);
  }



}