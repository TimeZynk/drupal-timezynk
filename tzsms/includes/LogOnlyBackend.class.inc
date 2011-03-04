<?php

class LogOnlyBackend implements SmsBackend {
  public function send($from, $to, $message) {
    watchdog('sms_log', 'SMS Sent from "@from" to "@to": @message', array(
      '@from' => $from,
      '@to' => $to,
      '@message' => $message,
    ));
    return strval(rand(getrandmax()/10, getrandmax()));
  }
}
