<?php

abstract class IntellitimePost {
  private $bot;
  protected $form;

  public function __construct($bot, $form) {
    $this->bot = $bot;
    $this->form = $form;
  }

  public function post() {
    $html = $this->bot->post($this->getAction(), $this->getPostData());
    return $this->createPage($html, $this->bot);
  }

  protected function getAction() {
    return $this->form->getAction();
  }

  abstract protected function getPostData();
  abstract protected function createPage($html_string, $bot);
}