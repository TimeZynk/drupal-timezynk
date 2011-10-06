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

  public function getFormValues() {
    $post = array();
    $post = array_merge($post, $this->buildSelects());
    $post = array_merge($post, $this->buildInputs());
    return $post;
  }

  /**
   * Process all form inputs and build a POST:able array containing their
   * name and current value from DOM.
   *
   * @return Array HTTP POST fields
   */
  private function buildInputs() {
    $result = array();
    $inputs = $this->form->xpath("//input");
    if(empty($inputs)) {
      return $result;
    }

    foreach ($inputs as $input) {
      if (!$input['disabled']) {
        if ($input['type'] != "checkbox") {
          $result[(string)$input['name']] = (string)$input['value'];
        } else if ($input['checked']) {
          $result[(string) $input['name']] = "on";
        }
      }
    }
    return $result;
  }

  /**
  * Takes all select fields in the document and builds postable arrays of the selected options.
  * @return Array arrays of the form a[select tag name] = option value
  */
  private function buildSelects() {
    $result = array();
    $selects = $this->form->xpath("//select");
    foreach ($selects as $select) {
      if($select['disabled'] || count($select->option) === 0) {
        // Disabled or empty selects are excluded from the POST
        continue;
      }

      $name = (string)$select['name'];

      /* Check if the first option has a value and use that as default
       * According to W3C we should instead fallback to the contained
      * text, but that makes us send the "Expense" selects, which seems
      * to be wrong. Not sure what the correct way to handle it is.
      */
      if(isset($select->option[0]['value'])) {
        $result[$name] = (string)$select->option[0]['value'];
      }

      // Check if another option is selected and overrides the default
      foreach ($select->option as $option) {
        if ($option['selected'] && isset($option['value'])) {
          $value = (string) $option['value'];
          $result[$name] = $value;
        }
      }
    }
    return $result;
  }
}