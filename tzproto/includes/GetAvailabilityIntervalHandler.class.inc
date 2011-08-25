<?php

require_once 'CommandHandler.class.inc';

class GetAvailabilityIntervalHandler implements CommandHandler {
  /**
   * @var AvailabilityStore
   */
  private $availability_store;

  public function __construct($availability_store) {
    $this->availability_store = $availability_store;
  }

  public function handle($command, &$result) {
    $intervals = $this->availability_store->getAvailabilityIntervals();
    $intervals_result = new TZGetAvailabilityIntervalsResult();

    $intervals_result->set_enabled(!empty($intervals->enabled));
    if (!empty($intervals->enabled) && !empty($intervals->list)) {
      foreach ($intervals->list as $interval) {
        $tzinterval = $intervals_result->add_interval();
        $tzinterval->set_from($this->hhmm_to_tztime($interval[0]));
        $tzinterval->set_to($this->hhmm_to_tztime($interval[1]));
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