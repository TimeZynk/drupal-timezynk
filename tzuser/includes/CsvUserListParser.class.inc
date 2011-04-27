<?php

class MissingColumnException extends Exception {}

class CsvUserListParser {
  private $fd;
  private $columns;

  function __construct($file_path) {
    $this->fd = fopen($file_path, 'r');

    if (!$this->fd) {
      throw new InvalidArgumentException('Input file not found');
    }

    $this->findHeaderColumns();
  }

  private function findHeaderColumns() {
    $header = fgetcsv($this->fd);

    if (FALSE === array_search('UserId', $header)) {
      throw new MissingColumnException('UserId');
    }

    if (FALSE === array_search('MobilePhone', $header)) {
      throw new MissingColumnException('MobilePhone');
    }
    
    $this->columns = $header;
  }

  function getNextRow() {
    $csv_row = fgetcsv($this->fd);
    if (!$csv_row) {
      return FALSE;
    }

    $result = new stdClass();
    foreach ($csv_row as $index => $value) {
      $column_name = $this->columns[$index];
      if ($column_name) {
        $result->$column_name = $value;
      }
    }
    return $result;
  }

}