<?php

class TZSmsLogger implements TZIntellitimeLogger {
  function logException($message, $exception) {
    // Only send SMS on fatal exceptions
    if ($exception instanceof TZIntellitimeInconsistentPost ||
        $exception instanceof TZIntellitimeErrorPageException) {
      // Remove heading part to avoid excessive repetition
      $message_parts = explode(':', $message);
      array_shift($message_parts);
      $message = implode(':', $message_parts);
      tzsms_debug_sms($message);
    }
  }

  function logData($message, $data) {
    // Do nothing...
  }
}