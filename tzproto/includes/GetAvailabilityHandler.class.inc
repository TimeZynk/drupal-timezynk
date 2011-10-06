<?php

class GetAvailabilityHandler implements CommandHandler {
  private $account;
  /**
   * @var AvailabilityStore
   */
  private $availability_store;

  public function __construct($account, $availability_store) {
    $this->account = $account;
    $this->availability_store = $availability_store;
  }

  public function handle($get_availability_cmd, &$result, $protocol_version = 0) {
    if (!user_access('view own availability content', $this->account)) {
      throw new TZProtoException('May not view availability', 403);
    }

    $get_availability_result = new TZGetAvailabilityResult();
    $availabilities = $this->get_availabilities($get_availability_cmd);
    foreach ($availabilities as $availability) {
      $tzavailability = $get_availability_result->add_availability();
      $this->populate_tzavailability($tzavailability, $availability);
    }
    $result->set_get_availability_result($get_availability_result);
  }

  private function get_availabilities($get_availability_cmd) {
    $availabilities = array();

    if ($get_availability_cmd->id()) {
      // Get single availability
      $availability = $this->availability_store->find($get_availability_cmd->id());
      if (empty($availability)) {
        throw new TZProtoException("Availability not found", 404);
      }
      if ($availability->getUserId() != $this->account->uid) {
        throw new TZProtoException("May not read other user's availability", 403);
      }
      $availabilities[] = $availability;
    } else {
      // Get all within date range
      $range = tzproto_create_datetime_range($get_availability_cmd->date_range());
      $availabilities = $this->availability_store->findBetween($this->account->uid, $range->start, $range->end);
    }
    return $availabilities;
  }

  /**
   * Populates a TZAvailability object from one Availability DB entry.
   * @param TZAvailability $tzavailability the TZAvailability object to populate.
   * @param Availability $availability a database entry for availability.
   */
  private function populate_tzavailability(&$tzavailability, $availability) {
    $tzavailability->set_id($availability->getId());
    $tzavailability->set_type($availability->getType());

    $time_span = tzproto_create_tztimespan($availability->getStartTime()->format('U'),
                                           $availability->getEndTime()->format('U'));
    $tzavailability->set_time_span($time_span);
  }
}