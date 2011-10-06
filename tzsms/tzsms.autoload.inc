<?php
function __tzsms_autoload($class_name) {
  switch ($class_name) {
    case 'HttpHelper':
    case 'DrupalHttpHelper':
    case 'LogOnlyHttpHelper':
    case 'BeepSend':
      require dirname(__FILE__) . '/includes/BeepSend.class.inc';
      return TRUE;
    case 'LogOnlyBackend':
      require dirname(__FILE__) . '/includes/LogOnlyBackend.class.inc';
      return TRUE;
    case 'SmsBackendException':
    case 'SmsBackend':
      require dirname(__FILE__) . '/includes/SmsBackend.class.inc';
      return TRUE;
    case 'SmsBackendFactory':
      require dirname(__FILE__) . '/includes/SmsBackendFactory.class.inc';
      return TRUE;
    case 'SmsFrameworkBackend':
      require dirname(__FILE__) . '/includes/SmsFrameworkBackend.class.inc';
      return TRUE;
    default:
      return FALSE;
  }
}
spl_autoload_register('__tzsms_autoload', TRUE, TRUE);
