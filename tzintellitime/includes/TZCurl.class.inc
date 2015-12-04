<?php

class TZNetworkFailureException extends TZIntellitimeServerException {}

class TZCurl {
  /**
   * the cURL handle.
   */
  private $handle = NULL;

  private $uid = NULL;
  private $last_effective_url = NULL;
  private $cookiejar = NULL;
  private $cookiejar_created = NULL;

  public function __construct($uid = NULL) {
    $this->uid = $uid;
    $this->handle = curl_init();
    $this->cookiejar = tempnam(file_directory_temp(), 'tzcurl');

    // Resume session if session data is available, otherwise initialize new
    $session = db_fetch_object(db_query("SELECT * FROM {tzintellitime_session} WHERE uid = %d", $uid));
    if ($session) {
      $this->last_effective_url = $session->last_effective_url;
      $this->cookiejar_created = $session->created;
      file_put_contents($this->cookiejar, $session->cookiejar);
    } else {
      $this->last_effective_url = '';
      $this->cookiejar_created = time();
    }

    $this->setupDefaultCurlOptions();
  }

  /**
   * Destroys us!
   * Evil androids hate robots!
   */
  public function __destruct() {
    $this->close();
    if ($this->cookiejar !== NULL) {
      unlink($this->cookiejar);
      $this->cookiejar = NULL;
    }
  }

  public function close() {
    if($this->handle !== NULL) {
      curl_close($this->handle);
      $this->handle = NULL;
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

    $this->last_effective_url = curl_getinfo($this->handle, CURLINFO_EFFECTIVE_URL);

    return $output;
  }

  public function closeAndPersistSession() {
    $this->close(); // Make sure curl writes its session to disk

    if ($this->cookiejar !== NULL) {
      $cookiejar = file_get_contents($this->cookiejar);
      $record = array(
        'uid' => $this->uid,
        'last_effective_url' => $this->last_effective_url,
        'cookiejar' => $cookiejar,
        'changed' => time(),
        'created' => $this->cookiejar_created,
      );

      db_query(
      'INSERT INTO {tzintellitime_session} (uid, last_effective_url, changed, created, cookiejar)
              VALUES (%d, \'%s\', %d, %d, %b)
              ON DUPLICATE KEY
              UPDATE last_effective_url=VALUES(last_effective_url),
                     changed=VALUES(changed),
                     cookiejar=VALUES(cookiejar);'
      );

      unlink($this->cookiejar);
      $this->cookiejar = NULL;
    }
  }

  /**
   * Get the last effective URL used.
   * @return string
   */
  public function getLastEffectiveURL() {
    return $this->last_effective_url;
  }

  public function getSessionData() {
    return array(
      'last_effective_url' => $this->last_effective_url,
      'created' => $this->cookiejar_created,
    );
  }

  /**
   * Check if session data could describe a valid session
   * @param Array $sessionData
   * @return TRUE if session data could be valid, else FALSE
   */
  public static function hasValidSessionData($uid) {
    $info = db_fetch_object(db_query("SELECT LENGTH(last_effective_url) AS url_length, LENGTH(cookiejar) AS cookie_size FROM {tzintellitime_session} WHERE uid = %d LIMIT 1", $uid));
    return $info && $info->url_length > 0 && $info->cookie_size > 0;
  }

  /**
   * Destroy session data and free associated resources
   * @param Array $sessionData
   */
  public static function destroySessionData($uid) {
    db_query("DELETE FROM {tzintellitime_session} WHERE uid = %d", $uid);
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
      CURLOPT_COOKIEFILE => $this->cookiejar,
      CURLOPT_COOKIEJAR => $this->cookiejar,
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
