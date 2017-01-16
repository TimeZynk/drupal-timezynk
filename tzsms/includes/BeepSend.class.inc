<?php

interface HttpHelper {
  function post($apiToken, $url, $data);
}

class DrupalHttpHelper implements HttpHelper {
  function post($apiToken, $url, $data) {
    $headers = array();
    $headers['Content-Type'] = 'application/json';
    $headers['Authorization'] = 'Token ' . $apiToken;
    if (!is_string($data)) {
      $data = json_encode($data, JSON_UNESCAPED_UNICODE);
    }
    return drupal_http_request($url, $headers, 'POST', $data);
  }
}

class LogOnlyHttpHelper implements HttpHelper {
  function post($apiToken, $url, $data) {
    watchdog('LogHttp', 'Requesting SMS URL: "@url"', array('@url' => $url));
    return (object)array(
      'code' => 200,
      'data' => strval(rand(getrandmax()/10, getrandmax())),
    );
  }
}

class BeepSend implements SmsBackend {
  private $gatewayUrl = 'https://api.beepsend.com/2/send/';
  private $httpHelper;
  private $apiToken;

  function __construct($apiToken, $httpHelper) {
    if (empty($apiToken) || empty($httpHelper)) {
      throw new InvalidArgumentException('All arguments must be specified');
    }

    $this->apiToken = $apiToken;
    $this->httpHelper = $httpHelper;
  }

  function send($from, $to, $message) {
    $params = array();
    $params['to'] = $to;
    $params['from'] = $from;
    $params['body'] = $message;
    $params['encoding'] = 'UTF-8';

    $httpResult = $this->httpHelper->post($this->apiToken, $this->gatewayUrl, $params);

    if (!empty($httpResult->error)) {
      throw new SmsBackendException($httpResult->error);
    }

    if ($httpResult->code < 200 || $httpResult->code >= 300) {
      throw new SmsBackendException($httpResult->code . ": " . $httpResult->data);
    }

    if (preg_match('/^\s*(E\d+)/', $httpResult->data, $match)) {
      throw new SmsBackendException('BeepSend error code ' . $match[1]);
    }

    return trim($httpResult->data);
  }
}
