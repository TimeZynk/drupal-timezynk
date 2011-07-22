<?php

class IntellitimeSynchronizationDriver {
  function __construct($account) {
    $this->account = $account;
  }
  
  function synchronize() {
    if(!tzintellitime_is_authenticated($this->account)) {
      // User not found or not intellitime user
      $this->warning('No session available', $this->account);
      user_save($this->account, array('intellitime_active' => 0));
      return TZIntellitimeSyncController::AUTH_FAILURE;
    }

    try {
      $curl = new TZCurl($this->account->intellitime_session_data);
      $serverInterface = new TZIntellitimeBot($curl, TZINTELLITIME_BASE_URL);

      $loggers = array(new TZWatchdogLogger(), new TZSmsLogger());
      $result = $this->syncReports($serverInterface, $loggers);
      
      if ($result === TZIntellitimeSyncController::SYNC_OK &&
          user_access('create availability content', $this->account)) {
        $this->syncAvailability($serverInterface);
      }

      $curl->close();  // Make sure curl writes its session to disk
    } catch (InvalidArgumentException $e) {
      $this->warning('Caught InvalidArgumentException');
      $result = TZIntellitimeSyncController::AUTH_FAILURE;
    }

    if(TZIntellitimeSyncController::SYNC_OK === $result) {
      $this->account = user_save($this->account, array('intellitime_last_sync' => time()));
    } elseif(TZIntellitimeSyncController::AUTH_FAILURE === $result) {
      // Failed authentication, unschedule user
      $this->warning('Expired session', $this->account);
      TZCurl::destroySessionData($this->account->intellitime_session_data);
      user_save($this->account, array('intellitime_session_data' => NULL, 'intellitime_active' => 0));
    } else {
      // Hopefully a temporary failure, log
      $this->warning('Failed to synchronize', $this->account);
    }

    return $result;
  }
  
  private function syncReports($serverInterface, $loggers) {
    // SyncPolicy starting today
    $syncPolicy = new TZIntellitimeSimpleSyncPolicy(
      $this->account,
      tzbase_make_date(),
      TZINTELLITIME_SYNC_WEEKS_FORWARD,
      TZINTELLITIME_SYNC_WEEKS_BACKWARD
    );
    $weekFactory = new TZIntellitimeDefaultWeekFactory($serverInterface, $this->account);
    $reportStorage = new TZIntellitimeDBReportStorage($this->account);
    $userJobsMappingPolicy = new TZIntellitimeUserJobsMappingPolicy(tzbase_user_jobs_mapper(), $this->account);

    $ctrlr = new TZIntellitimeSyncController($syncPolicy, $weekFactory, $reportStorage, $userJobsMappingPolicy, $this->account);
    foreach ($loggers as $logger) {
      $ctrlr->registerLogger($logger);
    }
    return $ctrlr->synchronize();
  }

  private function syncAvailability($serverInterface) {
    $availabilityStore = tzbase_availability_store();
    $intellitimeAvailabilityFactory = $this->createIntellitimeAvailabilityFactory();
    $controller = new IntellitimeAvailabilitySyncController($this->account, $availabilityStore, $intellitimeAvailabilityFactory, $serverInterface);
    try {
      $controller->synchronize();
    } catch (TZIntellitimeServerException $e) {
      $this->warning('Error page while syncing availability: ' . $e->getMessage());
    }
  }
  
  private function createIntellitimeAvailabilityFactory() {
    $day_range = $this->makeRange(TZINTELLITIME_SYNC_AVAILABILITY_DAY);
    $evening_range = $this->makeRange(TZINTELLITIME_SYNC_AVAILABILITY_EVENING);
    $night_range = $this->makeRange(TZINTELLITIME_SYNC_AVAILABILITY_NIGHT);
    return new IntellitimeAvailabilityFactory($day_range, $evening_range, $night_range);
  }
  
  private function makeRange($range_text) {
    $range_parts = explode('-', $range_text);
    return array_combine(array('start', 'end'), $range_parts);
  }

  private function warning($message) {
    $this->log($message, WATCHDOG_WARNING);
  }

  private function log($message, $severity) {
    $session_age = time();
    if(!empty($this->account->intellitime_session_data['created'])) {
      $session_age -= $this->account->intellitime_session_data['created'];
    }

    watchdog('tzintellitime', '@message, @name (@uid), last sync @interval ago, session created @age ago',
           array(
             '@message' => $message,
             '@name' => $this->account->name,
             '@uid' => $this->account->uid,
             '@interval' => format_interval(time() - $this->account->intellitime_last_sync),
             '@age' => format_interval($session_age),
           ),
           WATCHDOG_NOTICE,
           theme('username', $this->account));
  }
}