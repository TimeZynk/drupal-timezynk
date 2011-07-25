<?php

/**
 * Abstract interface for logging.
 * Used to encapsulates logging capabilities.
 * Supports log severity levels as per http://www.ietf.org/rfc/rfc3164.txt
 */
interface TZIntellitimeLogger {

  /**
   * Emergency: system is unusable
   * @var integer
   */
  const EMERGENCY = 0;
  /**
   * Alert: action must be taken immediately
   * @var integer
   */
  const ALERT = 1;
  /**
   * Critical: critical conditions
   * @var integer
   */
  const CRITICAL = 2;
  /**
   * Error: error conditions
   * @var integer
   */
  const ERROR = 3;
  /**
   * Warning: warning conditions
   * @var integer
   */
  const WARNING = 4;
  /**
   * Notice: normal but significant condition
   * @var integer
   */
  const NOTICE = 5;
  /**
   * Informational: informational messages
   * @var integer
   */
  const INFO = 6;
  /**
   * Debug: debug-level messages
   * @var integer
   */
  const DEBUG = 7;

  /**
   * Logs the message and the exception.
   *
   * @param string $message message to log together with exception
   * @param Exception $exception the exception to log
   * @param int $logLevel the importance of the message. Ranges from ::DEBUG to ::EMERGENCY. Default value is ::INFO.
   */
  function logException($message, $exception, $logLevel = TZIntellitimeLogger::INFO);

  /**
   * Logs the message and the data.
   * @param string $message message to log together with data
   * @param mixed $data data to log
   * @param int $logLevel the importance of the message. Ranges from ::DEBUG to ::EMERGENCY. Default value is ::INFO.
   */
  function logData($message, $data, $logLevel = TZIntellitimeLogger::INFO);
}