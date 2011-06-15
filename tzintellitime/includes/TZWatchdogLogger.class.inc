<?php

class TZWatchdogLogger implements TZIntellitimeLogger {
  function logException($message, $exception) {
    $data = '';
    if ($exception) {
      $data = $exception->getRawData();
    }
    $this->logData($message, $data);
  }

  function logData($message, $data) {
    tzbase_watchdog_with_data('tzintellitime', $message, $data);
  }
}