<?php

class TZIntellitimeAssignment {
  const TYPE_ASSIGNMENT = 0;
  const TYPE_ABSENCE = 1;

  public $id = NULL;
  public $report_key = NULL;
  public $title = NULL;
  public $type = TZIntellitimeAssignment::TYPE_ASSIGNMENT;

  public function __construct($title = NULL) {
    $this->title = $title;
    if($title) {
      $this->id = 'PLACEHOLDER_ID_' . md5($title);
      $this->report_key = $title;
    }
  }

  public function convert_to_tzjob($tzjob = NULL) {
    if (empty($tzjob)) {
      // New job, set default fields
      $tzjob = new stdClass();
      $tzjob->type = 'tzjob';
      $tzjob->body = '';
      $tzjob->parentid = 0;
      $tzjob->flags = TZJobFlags::ACTIVE;
    }

    $tzjob->title = $this->title;
    $tzjob->jobcode = $this->report_key;
    $tzjob->intellitime_id = $this->id;
    return $tzjob;
  }
}