<?php

class TZIntellitimeWeekDataFactoryTest extends PHPUnit_Framework_TestCase {
  public function setUp() {
    $this->timezone = date_default_timezone(FALSE);
    $this->server = $this->getMock('IntellitimeServer');
  }

  public function testBasicCreateWeekData() {
    $expectedDate = new DateTime('2011-01-25', $this->timezone);
    $expectedAction = 'TimeReport/TimeReport.aspx?DateInWeek=2011-01-25';

    $this->server->expects($this->once())
                 ->method('get')
                 ->with($expectedAction)
                 ->will($this->returnValue($this->loadHTMLFile('intellitime-v9-timereport-three-open.txt')));

    $factory = new TZIntellitimeWeekDataFactory($this->server);
    $weekData = $factory->createWeekData($expectedDate);
    $this->assertInstanceOf('TZIntellitimeWeekData', $weekData);
    $assignments = $weekData->getAssignments();
    $this->assertEquals(1, count($assignments));
  }

  public function testRefreshWeekWithoutDate() {
    $expectedDate = tzbase_make_date();
    $expectedAction = 'TimeReport/TimeReport.aspx?DateInWeek=' . $expectedDate->format("Y-m-d");

    $this->server->expects($this->once())
        ->method('get')
        ->with($expectedAction)
        ->will($this->returnValue($this->loadHTMLFile('intellitime-v9-timereport-three-open.txt')));

    $factory = new TZIntellitimeWeekDataFactory($this->server);
    $weekData = $factory->createWeekData(NULL);
    $this->assertInstanceOf('TZIntellitimeWeekData', $weekData);
    $assignments = $weekData->getAssignments();
    $this->assertEquals(1, count($assignments));
  }


  public function testNullPostData() {
    $factory = new TZIntellitimeWeekDataFactory($this->server);
    try {
      $weekData = $factory->createWeekDataFromPost(NULL);
      $this->fail("Expected an InvalidArgumentException");
    } catch (InvalidArgumentException $e) {
      $this->assertNotNull($e);
    }
  }

  public function testPostData() {
    $post = $this->getMock('IntellitimeWeekUpdatePost');

    $post->expects($this->once())
      ->method('post')
      ->will($this->returnValue(new IntellitimeWeekPage($this->loadHTMLFile('intellitime-v9-timereport-three-open.txt'))));

    $factory = new TZIntellitimeWeekDataFactory($this->server);
    $weekData = $factory->createWeekDataFromPost($post);
    $this->assertInstanceOf('TZIntellitimeWeekData', $weekData);
    $assignments = $weekData->getAssignments();
    $this->assertEquals(1, count($assignments));
  }

  public function testPassesServerToNewPage() {
    $expectedDate = new DateTime('2011-01-25', $this->timezone);
    $expectedAction = 'TimeReport/TimeReport.aspx?DateInWeek=2011-01-25';

    $this->server->expects($this->once())
                 ->method('get')
                 ->with($expectedAction)
                 ->will($this->returnValue($this->loadHTMLFile('intellitime-v9-timereport-three-open.txt')));

    $factory = new TZIntellitimeWeekDataFactory($this->server);
    $weekData = $factory->createWeekData($expectedDate);


    $reports = array(createMockReport('mhbLP96iqH04xVvkWCFl884hbku5Eii3', '2011-01-26', '08:00', '17:00'));
    $reports[0]->intellitime_local_changes = 1;

    $this->server->expects($this->once())
                 ->method('post')
                 ->will($this->returnValue($this->loadHTMLFile('intellitime-v9-timereport-three-open.txt')));
    $post = $weekData->buildPost($reports);
    $page = $post->post();
  }

  private function loadHTMLFile($filename) {
    $full_name = dirname(__FILE__) . "/../tests/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    return $contents;
  }
}