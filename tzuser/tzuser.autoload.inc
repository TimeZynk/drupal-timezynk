<?php
function __tzuser_autoload($class_name) {
  switch ($class_name) {
    case 'MissingColumnException':
    case 'CsvUserListParser':
      require dirname(__FILE__) . '/includes/CsvUserListParser.class.inc';
      return TRUE;
    case 'TZUserFilter':
      require dirname(__FILE__) . '/includes/TZUserFilter.class.inc';
      return TRUE;
    case 'TZUserStatus':
      require dirname(__FILE__) . '/includes/TZUserStatus.class.inc';
      return TRUE;
    default:
      return FALSE;
  }
}
spl_autoload_register('__tzuser_autoload', TRUE, TRUE);
