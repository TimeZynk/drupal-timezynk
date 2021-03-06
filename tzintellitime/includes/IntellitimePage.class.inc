<?php

class TZIntellitimeErrorPageException extends TZIntellitimeServerException {
  /**
   * @var String
   */
  private $intellitime_type;

  public function setIntellitimeType($type) {
    $this->intellitime_type = $type;
  }

  public function getIntellitimeType() {
    return $this->intellitime_type;
  }
}

class IntellitimePage {
  /**
   * @var SimpleXMLElement
   */
  protected $doc;

  /**
   * @var IntellitimeServer
   */
  protected $server;

  /**
   * @param String $html_string
   * @param IntellitimeServer $server
   * @throws InvalidArgumentException
   */
  public function __construct($html_string, $server = NULL) {
    if (empty($html_string)) {
      throw new InvalidArgumentException("Empty HTML");
    }

    $this->server = $server;

    $this->doc = $this->load_simplexml_from_html($html_string);
    $this->validate();
  }

  public function __toString() {
    return $this->doc->asXML();
  }

  public function getForm() {
    // Extract forms
    $forms = $this->doc->xpath("//form");
    if($forms) {
      return new IntellitimeForm($forms[0]);
    }
    return NULL;
  }

  /**
   * Check Intellitime Page for errors. Throws TZIntellitimeErrorPageException
   * with details if error page is detected.
   */
  protected function validate() {
    $form = $this->getForm();
    if (!$form) {
      // No form on the page, weird but not an error page
      return;
    }

    $action = $form->getAction();
    if (FALSE === strpos($action, 'Error.aspx')) {
      // No error, just standard page
      return;
    }

    // Found error, build exception
    $message = 'Unknown Intellitime error page';
    $messages = $this->doc->xpath('//*[@id="LabelErrortext"]');
    if ($messages) {
      $message = preg_replace('/\s+/m', " ", (string)$messages[0]);
    }

    $e = new TZIntellitimeErrorPageException($message);

    $exception_types = $this->doc->xpath('//*[@id="ExceptionType"]');
    if ($exception_types) {
      $e->setIntellitimeType((string)$exception_types[0]);
    }
    throw $e;
  }

  private function load_simplexml_from_html($html_string) {
    /* The intelliplan documents contains some horrible misencodings
     * if adding absence rows containing '<' or '>'. They are printed
     * raw into the HTML and makes the parser cut the rest of the line.
     * Try to replace them with HTML entities.
     */
    $replace_lt_gt_with_entities = function($matches) {
      return ' ' . ($matches[1] == '<' ? '&lt;' : '&gt;') . $matches[2];
    };
    $html_string = preg_replace_callback('/\s([<>])(\d+)/', $replace_lt_gt_with_entities, $html_string);

    /* The intellitime docs are declared as ISO-8859-1, but are in fact UTF-8.
     * In order to get rid of the horrible misencodings, we force UTF-8.
     */
    $html_string = mb_convert_encoding($html_string, 'HTML-ENTITIES', 'UTF-8');

    /* Use @ to supress any warnings from malformed HTML soup. */
    $doc = new DOMDocument();
    @$doc->loadHTML($html_string);
    return simplexml_import_dom($doc);
  }
}