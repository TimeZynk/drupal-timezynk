<?php

require_once 'CommandHandler.class.inc';

class SaveAvailabilityHandler implements CommandHandler {
  private $account;
  /**
   * @var AvailabilityStore
   */
  private $availability_store;

  public function __construct($account, $availability_store) {
    $this->account = $account;
    $this->availability_store = $availability_store;
  }

  public function handle($save_availability_cmd, &$result, $protocol_version = 0) {
    if (!user_access('create availability content', $this->account)) {
      throw new TZProtoException('May not create availability', 403);
    }

    $save_availability_result = new TZSaveAvailabilityResult();

    $tzavailability = $save_availability_cmd->availability();
    if (!$tzavailability) {
      return;
    }

    $availability = NULL;

    if ($tzavailability->id()) {
      $availability = $this->availability_store->find($tzavailability->id());
      if (empty($availability)) {
        throw new TZProtoException("Availability not found", 404);
      }
      if ($availability->getUserId() != $this->account->uid) {
        throw new TZProtoException("May not modify other user's availability report", 403);
      }
    } else {
      $availability = $this->availability_store->createAvailability();
      $availability->setUserId($this->account->uid);
    }

    $availability->setType($tzavailability->type());

    $time_span = $tzavailability->time_span();
    if (!$time_span) {
      throw new TZProtoException('Missing time span in availability', 400);
    }

    $start_date = tzproto_create_datetime($time_span->date(), $time_span->start());
    $end_date = tzproto_create_datetime($time_span->date(), $time_span->end());
    if ($end_date <= $start_date) {
      $end_date->modify('+1 day');
    }
    $availability->setStartTime($start_date)
                 ->setEndTime($end_date)
                 ->setLocalChanges(TRUE);
    try {
      $availability->save();
    } catch (TZDBValidationException $e) {
      throw new TZProtoException("Validation failed", 400);
    }

    $save_availability_result->set_id($availability->getId());
    $result->set_save_availability_result($save_availability_result);
  }
}