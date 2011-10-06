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
    $html = $this->server->post($this->getAction(), $this->getPostData());
    return $this->createPage($html, $this->server);
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