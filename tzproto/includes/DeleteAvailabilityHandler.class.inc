<?php

class DeleteAvailabilityHandler implements CommandHandler {
  private $account;
  /**
   * @var AvailabilityStore
   */
  private $availability_store;

  public function __construct($account, $availability_store) {
    $this->account = $account;
    $this->availability_store = $availability_store;
  }

  public function handle($delete_availability_cmd, &$result, $protocol_version = 0) {
    if (!user_access('remove own availability content', $this->account)) {
      throw new TZProtoException('May not remove availability', 403);
    }

    $availability = $this->availability_store->find($delete_availability_cmd->id());
    if (empty($availability)) {
      throw new TZProtoException("Not found", 404);
    }
    if ($availability->getUserId() != $this->account->uid) {
      // may not modify another users availability
      throw new TZProtoException("Forbidden", 403);
    }

    $availability->setType(Availability::DELETED);
    $availability->setLocalChanges();

    try {
      $availability->save();
    } catch (TZDBValidationException $e) {
      throw new TZProtoException("Validation failed", 400);
    }
  }
}