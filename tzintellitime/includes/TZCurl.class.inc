<?php

class TZIntellitimeServerException extends Exception {
  private $rawData;
  public function __construct($message = '', $data = NULL) {
    parent::__construct($message);
    $this->rawData = $data;
  }

  public function getRawData() {
    return $this->rawData;
  }
}

class TZNetworkFailureException extends TZIntellitimeServerException {}

class TZCurl {
  /**
   * the cURL handle.
   */
  private $handle = NULL;

  /**
   * An array containing the cookiejar file path and
   * the last known effective URL.
   * Fields are referenced as $session_data['cookiejar']
   * and $session_data['last_effective_url']
   * @var Array
   */
  private $session_data = NULL;

  public function __construct($session_data = NULL) {
    $this->handle = curl_init();

    // Resume session if session data is available, otherwise initialize new
    if($session_data) {
      if(!is_file($session_data['cookiejar']) || !(filesize($session_data['cookiejar']) > 0)) {
        throw new InvalidArgumentException('session_data contained invalid cookie jar');
      }
      $this->session_data = $session_data;
    } else {
      $this->session_data = array(
        'cookiejar' => tempnam(file_directory_temp(), 'tzcurl'),
        'created' => time(),
        'last_effective_url' => '',
      );
    }

    $this->setupDefaultCurlOptions();
  }

  /**
   * Destroys us!
   * Evil androids hate robots!
   */
  function __destruct() {
    if($this->handle !== NULL) {
      curl_close($this->handle);
    }
  }

  /**
   * Perform a HTTP request to the given URL. If postData is given
   * it will perform a POST, otherwise a GET.
   *
   * @param string $url
   * @param Array $postData
   */
  public function request($url, $postData = NULL) {
    $this->setupCurlOptionsForRequest($url, $postData);

    $output = curl_exec($this->handle);
    if(empty($output)) {
      $error_msg = curl_error($this->handle);
      throw new TZNetworkFailureException($error_msg);
    }

    $this->session_data['last_effective_url'] = curl_getinfo($this->handle, CURLINFO_EFFECTIVE_URL);
    return $output;
  }

  /**
   * Get the last effective URL used.
   * @return string
   */
  public function getLastEffectiveURL() {
    return $this->session_data['last_effective_url'];
  }

  public function getSessionData() {
    return $this->session_data;
  }

  /**
   * Check if session data could describe a valid session
   * @param Array $sessionData
   * @return TRUE if session data could be valid, else FALSE
   */
  public static function isValidSessionData($sessionData) {
    return isset($sessionData['cookiejar']) &&
      file_exists($sessionData['cookiejar']) &&
      isset($sessionData['last_effective_url']) &&
      strlen($sessionData['last_effective_url']);
  }

  /**
   * Destroy session data and free associated resources
   * @param Array $sessionData
   */
  public static function destroySessionData(&$sessionData) {
    if(isset($sessionData['cookiejar']) && file_exists($sessionData['cookiejar'])) {
      unlink($sessionData['cookiejar']);
      unset($sessionData['cookiejar']);
    }
    unset($sessionData['last_effective_url']);
  }

  private function setupCurlOptionsForRequest($url, $postData = NULL) {
    $curl_opts = array(
      CURLOPT_URL => $url,
    );

    if(!empty($postData)) {
      $curl_opts[CURLOPT_POSTFIELDS] = http_build_query($postData, '', '&');
      $curl_opts[CURLOPT_POST] = TRUE;
    } else {
      $curl_opts[CURLOPT_POSTFIELDS] = NULL;
      $curl_opts[CURLOPT_POST] = FALSE;
    }
    curl_setopt_array($this->handle, $curl_opts);
  }

  /**
   * Configure default cURL options.
   */
  private function setupDefaultCurlOptions() {
    global $db_prefix;

    /* This part here is to make us compatible with the way that
     * simpletest detects if a request comes from a running unit
     * test. If this is not setup, we will get values from the
     * host site instead of the test site. */
    $user_agent = '';
    if (preg_match('/simpletest\d+/', $db_prefix, $matches)) {
      $user_agent = drupal_generate_test_ua($matches[0]);
    } else {
      $user_agent = "TimeZynk/1.0";
    }

    $curl_opts = array(
      CURLOPT_USERAGENT => $user_agent,
      CURLOPT_COOKIEFILE => $this->session_data['cookiejar'],
      CURLOPT_COOKIEJAR => $this->session_data['cookiejar'],
      CURLOPT_RETURNTRANSFER => TRUE,
      CURLOPT_BINARYTRANSFER => TRUE,
      CURLOPT_FAILONERROR => TRUE,
      CURLOPT_FOLLOWLOCATION => TRUE,
      CURLOPT_SSL_VERIFYPEER => FALSE, // Turn of SSL verification since intellitime uses a non-standard CA
      CURLOPT_SSL_VERIFYHOST => 2,
      CURLOPT_TIMEOUT => 30,
      CURLOPT_CONNECTTIMEOUT => 20,
      CURLOPT_HTTPHEADER => array('Accept-Language: sv-SE'),
    );

    curl_setopt_array($this->handle, $curl_opts);
  }
}
