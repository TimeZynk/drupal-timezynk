<?php
function __tzbase_autoload($class_name) {
  switch ($class_name) {
    case 'Availability':
      require dirname(__FILE__) . '/includes/Availability.class.inc';
      return TRUE;
    case 'AvailabilityStore':
      require dirname(__FILE__) . '/includes/AvailabilityStore.class.inc';
      return TRUE;
    case 'TZDBValidationException':
      require dirname(__FILE__) . '/includes/TZDBValidationException.class.inc';
      return TRUE;
    case 'TZDBWrapper':
      require dirname(__FILE__) . '/includes/TZDBWrapper.class.inc';
      return TRUE;
    case 'TZDrupalDBWrapper':
      require dirname(__FILE__) . '/includes/TZDrupalDBWrapper.class.inc';
      return TRUE;
    case 'TZUserJobMapping':
      require dirname(__FILE__) . '/includes/TZUserJobMapping.class.inc';
      return TRUE;
    case 'TZUserJobsMapper':
      require dirname(__FILE__) . '/includes/TZUserJobsMapper.class.inc';
      return TRUE;
    default:
      return FALSE;
  }
}
spl_autoload_register('__tzbase_autoload', TRUE, TRUE);
