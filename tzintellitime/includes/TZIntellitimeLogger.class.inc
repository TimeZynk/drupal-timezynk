<?php

/**
 * Abstract interface for logging.
 * Used to encapsulates logging capabilities.
 *
 */
interface TZIntellitimeLogger {

  /**
   * Logs the message and the exception.
   *
   * @param string $message message to log together with exception
   * @param Exception $exception the exception to log
   */
  function logException($message, $exception);

  /**
   * Logs the message and the data.
   * @param string $message message to log together with data
   * @param mixed $data data to log
   */
  function logData($message, $data);
}