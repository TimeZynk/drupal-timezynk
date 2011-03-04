<?php

interface HttpHelper {
  function get($url);
}

class DrupalHttpHelper implements HttpHelper {
  function get($url) {
    return drupal_http_request($url);
  }
}

class LogOnlyHttpHelper implements HttpHelper {
  function get($url) {
    watchdog('LogHttp', 'Requesting SMS URL: "@url"', array('@url' => $url));
    return (object)array(
      'code' => 200,
      'data' => strval(rand(getrandmax()/10, getrandmax())),
    );
  }
}

class BeepSend implements SmsBackend {
  private $gatewayUrl = 'https://connect.beepsend.com/gateway.php';
  private $httpHelper;
  private $username;
  private $password;

  function __construct($username, $password, $httpHelper) {
    if (empty($username) || empty($password) || empty($httpHelper)) {
      throw new InvalidArgumentException('All arguments must be specified');
    }

    $this->username = $username;
    $this->password = $password;
    $this->httpHelper = $httpHelper;
  }

  function send($from, $to, $message) {
    $params = array();
    $params['user'] = $this->username;
    $params['pass'] = $this->password;
    $params['to'] = $to;
    $params['from'] = $from;
    $params['message'] = utf8_decode($message);

    $requestUrl = $this->gatewayUrl . '?' . http_build_query($params, NULL, '&');

    $httpResult = $this->httpHelper->get($requestUrl);

    if (!empty($httpResult->error)) {
      throw new SmsBackendException($httpResult->error);
    }

    if ($httpResult->code < 200 || $httpResult->code >= 300) {
      throw new SmsBackendException($httpResult->code . ": " . $httpResult->data);
    }

    if (preg_match('/^E\d+/', $httpResult->data, $match)) {
      throw new SmsBackendException('BeepSend error code ' . $match[0]);
    }

    return $httpResult->data;
  }
}
