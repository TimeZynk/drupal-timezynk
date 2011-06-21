<?php

class TZDBValidationException extends Exception {
  private $errors = array();


  function addError($message) {
    $this->errors[] = $message;
  }

  function getErrors() {
    return $this->errors;
  }

  function hasErrors() {
    return count($this->errors) > 0;
  }

}