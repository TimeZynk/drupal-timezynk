<?php

class IntellitimeSynchronizationDriver {
  function __construct($account) {
    $this->account = $account;
  }

  function synchronize() {
    $curl = NULL;

    if(!tzintellitime_is_authenticated($this->account)) {
      // User not found or not intellitime user
      $this->warning('No session available', $this->account);
      user_save($this->account, array('intellitime_active' => 0));
      return array(
        'status' => TZIntellitimeSyncStatus::AUTH_FAILURE,
      );
    }

    try {
      $curl = new TZCurl($this->account->uid);
      $server = new IntellitimeServer($curl);

      $loggers = new TZCompositeLogger();
      $loggers->add(new TZWatchdogLogger());
      $loggers->add(new TZSmsLogger());
      $result = $this->syncReports($server, $loggers);
      $status = isset($result) ? $result['status'] : TZIntellitimeSyncStatus::NETWORK_FAILURE;

      if ($status === TZIntellitimeSyncStatus::SYNC_OK &&
          user_access('create availability content', $this->account) &&
          !user_access('disable own intelliplan availability sync', $this->account)) {
        $result['avIter'] = $this->syncAvailability($server, $loggers);
      }

      $curl->close(); // Make sure curl writes its session to disk

    } catch (InvalidArgumentException $e) {
      $this->warning('Caught InvalidArgumentException');
      return array(
        'status' => TZIntellitimeSyncStatus::AUTH_FAILURE,
      );
    }

    if(TZIntellitimeSyncStatus::SYNC_OK === $status) {
      $curl->closeAndPersistSession(); // Save session to DB
      $this->account = user_save($this->account, array('intellitime_last_sync' => time()));
    } elseif(TZIntellitimeSyncStatus::AUTH_FAILURE === $status) {
      // Failed authentication, unschedule user
      $this->warning('Expired session', $this->account);
      $curl->close();
      TZCurl::destroySessionData($this->account->uid);
      user_save($this->account, array('intellitime_session_data' => NULL, 'intellitime_active' => 0));
    } else {
      // Hopefully a temporary failure, log
      $this->warning('Failed to synchronize', $this->account);
      $curl->closeAndPersistSession(); // Save session to DB
    }

    return $result;
  }

  private function syncReports($server, $loggers) {
    // SyncPolicy starting today
    $syncPolicy = new TZIntellitimeSimpleSyncPolicy(
      $this->account,
      tzbase_make_date(),
      TZINTELLITIME_SYNC_WEEKS_FORWARD,
      TZINTELLITIME_SYNC_WEEKS_BACKWARD
    );
    $weekDataFactory = new TZIntellitimeWeekDataFactory($server);
    $weekFactory = new TZIntellitimeWeekFactory($weekDataFactory, $this->account);
    $reportStorage = new TZIntellitimeDBReportStorage($this->account);
    $userJobsMappingPolicy = new TZIntellitimeUserJobsMappingPolicy(tzbase_user_jobs_mapper(), $this->account);

    $ctrlr = new TZIntellitimeSyncController($syncPolicy, $weekFactory, $reportStorage, $userJobsMappingPolicy, $this->account);
    $ctrlr->registerLogger($loggers);
    return $ctrlr->synchronize();
  }

  private function syncAvailability($server, $loggers) {
    $availabilityStore = tzbase_availability_store();
    $intellitimeAvailabilityFactory = $this->createIntellitimeAvailabilityFactory($availabilityStore);
    $controller = new IntellitimeAvailabilitySyncController($this->account, $availabilityStore, $intellitimeAvailabilityFactory, $server);
    $controller->registerLogger($loggers);
    try {
      return $controller->synchronize();
    } catch (TZIntellitimeServerException $e) {
      $this->warning('Error page while syncing availability: ' . $e->getMessage());
    }
    return 0;
  }

  private function createIntellitimeAvailabilityFactory($availabilityStore) {
    return new IntellitimeAvailabilityFactory($availabilityStore,
                                              TZINTELLITIME_SYNC_AVAILABILITY_DAY,
                                              TZINTELLITIME_SYNC_AVAILABILITY_EVENING,
                                              TZINTELLITIME_SYNC_AVAILABILITY_NIGHT);
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
