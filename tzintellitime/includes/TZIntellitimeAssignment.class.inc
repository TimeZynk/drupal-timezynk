<?php

class TZIntellitimeAssignment {
  const TYPE_ASSIGNMENT = 0;
  const TYPE_ABSENCE = 1;

  public $id = NULL;
  public $report_key = NULL;
  public $title = NULL;
  public $type = TZIntellitimeAssignment::TYPE_ASSIGNMENT;

  public function __construct($title = NULL, $id = NULL, $report_key = NULL) {
    $this->title = $title;

    if($id) {
      $this->id = $id;
    } elseif($title) {
      $this->id = md5($title);
    }

    if($report_key) {
      $this->report_key = $report_key;
    } else {
      $this->report_key = $title;
    }
  }

  public function convert_to_tzjob($account, $tzjob = NULL) {
    if (empty($tzjob)) {
      // New job, set default fields
      $tzjob = new stdClass();
      $tzjob->type = 'tzjob';
      $tzjob->body = '';
      $tzjob->jobcode = '';
      $tzjob->name = $account->name;
      $tzjob->uid = $account->uid;
      $tzjob->parentid = 0;
      $tzjob->flags = TZJobFlags::ACTIVE;
    }

    $tzjob->title = $this->title;
    $tzjob->intellitime_id = $this->id;
    return $tzjob;
  }
}