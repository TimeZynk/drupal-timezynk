<?php

class GetAvailabilityIntervalHandler extends CommandHandler {
  /**
   * @var AvailabilityStore
   */
  private $availability_store;

  public function __construct($protocol_version, $availability_store) {
    parent::__construct($protocol_version);
    $this->availability_store = $availability_store;
  }

  public function handle($cmd, &$result) {
    $intervals = $this->availability_store->getAvailabilityIntervals();
    $intervals_result = new TZGetAvailabilityIntervalsResult();

    $intervals_result->set_enabled(!empty($intervals->enabled));
    if (!empty($intervals->days_in_advance) && $intervals->days_in_advance > 0) {
      $intervals_result->set_days_in_advance($intervals->days_in_advance);
    }
    if (!empty($intervals->enabled) && !empty($intervals->list)) {
      foreach ($intervals->list as $interval) {
        $tzinterval = $intervals_result->add_interval();
        $tzinterval->set_start($this->hhmm_to_tztime($interval->begin));
        $tzinterval->set_end($this->hhmm_to_tztime($interval->end));
        $tzinterval->set_exclusive(!empty($interval->exclusive));
      }
    }
    $result->set_get_availability_intervals_result($intervals_result);
  }

  private function hhmm_to_tztime($hhmm) {
    $fields = explode(':', $hhmm);
    $tztime = new TZTime();
    $tztime->set_hour(intval($fields[0], 10));
    $tztime->set_minute(intval($fields[1], 10));
    return $tztime;
  }
}
