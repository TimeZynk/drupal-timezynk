<?php

class TZIntellitimeAssignment {
  const TYPE_ASSIGNMENT = 0;
  const TYPE_ABSENCE = 1;

  public $id = NULL;
  private $search_term = NULL;
  public $title = NULL;
  public $type = NULL;
  private $may_create_report = FALSE;

  /**
   *
   * @param String $abbreviated_title the (possibly shortened) title.
   */
  public function __construct($abbreviated_title, $search_term = NULL, $id = NULL, $type = TZIntellitimeAssignment::TYPE_ASSIGNMENT) {
    $this->title = $abbreviated_title;
    $this->search_term = $search_term ? $search_term : $abbreviated_title;
    $this->id = $id ? $id :  'PLACEHOLDER_ID_' . md5($abbreviated_title);
    $this->type = $type;
  }

  public function setMayCreateReport($may_create_report) {
    $this->may_create_report = $may_create_report;
  }

  public function mayCreateReport() {
    return $this->may_create_report;
  }

  public function convert_to_tzjob() {
    $tzjob = new stdClass();
    $tzjob->type = 'tzjob';
    $tzjob->body = '';
    $tzjob->parentid = 0;
    $tzjob->flags = TZJobFlags::ACTIVE;
    $tzjob->title = $this->title;
    $tzjob->intellitime_id = $this->id;
    return $tzjob;
  }

  /**
	 * Intellitime job titles are comma-separated strings from several
   * task levels that are sometimes abbreviated in different places
   * on the web page. The abbreviation algorithm removes a few
   * characters from the end of each string before concatenating
   * each string separated by commas.
   *
   * We need to be able to match the full-length versions of these
   * strings with the abbreviated version, and that is the task
   * of this function.
   *
   * <h2>Example</h2>
   * Full version: 'Test Company, My assignment, My task, My reporting code'
   * Abbreviated version: 'Test Com, My assig, My t, My repor'
   *
   * @param String $full_title the full title to match against.
   */
  public function matchFullTitle($full_title) {
    if($full_title == $this->title) {
      return TRUE;
    }

    if($full_title == $this->search_term) {
      return TRUE;
    }

    $keypath = explode(', ', $this->title);
    $jobpath = explode(', ', $full_title);

    if(count($jobpath) != count($keypath)) {
      return FALSE;
    }

    $match = TRUE;
    for($i = 0; $i < count($jobpath); $i++) {
      $keysegment = $keypath[$i];
      $job = $jobpath[$i];
      $segment = substr($job, 0, strlen($keysegment));
      if($keysegment !== $segment) {
        $match = FALSE;
        break;
      }
    }
    return $match;
  }
}