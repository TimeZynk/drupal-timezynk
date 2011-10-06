<?php

class TZIntellitimeReport {
  const STATE_OPEN = 0;
  const STATE_REPORTED = 1;
  const STATE_LOCKED = 2;
  const STATE_DELETED = 255;

  public $id = NULL;
  public $state = TZIntellitimeReport::STATE_OPEN;
  public $year = NULL;
  public $month = NULL;
  public $day = NULL;
  public $title = NULL;
  public $begin = NULL;
  public $end = NULL;
  public $break_duration_minutes = 0;
  public $overtime_hours = 0;
  public $comment = '';
  public $jobid = NULL;

  public $stateImmutable = FALSE;
  public $total_hours = 0;

  /**
   * Construct an intellitime report from a tzreport
   */
  public function __construct($tzreport = NULL) {
    if (empty($tzreport)) {
      return;
    }

    if (!empty($tzreport->intellitime_id)) {
      $this->id = tzintellitime_get_intellitime_id($tzreport);
    }

    if (!empty($tzreport->intellitime_jobid)) {
      $this->jobid = $tzreport->intellitime_jobid;
    }

    if ($tzreport->flags < TZFlags::REPORTED) {
      $this->state = TZIntellitimeReport::STATE_OPEN;
    } else if ($tzreport->flags == TZFlags::REPORTED) {
      $this->state = TZIntellitimeReport::STATE_REPORTED;
    } else if ($tzreport->flags == TZFlags::DELETED) {
      $this->state = TZIntellitimeReport::STATE_DELETED;
    } else if ($tzreport->flags == TZFlags::APPROVED ||
               $tzreport->flags == TZFlags::LOCKED) {
      $this->state = TZIntellitimeReport::STATE_LOCKED;
      $this->stateImmutable = TRUE;
    } else {
      throw new InvalidArgumentException('Unknown flag ' . $tzreport->flags . ' in tzreport ' . $tzreport->nid);
    }

    $begindate = tzbase_make_date($tzreport->begintime);
    $this->year = $begindate->format('Y');
    $this->month = $begindate->format('m');
    $this->day = $begindate->format('d');

    $this->title = $tzreport->title;
    $this->comment = $tzreport->body;

    $this->begin = $begindate->format('H:i');
    $this->end = tzbase_make_date($tzreport->endtime)->format('H:i');

    $this->break_duration_minutes = round($tzreport->breakduration/60);
    $this->total_hours = round(($tzreport->endtime - $tzreport->begintime - $tzreport->breakduration) / 3600, 2);
  }

  public function get_date_string() {
    return sprintf("%04d-%02d-%02d", $this->year, $this->month, $this->day);
  }

  /**
   * Convert this TZIntellitimeReport to a tzreport node. If a previous
   * node is available, it is updated instead.
   * @param array $account user object associated with this report
   * @param array $tzreport optional tzreport node to update
   * @return array new or updated tzreport node
   */
  public function convert_to_tzreport($account, $tzreport = NULL) {
    tzbase_include_proto_classes();

    $newTZReport = empty($tzreport);

    if($newTZReport) {
      $tzreport = new stdClass();
      $tzreport->type = 'tzreport';
      $tzreport->travelduration = 0;
      $tzreport->travelkm = 0;
      $tzreport->signature = '';
      $tzreport->name = $account->name;
      $tzreport->uid = $account->uid;
    } else {
      $tzreport = clone($tzreport);
    }

    $tzreport->title = $this->title;
    $tzreport->body = $this->comment;

    // Convert begin and end to DateTime objects
    $begindate = $this->get_begintime();
    $enddate = $this->get_endtime();

    $tzreport->begintime = $begindate->format('U');
    $tzreport->endtime = $enddate->format('U');
    if ($tzreport->endtime < $tzreport->begintime) {
      // endtime can only be smaller if we have passed midnight. Add one day to endtime
      $enddate->modify('+1 day');
      $tzreport->endtime = $enddate->format('U');
    }
    $tzreport->breakduration = $this->break_duration_minutes*60;

    $tzreport->intellitime_id = $this->get_intellitime_id($account->uid);

    switch($this->state) {
      case TZIntellitimeReport::STATE_OPEN:
        if($newTZReport ||
            !$this->stateImmutable ||
            (isset($tzreport->intellitime_last_state) && $tzreport->intellitime_last_state != TZIntellitimeReport::STATE_OPEN)) {
          $tzreport->flags = TZFlags::CREATED;
          $tzreport->intellitime_last_state = $this->state;
        }
        break;
      case TZIntellitimeReport::STATE_REPORTED:
        $tzreport->flags = TZFlags::REPORTED;
        $tzreport->intellitime_last_state = $this->state;
        break;
      default:
        $tzreport->flags = TZFlags::LOCKED;
        $tzreport->intellitime_last_state = $this->state;
        break;
    }

    $tzreport->assignedto = $account->uid;
    $tzreport->intellitime_local_changes = 0;

    return $tzreport;
  }

  /**
   * Get the unique report ID that will be used when saving
   * this report as a tzreport.
   * @param string uid User ID of assigned user
   * @return string unique ID
   */
  public function get_intellitime_id($uid) {
    return serialize(array($this->get_date_string(), (string)$uid, (string)$this->id));
  }

  /**
   * Gets an DateTime object based on the reports year and month, day and begin attributes.
   */
  public function get_begintime() {
    $timezone = date_default_timezone(FALSE);
    $isodate_string = $this->get_date_string() . " " . $this->begin;
    return date_make_date($isodate_string, $timezone);
  }

  /**
   * Gets an DateTime object based on the reports year and month, day and end attributes.
   */
  public function get_endtime() {
    $timezone = date_default_timezone(FALSE);
    $isodate_string = $this->get_date_string() . " " . $this->end;
    return date_make_date($isodate_string, $timezone);
  }
}