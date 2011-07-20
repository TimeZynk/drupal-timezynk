<?php

class IntellitimeAvailabilityUpdatePost extends IntellitimePost {
  private $availabilities;

  public function __construct($bot, $form, $availabilities) {
    if (empty($availabilities)) {
      throw new InvalidArgumentException('Missing availabilities');
    }
    parent::__construct($bot, $form);
    $this->availabilities = $availabilities;
  }

  protected function getPostData() {
    $data = $this->form->getFormValues();

    $this->clearAllCheckboxes($data);

    foreach ($this->availabilities as $a) {
      $this->markAvailabilityCheckboxes($data, $a);
    }

    return $data;
  }

  protected function createPage($html_string, $bot) {
    return new IntellitimeAvailabilityFinalPage($html_string, $bot);
  }

  private function clearAllCheckboxes(&$data) {
    foreach ($data as $key => $value) {
      if (preg_match('/:AvailabilityCheckBoxList:\d$/', $key)) {
        unset($data[$key]);
      }
    }
  }

  private function markAvailabilityCheckboxes(&$data, $a) {
    if ($a->isAvailableDuringDay()) {
      $data[$a->getFormId() . ':AvailabilityCheckBoxList:0'] = 'on';
    }
    if ($a->isAvailableDuringEvening()) {
      $data[$a->getFormId() . ':AvailabilityCheckBoxList:1'] = 'on';
    }
    if ($a->isAvailableDuringNight()) {
      $data[$a->getFormId() . ':AvailabilityCheckBoxList:2'] = 'on';
    }
  }
}
