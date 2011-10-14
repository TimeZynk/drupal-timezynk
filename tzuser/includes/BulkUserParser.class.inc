<?php

class BulkUserParser {
  private $doc;
  private $manager_lookup;

  function __construct($file, $manager_lookup) {
    // Find and include the PHPExcel library
    $phpexcel_path = libraries_get_path('phpexcel');
    require_once ($phpexcel_path . '/Classes/PHPExcel.php');

    $reader = PHPExcel_IOFactory::createReaderForFile($file->filepath);
    if (is_callable($reader, 'setReadDataOnly')) {
      $reader->setReadDataOnly(true);
    }
    if ($reader instanceof PHPExcel_Reader_CSV) {
      $reader->setDelimiter(',');
      $reader->setEnclosure('"');
      $reader->setLineEnding("\r\n");
      $reader->setSheetIndex(0);
    }
    $this->doc = $reader->load($file->filepath);

    $this->manager_lookup = $manager_lookup;
  }

  function getRows() {
    $rows = array();
    $headers = array();

    $sheet = $this->doc->getActiveSheet();
    foreach ($sheet->getRowIterator() as $row) {
      if (empty($headers)) {
        $headers = $this->indexHeaders($row);
      } else {
        $rows[] = $this->readRow($headers, $row);
      }
    }

    return $rows;
  }

  private function readRow($headers, $row) {
    $data = array();
    foreach ($row->getCellIterator() as $cell) {
      $header = $headers[$cell->getColumn()];
      if (!empty($header)) {
        $cell->setDataType(PHPExcel_Cell_DataType::TYPE_STRING);
        $data[$header] = $cell->getValue();
      }
    }
    return (object)$data;
  }

  private function indexHeaders($row) {
    $headers = array();
    foreach ($row->getCellIterator() as $cell) {
      $headers[$cell->getColumn()] = strtolower($cell->getValue());
    }
    return $headers;
  }
}


