<?php

class IntellitimeServer {
  /**
   * @var TZCurl curl handle
   */
  private $curl;

  /**
   * Creates a new IntellitimeServer
   * @param TZCurl $curl Curl wrapper object
   * @param string $url the customer login URL for intellitime.
   */
  function __construct($curl) {
    $this->curl = $curl;
  }

  public function post($action, $data) {
    if (empty($action) || empty($data)) {
      throw new InvalidArgumentException('Missing arguments to IntellitimeServer::post()');
    }
    $actionUrl = $this->build_action_url($action);
    return $this->request($actionUrl, $data);
  }

  public function get($action) {
    if (strpos($action, "http") === 0) {
      $actionUrl = $action;
    } else {
      $actionUrl = $this->build_action_url($action);
    }
    return $this->request($actionUrl);
  }

  /**
   * Lowlevel HTTP request. Issues a GET or POST request depending on the value of $post_data
   * @param string $url URL to request
   * @param array $post_data optional key => value array of POST parameters
   * @return IntellitimeWeekPage Parser on success, otherwise FALSE
   */
  private function request($url, $post_data = NULL) {
    $output = $this->curl->request($url, $post_data);
    if(empty($output)) {
      throw new TZNetworkFailureException('Empty page returned from ' . $url);
    }
    return $output;
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