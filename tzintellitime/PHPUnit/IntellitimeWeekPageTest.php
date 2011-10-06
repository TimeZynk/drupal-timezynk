<?php

class IntellitimeWeekPageTest extends PHPUnit_Framework_TestCase {
  private function loadHTMLFile($filename) {
    $full_name = dirname(__FILE__) . "/../tests/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    return new IntellitimeWeekPage($contents);
  }

  public function testParseAssignments() {
    $page = $this->loadHTMLFile('intellitime-timereport-page.html');
    $assignments = $page->getAssignments();
    $this->assertEquals(29, count($assignments), "expects 29 assignments");
    $count_assignments = 0;
    $count_absence = 0;
    foreach ($assignments as $assignment) {
      if ($assignment->type == TZIntellitimeAssignment::TYPE_ASSIGNMENT) {
        $count_assignments += 1;
        $this->assertEquals("5983", $assignment->id, "assignment code 5983");
      } else {
        $count_absence += 1;
        $this->assertEquals("_AC_", substr($assignment->id, 0, 4));
      }
    }
    $this->assertEquals(1, $count_assignments);
    $this->assertEquals(28, $count_absence);
  }

  public function testParseAssignmentsUTF8Encoding() {
    $page = $this->loadHTMLFile('intellitime-timereport-page.html');
    $assignments = $page->getAssignments();
    $this->assertEquals("Testföretaget Effekt, Lagerarbetare", $assignments[0]->title, "expect UTF-8 encoding");
  }
}
