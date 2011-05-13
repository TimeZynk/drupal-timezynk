<?php

class TZIntellitimeUserStatus extends TZUserStatus {
  private $sync_info;

  /**
   * @param TZUserStatus $userStatus
   * @param $sync_info
   */
  function __construct($userStatus, $sync_info) {
    parent::__construct(
      $userStatus->getUid(),
      $userStatus->getStatusTimeStamp(),
      $userStatus->getLastLogin(),
      $userStatus->getLoginLimit()
    );
    $this->setNumberOfDueReports($userStatus->getNumberOfDueReports());
    $this->sync_info = $sync_info;
  }

  function getStatusCode() {
    $original_status_code = parent::getStatusCode();

    if ($original_status_code != TZUserStatus::GREEN) {
      return $original_status_code;
    }

    if ($this->sync_info->intellitime_active) {
      // User session still active, all should be fine and dandy
      return $original_status_code;
    }

    if ($this->getStatusTimeStamp() <= $this->sync_info->intellitime_last_sync) {
      // Last sync is newer than status time, also means everything is alright
      return $original_status_code;
    }

    // Session expired and last sync too old, set user as yellow
    return TZUserStatus::YELLOW;
  }

  function getNumberOfDueReports() {
    $status_code = $this->getStatusCode();
    $number_of_due_reports = parent::getNumberOfDueReports();

    if ($status_code === TZUserStatus::YELLOW &&
        $number_of_due_reports === 0) {
      return TZUserStatus::NUMBER_OF_REPORTS_UNKNOWN;
    }
    return $number_of_due_reports;
  }
}
