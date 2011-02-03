<?php

class TZIntellitimeBot {
  /**
   * the cURL handle used for login.
   * @var a handle or NULL
   */
  private $handle = NULL;

  /**
   * The intellitime login URL, unique per customer.
   * @var string
   */
  private $login_url = NULL;

  /**
   * An array containing the cookiejar file path and
   * the last known effective URL.
   * Fields are referenced as $session_data['cookiejar']
   * and $session_data['last_effective_url']
   * @var Array
   */
  private $session_data = NULL;

  /**
   * Creates a new TZIntellitimeBot
   * @param string $url the customer login URL for intellitime.
   * @param Array $session_data Optional session data from a previous session
   */
  function __construct($url, $session_data = NULL) {
    $handle = curl_init();
    $this->handle = $handle;
    $this->login_url = $url;

    // Resume session if session data is available, otherwise initialize new
    if($session_data) {
      if(!is_file($session_data['cookiejar']) || !(filesize($session_data['cookiejar']) > 0)) {
        throw new InvalidArgumentException('session_data contained invalid cookie jar');
      }
      $this->session_data = $session_data;
    } else {
      $this->session_data = array(
        'cookiejar' => tempnam(file_directory_temp(), 'tzintellitimebot'),
      );
    }

    $this->setup_default_curl_options();
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
   * Logs in to the intellitime web service at $url
   * @param string $url the customer specific login URL.
   * @param string $username the user to login as
   * @param string $password the password to login with
   * @return bool TRUE if successful, otherwise FALSE.
   */
  public function login($username, $password) {
    $parser = $this->request($this->login_url);
    $post = $parser->build_login_post($username, $password);
    $post_url = $this->build_action_url("Portal/" . $post['action']);
    $parser = $this->request($post_url, $post['data']);
    return $parser->is_valid_login();
  }

  /**
   * Fetch all available intellitime data for the week that contains the given date
   * @param int $date DateTime object
   * @return FALSE on network failure, empty array on login failure, otherwise array with keys
   *   'assignments', 'reports' and 'unfinished_weeks'.
   */
  public function load_week($date = NULL) {
    // Use today as default date
    if($date === NULL) {
      $date = tzbase_make_date();
    }

    $datestr = $date->format('Y-m-d');
    $dateaction = "TimeReport/TimeReport.aspx?DateInWeek=$datestr";

    $url = $this->build_action_url($dateaction);
    $parser = $this->request($url);

    return $this->week_data($parser);
  }

  /**
   * Logout from the Intellitime Website
   */
  public function logout() {
    $this->request($this->build_action_url('Portal/LogOut.aspx?MId=LogOut'));
  }

  /**
   * Configure default cURL options.
   */
  private function setup_default_curl_options() {
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

  /**
   * Lowlevel HTTP request. Issues a GET or POST request depending on the value of $post_data
   * @param string $url URL to request
   * @param array $post_data optional key => value array of POST parameters
   * @return TZIntellitimeParser Parser on success, otherwise FALSE
   */
  public function request($url, $post_data = NULL) {
    $curl_opts = array(
      CURLOPT_URL => $url,
    );

    if(!empty($post_data)) {
      $curl_opts[CURLOPT_POSTFIELDS] = http_build_query($post_data, '', '&');
      $curl_opts[CURLOPT_POST] = TRUE;
    } else {
      $curl_opts[CURLOPT_POSTFIELDS] = NULL;
      $curl_opts[CURLOPT_POST] = FALSE;
    }
    curl_setopt_array($this->handle, $curl_opts);

    $output = curl_exec($this->handle);
    if(!empty($output)) {
      $this->session_data['last_effective_url'] = curl_getinfo($this->handle, CURLINFO_EFFECTIVE_URL);
      return new TZIntellitimeParser($output);
    }
    $error_msg = curl_error($this->handle);
    watchdog('tzintellitime', 'Request failed: @error', array('@error' => $error_msg), WATCHDOG_WARNING);
    throw new TZNetworkFailureException($error_msg);
  }

  /**
   *  Strip away <Dirname>/<File>.aspx?<query> from current effective URL and append $action instead
   * @param string $url the URL as a string
   * @param string $action the action as a string
   * @return the newly constructed URL
   */
  private function build_action_url($action) {
    $base_url = preg_replace("/[^\/]+\/[^\/]+\.aspx.*$/", "", $this->session_data['last_effective_url']);
    return $base_url . $action;
  }

  /**
   * Send an updated report to the intellitime web service.
   * @param TZIntellitimeReport $report a report containing the updated values.
   * @return FALSE on network failure, empty array on login failure, otherwise array with keys
   *   'assignments', 'reports' and 'unfinished_weeks'.
   */
  public function update_report($report) {
    $datestr = $report->get_date_string();
    $dateaction = "TimeReport/TimeReport.aspx?DateInWeek=$datestr";

    // We need fresh info about the time reports for build_update_report_post.
    $url = $this->build_action_url($dateaction);
    $parser = $this->request($url);
    $reports = $parser->parse_reports();

    // Check the current state of the report that has changed
    foreach($reports as $old_report) {
      if ($report->id === $old_report->id) {
        if ($report == $old_report) {
          // No need to do anything, the report has not changed.
          return $this->week_data($parser);
        } else if ($old_report->state == TZIntellitimeReport::STATE_REPORTED) {
          // First open the report
          $old_state = $report->state;
          $report->state = TZIntellitimeReport::STATE_OPEN;
          $parser = $this->post_report($parser, $dateaction, $report);
          $report->state = $old_state;
        } else if ($old_report->state == TZIntellitimeReport::STATE_LOCKED) {
          // Report locked, nothing can be done
          return $this->week_data($parser);
        }

        // Send the post.
        $parser = $this->post_report($parser, $dateaction, $report);
        break;
      }
    }

    return $this->week_data($parser);
  }

  private function post_report($parser, $dateaction, $report) {
    $post_url = $this->build_action_url($dateaction);
    $post = $parser->build_update_report_post($report);
    return $this->request($post_url, $post['data']);
  }

  /**
   * Fetches persistable session data that can be used to resume
   * this session with a different TZIntellitimeBot object.
   * @return Array session_data
   */
  public function get_session_data() {
    return $this->session_data;
  }

  /**
   * Parse current week data.
   * @param TZIntellitimeParser $parser
   * @return Array array with keys 'assignments', 'reports' and 'unfinished_weeks' or empty array if not logged in
   */
  private function week_data($parser) {
    if(!$parser->is_valid_login()) {
      throw new TZAuthenticationFailureException();
    }

    $result = array();
    $assignments = $parser->parse_assignments();
    $absence_types = $parser->parse_absence_types();
    $result['assignments'] = array_merge($assignments, $absence_types);
    $result['reports'] = $parser->parse_reports();
    $result['unfinished_weeks'] = $parser->parse_unfinished_weeks();
    return $result;
  }

  /**
   * Check if session data could describe a valid session
   * @param Array $session_data
   * @return TRUE if session data could be valid, else FALSE
   */
  public static function is_valid_session_data($session_data) {
    return isset($session_data['cookiejar']) &&
      file_exists($session_data['cookiejar']) &&
      isset($session_data['last_effective_url']) &&
      strlen($session_data['last_effective_url']);
  }

  /**
   * Destroy session data and free associated resources
   * @param Array $session_data
   */
  public static function destroy_session_data(&$session_data) {
    if(isset($session_data['cookiejar']) && file_exists($session_data['cookiejar'])) {
      unlink($session_data['cookiejar']);
      unset($session_data['cookiejar']);
    }
    unset($session_data['last_effective_url']);
  }
}