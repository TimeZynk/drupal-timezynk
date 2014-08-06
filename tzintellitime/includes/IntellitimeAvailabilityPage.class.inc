<?php

class IntellitimeAvailabilityPage extends IntellitimeAuthenticatedPage {

  private $addedDays = array();
  private $updatedDays = array();

  /**
   *
   * @return Array map of ISO-date => IntellitimeAvailability
   */
  public function getAvailableDays() {
    return $this->parseCheckboxListsToAvailabilities();
  }

  /**
   *
   * @param Array $availabilites
   */
  public function setAvailabilities($availabilities) {
    $date_range = $this->parseDateRange();
    $existingDays = $this->parseCheckboxListsToAvailabilities();

    foreach ($availabilities as $a) {
      $date = $a->getDate();
      if ($date < $date_range->start || $date_range->end < $date) {
        throw new InvalidArgumentException("Availability out of date range");
      }

      $date_string = $date->format('Y-m-d');
      if (empty($existingDays[$date_string])) {
        $this->addedDays[]  = $a;
      } else {
        $a->setFormId($existingDays[$date_string]->getFormId());
        /* We do not key this, since there might be more than one availability per day,
         * and we want the update post to automagically and additively merge the values
         * of these.
         */
        $this->updatedDays[] = $a;
      }
    }
  }

  /**
   * Gets an IntellitimePost command object.
   * @return IntellitimePost the post, or NULL if there are no changes to push to the server.
   */
  public function getPost() {
    if (!empty($this->addedDays)) {
      return new IntellitimeAvailabilityAddPost($this->server, $this->getForm(), reset($this->addedDays));
    } else {
      return new IntellitimeAvailabilityUpdatePost($this->server, $this->getForm(), $this->updatedDays);
    }
  }

  private function parseCheckboxListsToAvailabilities() {
    $checkboxlists = $this->doc->xpath('//*[@class="AvailabilityCheckBoxList"]');
    if (empty($checkboxlists)) {
      return array();
    }
    $formValues = $this->getForm()->getFormValues();
    foreach ($checkboxlists as $checkboxlist) {
      $prefix = $this->findDayPrefix($checkboxlist);
      if (empty($prefix)) {
        continue;
      }
      $a = $this->populateNewAvailability($prefix, $formValues);
      $available_days[$a->getDate()->format('Y-m-d')] = $a;
    }
    return $available_days;
  }

  private function populateNewAvailability($prefix, $formValues) {
    $day = new IntellitimeAvailability();
    $day->setFormId($prefix);

    $labelId = str_replace('$', '_', $prefix) . '_LabelAvailabilityShortDate';
    $dateLabel = $this->doc->xpath('//span[@id="' . $labelId . '"]');
    $date = date_make_date((string)$dateLabel[0] . 'T00:00:00');
    $day->setDate($date);

    $day->setDay(!empty($formValues[$prefix . '$AvailabilityCheckBoxList$0']));
    $day->setEvening(!empty($formValues[$prefix . '$AvailabilityCheckBoxList$1']));
    $day->setNight(!empty($formValues[$prefix . '$AvailabilityCheckBoxList$2']));

    return $day;
  }

  private function findDayPrefix($checkboxlist) {
    $prefix = "";
    $inputs = $checkboxlist->xpath('.//input[@type="checkbox"]');
    if ($inputs) {
      $input_name_segments = explode('$', $inputs[0]['name']);
      $form_id_segments = array_slice($input_name_segments, 0, -2);
      $prefix = implode('$', $form_id_segments);
    }
    return $prefix;
  }

  private function parseDateRange() {
    $days_since = array();
    $date_links = $this->doc->xpath('//table[@id="CalendarTable"]//a');
    if ($date_links) {
      foreach ($date_links as $date_link) {
        if (preg_match('/\'(\d+)\'\)$/', $date_link['href'], $m)) {
          $days_since[] = intval($m[1], 10);
        }
      }
    }
    $ip_epoch = date_make_date('2000-01-01');
    $start = clone($ip_epoch);
    $start->modify('+' . min($days_since) . ' days');
    $end = clone($ip_epoch);
    $end->modify('+' . max($days_since) . ' days');
    $end->setTime(23, 59, 59);

    return (object)array(
      'start' => $start,
      'end' => $end,
    );
  }
}

class IntellitimeAvailabilityFinalPage extends IntellitimeAvailabilityPage {
  public function getPost() {
    return NULL;
  }
  public function setAvailabilities($availabilities) {
    // Do nothing

  }
}
