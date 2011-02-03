<?php

class TZIntellitimeBot implements TZIntellitimeServerInterface {
  /**
   * @var TZCurl curl handle
   */
  private $curl;

  /**
   * The intellitime login URL, unique per customer.
   * @var string
   */
  private $login_url = NULL;

  /**
   * Creates a new TZIntellitimeBot
   * @param TZCurl $curl Curl wrapper object
   * @param string $url the customer login URL for intellitime.
   */
  function __construct($curl, $url) {
    $this->curl = $curl;
    $this->login_url = $url;
  }

  public function postWeek($postData) {
    if(empty($postData)) {
      throw new InvalidArgumentException('Empty PostData object in TZIntellitimeBot::postWeek()');
    }

    $actionUrl = $this->build_action_url($postData->getPostAction());
    $parser = $this->request($actionUrl, $postData->getPostData());
    return new TZIntellitimeWeekData($parser);
  }

  public function refreshWeek($date) {
    // Use today as default date
    if($date === NULL) {
      $date = tzbase_make_date();
    }

    $datestr = $date->format('Y-m-d');
    $dateaction = "TimeReport/TimeReport.aspx?DateInWeek=$datestr";

    $url = $this->build_action_url($dateaction);
    $parser = $this->request($url);
    return new TZIntellitimeWeekData($parser);
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
   * Logout from the Intellitime Website
   */
  public function logout() {
    $this->request($this->build_action_url('Portal/LogOut.aspx?MId=LogOut'));
  }

  /**
   * Lowlevel HTTP request. Issues a GET or POST request depending on the value of $post_data
   * @param string $url URL to request
   * @param array $post_data optional key => value array of POST parameters
   * @return TZIntellitimeParser Parser on success, otherwise FALSE
   */
  private function request($url, $post_data = NULL) {
    $output = $this->curl->request($url, $post_data);
    if(empty($output)) {
      throw new TZNetworkFailureException('Empty page returned from ' . $url);
    }
    return new TZIntellitimeParser($output);
  }

  /**
   *  Strip away <Dirname>/<File>.aspx?<query> from current effective URL and append $action instead
   * @param string $url the URL as a string
   * @param string $action the action as a string
   * @return the newly constructed URL
   */
  private function build_action_url($action) {
    $url_parts = explode('?', $this->curl->getLastEffectiveURL());
    /* Check if the URL contains a URL-cookie in the form of
     * http://..../(<cookie>)/Portal/.../*.aspx
     * In this case we should cut the URL right after the cookie before
     * appending the action.
     */
    if(preg_match('%^([^(]+)(\([^)]+\)/).*$%', $url_parts[0], $matches)) {
      return $matches[1] . $matches[2] . $action;
    }
    // No URL-cookie found, just remove the aspx-file and the top folder.
    $base_url = preg_replace("%[^/]+/[^/]+\.aspx.*$%", "", $url_parts[0]);
    return $base_url . $action;
  }
}