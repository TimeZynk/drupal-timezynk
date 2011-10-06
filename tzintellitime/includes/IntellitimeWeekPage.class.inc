<?php

class TZIntellitimeReportRowNotFound extends Exception {}
class TZIntellitimeInconsistentPost extends TZIntellitimeServerException {}

class IntellitimeWeekPage extends IntellitimeAuthenticatedPage {
  private $parser;

  public function __construct($html_string, $server = NULL) {
    parent::__construct($html_string, $server);
    $this->parser = new IntellitimeWeekPageParser($this->doc);
  }

  /**
   * Parse assignment titles and codes from the given document
   * @return array TZIntellitimeAssignment array
   */
  public function getAssignments() {
    $result = array();
    $this->parser->parse_assignment_dropdown($result, 'AddRowDropDown', TRUE);
    $this->parser->parse_assignment_dropdown($result, 'CustOrdersDropDown', FALSE);
    return array_values($result);
  }

  /**
   * Gets all reports from the document.
   * @return Array List of all reports as an array
   */
  public function getReports() {
    return $this->parser->parse_reports();
  }

  /**
   * Return list with weeks that have unfinished reports.
   * @return Array DateTime array with one of the days in each of the weeks that have unfinished reports
   */
  public function getUnfinishedWeeks() {
    return $this->parser->parse_unfinished_weeks();
  }

  public function getUpdatePost(array $itreports, $allReportsDone) {
    $post = new IntellitimeWeekUpdatePost($this->server, $this->getForm());
    $post->setReports($itreports)
         ->setParser($this->parser)
         ->setAllReportsDone($allReportsDone);
    return $post;
  }

  public function getUnlockPost(array $itreports, $unlockImmutable) {
    $post = new IntellitimeWeekUnlockPost($this->server, $this->getForm());
    $post->setReports($itreports)
         ->setParser($this->parser)
         ->setUnlockImmutable($unlockImmutable);
    return $post;
  }

  public function getInsertPost($nid, $itreport) {
    $post = new IntellitimeWeekInsertPost($this->server, $this->getForm());
    $post->setParser($this->parser)
         ->setReport($nid, $itreport);
    return $post;
  }
}
