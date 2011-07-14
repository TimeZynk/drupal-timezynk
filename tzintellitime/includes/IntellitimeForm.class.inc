<?php

class IntellitimeForm {
  /**
   * @var SimpleXMLElement
   */
  private $form;

  /**
   * Basic abstraction for a HTML form on the intellitime pages.
   * @param SimpleXMLElement $form_element
   */
  public function __construct($form_element) {
    $this->form = $form_element;
  }

  public function getAction() {
    return empty($this->form['action']) ? '' : (string)$this->form['action'];
  }
}