<?php

class SmsBackendException extends Exception {}

interface SmsBackend {
  function send($from, $to, $message);
}
