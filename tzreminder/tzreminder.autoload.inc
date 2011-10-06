<?php
function __tzreminder_autoload($class_name) {
  switch ($class_name) {
    case 'TZAbstractReminderPolicy':
      require dirname(__FILE__) . '/includes/TZAbstractReminderPolicy.class.inc';
      return TRUE;
    case 'TZDelayedReminderPolicy':
      require dirname(__FILE__) . '/includes/TZDelayedReminderPolicy.class.inc';
      return TRUE;
    case 'TZFixedDateReminderPolicy':
      require dirname(__FILE__) . '/includes/TZFixedDateReminderPolicy.class.inc';
      return TRUE;
    case 'TZQuery':
      require dirname(__FILE__) . '/includes/TZQuery.class.inc';
      return TRUE;
    case 'TZQueryBuilder':
    case 'TZReportQueryBuilder':
      require dirname(__FILE__) . '/includes/TZQueryBuilder.class.inc';
      return TRUE;
    case 'TZReminderPolicy':
      require dirname(__FILE__) . '/includes/TZReminderPolicy.class.inc';
      return TRUE;
    case 'TZTimeOfDaySpan':
      require dirname(__FILE__) . '/includes/TZTimeOfDaySpan.class.inc';
      return TRUE;
    default:
      return FALSE;
  }
}
spl_autoload_register('__tzreminder_autoload', TRUE, TRUE);
