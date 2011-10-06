<?php

class TZIntellitimeServerException extends Exception {
  private $rawData;
  public function __construct($message = '', $data = NULL) {
    parent::__construct($message);
    $this->rawData = $data;
  }

  public function getRawData() {
    return $this->rawData;
  }
}