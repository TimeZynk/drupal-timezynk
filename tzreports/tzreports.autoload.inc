<?php
function __tzreports_autoload($class_name) {
  switch ($class_name) {
    case 'TZReportsTimeReports':
      require dirname(__FILE__) . '/includes/TZReportsTimeReports.class.inc';
      return TRUE;
    default:
      return FALSE;
  }
}
spl_autoload_register('__tzreports_autoload', TRUE, TRUE);
