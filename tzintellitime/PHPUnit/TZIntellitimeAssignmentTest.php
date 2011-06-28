<?php


class TZIntellitimeAssignmentTest extends PHPUnit_Framework_TestCase {
  public function setUp() {
  }

  function testMatchingAssignmentMatches() {
    $full_title = 'Test Company, My assignment, My task, My reporting code';
    $abbreviated_title = 'Test Com, My assig, My t, My repor';
    $assignment = new TZIntellitimeAssignment($abbreviated_title);
    $this->assertTrue($assignment->matchFullTitle($full_title));
  }

  function testNonMatchingAssignmentDoesNotMatch() {
    $full_title = 'Test Company, My assignment, My task, My reporting code';
    $abbreviated_title = 'Test Com, Mygg assig, My t, My repor';
    $assignment = new TZIntellitimeAssignment($abbreviated_title);
    $this->assertFalse($assignment->matchFullTitle($full_title));
  }

  function testPassingSameTitleMatches() {
    $abbreviated_title = 'Test Com, My assig, My t, My repor';
    $assignment = new TZIntellitimeAssignment($abbreviated_title);
    $this->assertTrue($assignment->matchFullTitle($abbreviated_title));
  }

  function testPassingShortenedTitleAsFullTitleAndViceVersaShouldNotMatch() {
    $full_title = 'Test Company, My assignment, My task, My reporting code';
    $abbreviated_title = 'Test Com, My assig, My t, My repor';
    $assignment = new TZIntellitimeAssignment($full_title);
    $this->assertFalse($assignment->matchFullTitle($abbreviated_title));
  }
}