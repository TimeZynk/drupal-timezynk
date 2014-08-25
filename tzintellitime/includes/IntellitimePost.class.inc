<?php

abstract class IntellitimePost {
  /**
   * @var IntellitimeServer
   */
  private $server;

  /**
   * @var IntellitimeForm
   */
  protected $form;

  public function __construct($server, $form) {
    $this->server = $server;
    $this->form = $form;
  }

  public function post() {
    $data = $this->getPostData();
    $html = $this->server->post($this->getAction(), $data);
    $page = NULL;
    try {
      $page = $this->createPage($html, $this->server);
    } catch (TZIntellitimeServerException $e) {
      watchdog('intellitime_post', 'Error page after POST: ' . $e->getMessage() . "\nPOST data: " . print_r($data, TRUE));
      throw $e;
    }
    return $page;
  }

  protected function getAction() {
    return $this->getActionPrefix() . $this->form->getAction();
  }

  protected function getActionPrefix() {
    return "";
  }

  abstract protected function getPostData();
  abstract protected function createPage($html_string, $server);
}
