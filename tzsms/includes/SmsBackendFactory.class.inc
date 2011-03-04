<?php

class SmsBackendFactory {
  private $backend;

  function __construct($backend = 'beepsend') {
    $available_backends = SmsBackendFactory::all_backends();
    if (empty($available_backends[$backend])) {
      throw new InvalidArgumentException('Unknown backend');
    }

    $this->backend = $backend;
  }

  /**
   * @return SmsBackend
   */
  function create() {
    switch ($this->backend) {
      case 'log':
        return new LogOnlyBackend();

      case 'sms':
        return new SmsFrameworkBackend();

      case 'beepsend':
      default:
        $httpHelper = new DrupalHttpHelper();
        return new BeepSend(TZSMS_BEEPSEND_USERNAME, TZSMS_BEEPSEND_PASSWORD, $httpHelper);
    }
  }

  static function all_backends() {
    $backends = array(
      'log' => t('Log only'),
      'beepsend' => t('BeepSend'),
    );
    if (module_exists('sms')) {
      $backends['sms'] = t('SMS Framework');
    }
    return $backends;
  }
}
