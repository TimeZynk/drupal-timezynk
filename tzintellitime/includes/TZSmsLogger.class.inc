<?php

class TZSmsLogger implements TZIntellitimeLogger {
  function logException($message, $exception) {
    // Only send SMS on fatal exceptions
    if ($exception instanceof TZIntellitimeInconsistentPost) {
      tzsms_debug_sms($message);
    } else if($exception instanceof TZIntellitimeErrorPageException) {
      tzsms_debug_sms($message);
    }
  }

  function logData($message, $data) {
    // Do nothing...
  }
}