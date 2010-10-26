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

    $sql = "SELECT n.nid FROM {node} n INNER JOIN {tzreport} tz ON n.nid = tz.nid WHERE type = 'tzreport' AND status = 1 AND tz.flags != %d ORDER BY tz.begintime";
    $db_result = db_query(db_rewrite_sql($sql), TZFlags::DELETED);

    /* TODO: look up table header names from db, instead of hardcoding them. */
    $h = array (t('Id'), t('Subject'), t('Description'), t('Assignment'), t('Assignment description'),
      t('Start time'), t('End time'), t('Break duration'), t('Total'), t('State'), t('Assigned to'));

    $objPHPExcel->setActiveSheetIndex(0);
    $row = 1;
    for ($column = 0; $column < count($h); $column++) {
        $objPHPExcel->getActiveSheet()->setCellValueByColumnAndRow($column, $row, (string)($h[$column]));
    }

    while ($data = db_fetch_object($db_result)) {
        $row++;
        $column = 0;
        $node = node_load($data->nid);
        $job = node_load($node->jobid);
        $assigned_user = user_load($node->assignedto);

        $totalduration = '';

        // Calculate total duration if we are not signed in
        if($node->flags != TZFlags::SIGNED_IN) {
          $totalduration = $node->endtime - $node->begintime - $node->breakduration;
        } else {
          $node->endtime = '';
        }

        $objPHPExcel->getActiveSheet()->setCellValueByColumnAndRow($column++, $row, $node->nid);
        $objPHPExcel->getActiveSheet()->setCellValueByColumnAndRow($column++, $row, $node->title);
        $objPHPExcel->getActiveSheet()->setCellValueByColumnAndRow($column++, $row, $node->body);
        $objPHPExcel->getActiveSheet()->setCellValueByColumnAndRow($column++, $row, $job->title);
        $objPHPExcel->getActiveSheet()->setCellValueByColumnAndRow($column++, $row, $job->body);
        $objPHPExcel->getActiveSheet()->setCellValueByColumnAndRow($column++, $row, _excelexport_build_datetime_string($node->begintime));
        $objPHPExcel->getActiveSheet()->setCellValueByColumnAndRow($column++, $row, _excelexport_build_datetime_string($node->endtime));
        $objPHPExcel->getActiveSheet()->setCellValueByColumnAndRow($column++, $row, _excelexport_format_duration($node->breakduration));
        $objPHPExcel->getActiveSheet()->setCellValueByColumnAndRow($column++, $row, _excelexport_format_duration($totalduration));
        $objPHPExcel->getActiveSheet()->setCellValueByColumnAndRow($column++, $row, _excelexport_format_flags($node->flags));
        $objPHPExcel->getActiveSheet()->setCellValueByColumnAndRow($column++, $row, $assigned_user->realname);
    }

    _excelexport_autosize_columns($objPHPExcel, $column);

    $filename = tempnam(file_directory_temp(), 'ExcelExport');
    $objWriter = PHPExcel_IOFactory :: createWriter($objPHPExcel, 'Excel5');
    $objWriter->save($filename);

    drupal_set_header('Content-Type: application/vnd.ms-excel');
    drupal_set_header('Content-Disposition: attachment; filename=TimeZynk_Reports_' .
      format_date(time(), 'custom', 'Y-m-d') . '.xls');

	// We flush in order for the temp file to be fully written before we pass its contents on to drupal.
    flush();
    $fd = fopen($filename, 'r');
    fpassthru($fd);
    fclose($fd);
    unlink($filename);
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
function _excelexport_build_datetime_string($timestamp) {
    if (is_numeric($timestamp)) {
    	$timestamp = format_date($timestamp, 'custom', 'Y-m-d H:i');
    }
    return $timestamp;
}

/**
 * Formats a duration in a format suitable for excel
 * @param int $duration in seconds
 */
function _excelexport_format_duration($duration) {
  if (is_numeric($duration)) {
    $h = floor($duration/3600);
    $duration -= $h*3600;
    $m = floor($duration/60);
    $duration -= $m*60;
    $s = $duration;
    return sprintf("%d:%02d:%02d", $h, $m, $s);
  }
  return $duration;
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