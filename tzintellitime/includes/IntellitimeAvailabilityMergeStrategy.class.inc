<?php

class IntellitimeAvailabilityMergeStrategy {
  /**
   * @var IntellitimeAvailabilityFactory
   */
  private $factory;

  public function __construct($intellitimeAvailabilityFactory) {
    $this->factory = $intellitimeAvailabilityFactory;
  }

  public function mergeToIntellitimeAvailabilities($availabilities, $intellitime_availabilities) {
    foreach ($availabilities as $a) {
      if (!$a->haveLocalChanges()) {
        continue;
      }
      $date_string = $a->getStartTime()->format('Y-m-d');
      $ia = $this->factory->createIntellitimeAvailability($a);
      if (!empty($intellitime_availabilities[$date_string])) {
        $current_ia = &$intellitime_availabilities[$date_string];
        if ($a->isAvailable()) {
          if ($current_ia->haveLocalChanges()) {
            $current_ia->setDay($current_ia->isAvailableDuringDay() || $ia->isAvailableDuringDay())
                       ->setEvening($current_ia->isAvailableDuringEvening() || $ia->isAvailableDuringEvening())
                       ->setNight($current_ia->isAvailableDuringNight() || $ia->isAvailableDuringNight())
                       ->setLocalChanges();
          } else {
            $intellitime_availabilities[$date_string] = $ia;
          }
        } else {
          // Not available
          if (!$current_ia->haveLocalChanges()) {
            $current_ia->setDay(FALSE)->setEvening(FALSE)->setNight(FALSE)->setLocalChanges();
          }
        }
      } else if ($a->isAvailable()) {
        $intellitime_availabilities[$date_string] = $ia;
      }
    }
    return $intellitime_availabilities;
  }

  public function mergeToAvailabilities($availabilities, $intellitime_availabilities) {
    $date_map = array();
    foreach ($availabilities as $a) {
      $key = $a->getStartTime()->format('Y-m-d');
      $a->not_seen = TRUE;
      if (!isset($date_map[$key])) {
        $date_map[$key] = array($a);
      } else {
        $date_map[$key][] = $a;
      }
    }

    foreach ($intellitime_availabilities as $key => $ia) {
      $matching_availabilites = $this->factory->createAvailabilities($ia);
      if (empty($date_map[$key])) {
        $date_map[$key] = $matching_availabilites;
      } else {
        foreach ($matching_availabilites as $i => $a) {
          if (empty($date_map[$key][$i])) {
            $date_map[$key][$i] = $a;
          } else {
            $date_map[$key][$i]->setStartTime($a->getStartTime())
                               ->setEndTime($a->getEndTime())
                               ->setType($a->getType());
            unset($date_map[$key][$i]->not_seen);
          }
        }
      }
    }

    $result = array();
    foreach ($date_map as $map) {
      foreach ($map as $a) {
        if (isset($a->not_seen)) {
          $a->setType(Availability::DELETED);
          unset ($a->not_seen);
        }
        $result[] = $a;
      }
    }
    return $result;
  }
}