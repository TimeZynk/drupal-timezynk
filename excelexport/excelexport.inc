<?php

/**
 * @file Contains the excel export routines
 */
function excelexport_export() {
  tzbase_include_proto_classes();

  // Find and include the PHPExcel library
  $phpexcel_path = libraries_get_path('phpexcel');
  require_once ($phpexcel_path . '/Classes/PHPExcel.php');

  $objPHPExcel = new PHPExcel();

  $query = 'SELECT * FROM {node} n ' .
      'INNER JOIN {node_revisions} rev ON n.vid = rev.vid ' .
      'INNER JOIN {tzreport} tzr ON n.vid = tzr.vid ' .
      'INNER JOIN {tzuser} tzu ON tzr.assignedto = tzu.uid ' .
      'WHERE tzr.flags != 255';
  $query_args = array();

  if (!empty($_GET['start_date']['date'])) {
    $date = date_make_date($_GET['start_date']['date'] . 'T00:00:00');
    $query .= ' AND tzr.begintime >= %d';
    $query_args[] = $date->format('U');
  }

  if (!empty($_GET['end_date']['date'])) {
    $date = date_make_date($_GET['end_date']['date'] . 'T23:59:59');
    $query .= ' AND tzr.begintime <= %d';
    $query_args[] = $date->format('U');
  }

  if (!empty($_GET['users'])) {
    $query .= ' AND(';
    $users_query = array();
    foreach ($_GET['users'] as $uid) {
      $users_query[] = 'tzr.assignedto = %d';
      $query_args[] = $uid;
    }
    $query .= implode(' OR ', $users_query) . ')';
  }

  if (!empty($_GET['assignment'])) {
    $assignment_trail = explode(':', $_GET['assignment']);
    $top_assignment = end($assignment_trail);
    if (!empty($top_assignment) && ctype_digit($top_assignment)) {
      $children = tzbase_children_for_jobid($top_assignment);
      $query .= ' AND (';
      $jobid_query = array();
      foreach ($children as $jobid) {
        $jobid_query[] = 'tzr.jobid = %d';
        $query_args[] = $jobid;
      }
      $query .= implode(' OR ', $jobid_query) . ')';
    }
  }

  $query .= ' ORDER BY tzr.begintime';
  $result = db_query(db_rewrite_sql($query), $query_args);


  /* TODO: look up table header names from db, instead of hardcoding them. */
  $h = array (t('Id'), t('Name'), t('Assignment'), t('Comment'), t('Start'), t('End'),
    t('Break'), t('Total'), t('State'));

  $objPHPExcel->setActiveSheetIndex(0);
  $excelSheet = $objPHPExcel->getActiveSheet();

  $row = 1;
  for ($column = 0; $column < count($h); $column++) {
    $excelSheet->setCellValueByColumnAndRow($column, $row, (string)($h[$column]));
  }

  while ($node = db_fetch_object($result)) {
    $row++;
    $column = 0;
    $job = tzbase_node_load($node->jobid);

    $excelSheet->setCellValueByColumnAndRow($column++, $row, $node->nid);
    $excelSheet->setCellValueByColumnAndRow($column++, $row, $node->fullname);
    $excelSheet->setCellValueByColumnAndRow($column++, $row, $job->title);
    $excelSheet->setCellValueByColumnAndRow($column++, $row, $node->body);

    $excelSheet->setCellValueByColumnAndRow($column, $row, _excelexport_format_datetime($node->begintime));
    $excelSheet->getStyleByColumnAndRow($column, $row)
      ->getNumberFormat()
      ->setFormatCode('yyyy-mm-dd hh:mm');
    $column++;

    $excelSheet->setCellValueByColumnAndRow($column, $row, _excelexport_format_datetime($node->endtime));
    $excelSheet->getStyleByColumnAndRow($column, $row)
      ->getNumberFormat()
      ->setFormatCode('yyyy-mm-dd hh:mm');
    $column++;

    $excelSheet->setCellValueByColumnAndRow($column, $row, _excelexport_format_duration($node->breakduration));
    $excelSheet->getStyleByColumnAndRow($column, $row)
      ->getNumberFormat()
      ->setFormatCode('[h]:mm');
    $column++;

    $excelSheet->setCellValueByColumnAndRow($column, $row, _excelexport_format_duration($node->totalduration));
    $excelSheet->getStyleByColumnAndRow($column, $row)
      ->getNumberFormat()
      ->setFormatCode('[h]:mm');
    $column++;

    $excelSheet->setCellValueByColumnAndRow($column++, $row, _excelexport_format_flags($node->flags));
  }

  _excelexport_autosize_columns($objPHPExcel, $column);

  $filename = _excelexport_save_to_file($objPHPExcel);
  _excelexport_passthru($filename);
  unlink($filename);
}

function _excelexport_save_to_file($objPHPExcel) {
  $filename = tempnam(file_directory_temp(), 'ExcelExport');
  $objWriter = PHPExcel_IOFactory::createWriter($objPHPExcel, 'Excel5');
  $objWriter->save($filename);
  return $filename;
}

function _excelexport_passthru($filename) {
  drupal_set_header('Content-Type: application/vnd.ms-excel');
  drupal_set_header('Content-Disposition: attachment; filename=TimeZynk_Reports_' .
    tzbase_make_date()->format('Y-m-d\TH:i:s') . '.xls');

  // We flush in order for the temp file to be fully written before we pass its contents on to drupal.
  flush();
  $fd = fopen($filename, 'r');
  fpassthru($fd);
  fclose($fd);
}

/**
 * Autosizes the columns of the passed PHPExcel object.
 */
function _excelexport_autosize_columns($php_excel_instance, $max_column = 0) {
  if (!$php_excel_instance) {
    watchdog("Excel export", "Could not autosize columns, no such instance of PHPExcel");
    return;
  }
  // Make columns be the size they need to be in order to be easily read.
  for ($i = 0; $i < $max_column; $i++) {
    // This works so-so..
    $php_excel_instance->getActiveSheet()->getColumnDimensionByColumn($i)->setAutoSize(true);
  }
  //Calulating the column width seems to autosize them, sort of.
  $php_excel_instance->getActiveSheet()->calculateColumnWidths();
}

/**
 * Takes an unixtime timetamp and returns a de-epochified datetime string
 * corrected to be in GMT and not the local timezone.
 * @param $timestamp the UNIX timestamp.
 */
function _excelexport_format_datetime($timestamp) {
  $refdate = tzbase_make_date($timestamp);
  $refdate->setDate(1900,1,-1);
  $refdate->setTime(0,0,0);
  $date = tzbase_make_date($timestamp);
  return ($date->format('U') - $refdate->format('U')
          - $refdate->getOffset() + $date->getOffset())/86400;
}

/**
 * Formats a duration in a format suitable for excel
 * @param int $duration in seconds
 */
function _excelexport_format_duration($duration) {
  return $duration / 3600 / 24;
}

/**
 * Return report flags as a translated human-readable string.
 */
function _excelexport_format_flags($flags) {
  switch($flags) {
    case TZFlags::CREATED:
      return t('New');

    case TZFlags::SIGNED_IN:
      return t('Signed in');

    case TZFlags::REPORTED:
      return t('Reported');

    case TZFlags::APPROVED:
      return t('Approved');

    case TZFlags::LOCKED:
      return t('Locked');
  }
  return t('Unknown');
}
