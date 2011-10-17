<?php
function __tzproto_autoload($class_name) {
  switch ($class_name) {
    case 'CommandHandler':
      require dirname(__FILE__) . '/includes/CommandHandler.class.inc';
      return TRUE;
    case 'CreateReportHandler':
      require dirname(__FILE__) . '/includes/CreateReportHandler.class.inc';
      return TRUE;
    case 'DeleteAvailabilityHandler':
      require dirname(__FILE__) . '/includes/DeleteAvailabilityHandler.class.inc';
      return TRUE;
    case 'GetAvailabilityHandler':
      require dirname(__FILE__) . '/includes/GetAvailabilityHandler.class.inc';
      return TRUE;
    case 'GetAvailabilityIntervalHandler':
      require dirname(__FILE__) . '/includes/GetAvailabilityIntervalHandler.class.inc';
      return TRUE;
    case 'GetReportTemplatesHandler':
      require dirname(__FILE__) . '/includes/GetReportTemplatesHandler.class.inc';
      return TRUE;
    case 'SaveAvailabilityHandler':
      require dirname(__FILE__) . '/includes/SaveAvailabilityHandler.class.inc';
      return TRUE;
    default:
      return FALSE;
  }
}
spl_autoload_register('__tzproto_autoload', TRUE, TRUE);
