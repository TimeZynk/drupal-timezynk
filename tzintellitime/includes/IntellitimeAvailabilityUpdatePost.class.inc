<?php

class IntellitimeAvailabilityUpdatePost extends IntellitimePost {
  private $availabilities;

  /**
   * Constructs a POST command that updates all availability in Intellitime.
   * It additively merges the given availabilities for a day before uploading.
   * E.g. given one availability with 'day' and 'night' set to available, and
   * another availability with 'evening' set to available, both for the same
   * date, will result in 'day', 'evening' and 'night' set to available for
   * that date.
   *
   * @param IntellitimeServer $server the server that will do the actuall uploading
   * @param IntellitimeForm $form the availability form from Intellitime.
   * @param Array $availabilities an array containing IntellitimeAvailability
   * objects to push to the server.
   * @throws InvalidArgumentException if availabilites are not given.
   */
  public function __construct($server, $form, $availabilities) {
    if (!isset($availabilities)) {
      throw new InvalidArgumentException('Missing availabilities');
    }
    parent::__construct($server, $form);
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

  protected function getAction() {
    return 'Availability/' . parent::getAction();
  }

  protected function createPage($html_string, $server) {
    return new IntellitimeAvailabilityFinalPage($html_string, $server);
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
