<?php

class SmsFrameworkBackend implements SmsBackend {
  public function send($from, $to, $message) {
    $result = sms_send($to, $message);

    if (!$result) {
      throw new SmsBackendException('Failed to send message through SMS Framework');
    }

    return $result;
  }
}

