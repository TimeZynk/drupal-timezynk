<?php

class TZWatchdogLogger implements TZIntellitimeLogger {
  function logException($message, $exception, $logLevel = TZIntellitimeLogger::INFO) {
    $data = '';
    if ($exception) {
      $data = $exception->getRawData();
    }
    $this->logData($message, $data, $logLevel);
  }

  function logData($message, $data, $logLevel = TZIntellitimeLogger::INFO) {
    // Logger log severity levels are identical with WATCHDOG-levels.
    tzbase_watchdog_with_data('tzintellitime', $message, $data, $logLevel);
  }
}