<?php

class TZCompositeLogger implements TZIntellitimeLogger {

  /**
   * @var Array
   */
  private $children = array();

  function logException($message, $exception, $logLevel = TZIntellitimeLogger::INFO) {
    foreach ($this->children as $child) {
      $child->logException($message, $exception, $logLevel);
    }
  }

  function logData($message, $data, $logLevel = TZIntellitimeLogger::INFO) {
    foreach ($this->children as $child) {
      $child->logData($message, $data, $logLevel);
    }
  }

	/**
	 * Adds a child logger. The logger must implement the TZIntellitimeLogger
	 * interface. It is allowed to be another CompositeLogger.
   *
	 * All logs directed to the parent logger will be passed on to
	 * the child logger.
	 *
 	 * @param TZIntellitimeLogger $child
 	 */
  function add($child) {
    if (!($child instanceof TZIntellitimeLogger)) {
      throw new InvalidArgumentException("Must be of type TZIntellitimeLogger");
    }
    $this->children[] = $child;
  }

  /**
   * Gets an array of all this loggers children
   */
  function getChildren() {
    return $this->children;
  }

}