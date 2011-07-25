<?php

class TZSmsLogger implements TZIntellitimeLogger {
  function logException($message, $exception, $logLevel = TZIntellitimeLogger::INFO) {
    // Only send SMS on exceptions that are fatal to the sync..
    // (loglevels start at 0, 0 being the most severe.)
    if ($logLevel <= TZIntellitimeLogger::CRITICAL) {
      // Remove heading part to avoid excessive repetition
      $message_parts = explode(':', $message);
      array_shift($message_parts);
      $message = implode(':', $message_parts);
      tzsms_debug_sms($message);
    }
  }

  function logData($message, $data, $logLevel = TZIntellitimeLogger::INFO) {
    // Do nothing...
  }
}