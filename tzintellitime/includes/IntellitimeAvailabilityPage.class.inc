<?php

class IntellitimeAvailabilityPage extends IntellitimePage {
  public function getAvailableDays() {
    return $this->parseCheckboxListsToAvailabilities();
  }

  private function parseCheckboxListsToAvailabilities() {
    $checkboxlists = $this->doc->xpath('//table[@class="AvailabilityCheckBoxList"]');
    if (empty($checkboxlists)) {
      return array();
    }
    foreach ($checkboxlists as $checkboxlist) {
      $prefix = $this->findDayPrefix($checkboxlist);
      if (empty($prefix)) {
        continue;
      }
      $available_days[] = $this->populateNewAvailability($prefix);
    }
    return $available_days;
  }

  private function populateNewAvailability($prefix) {
    $day = new IntellitimeAvailability();
    $dateLabel = $this->doc->xpath('//span[@id="' . $prefix . 'LabelAvailabilityShortDate"]');
    $date = date_make_date((string)$dateLabel[0] . 'T00:00:00');
    $day->setDate($date);
    return $day;
  }

  private function findDayPrefix($checkboxlist) {
    $prefix = "";
    if (preg_match("/^m_availabilityItems_m_availableDaysRepeater__ctl\d+_/", $checkboxlist['id'], $m)) {
      $prefix = $m[0];
    }
    return $prefix;
  }
}