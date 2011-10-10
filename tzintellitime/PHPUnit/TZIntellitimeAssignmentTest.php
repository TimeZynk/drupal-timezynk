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

  function testGeneratesPlaceHolderIDWithDefaultConstructor() {
    $full_title = 'Test Company, My assignment, My task, My reporting code';
    $expected_id = 'PLACEHOLDER_ID_' . md5($full_title);
    $assignment = new TZIntellitimeAssignment($full_title);
    $this->assertEquals($expected_id, $assignment->id);
  }

  function testTranslatesAbsenceAssignmentsToType() {
    $full_title = 'Test Company, My assignment, My task, My reporting code';
    $assignment = new TZIntellitimeAssignment($full_title, NULL, NULL, TZIntellitimeAssignment::TYPE_ABSENCE);
    $tzjob = $assignment->convert_to_tzjob();
    $this->assertEquals(TZJobType::ABSENCE, $tzjob->jobtype);
  }

  function testTranslatesPresenceAssignmentsToType() {
    $full_title = 'Test Company, My assignment, My task, My reporting code';
    $assignment = new TZIntellitimeAssignment($full_title, NULL, NULL, TZIntellitimeAssignment::TYPE_ASSIGNMENT);
    $tzjob = $assignment->convert_to_tzjob();
    $this->assertEquals(TZJobType::PRESENCE, $tzjob->jobtype);
  }
}
