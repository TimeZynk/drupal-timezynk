<?php

class IntellitimeAvailabilitySyncController {
  private $availabilityStore;
  private $intellitimeAvailabilityFactory;
  private $serverInterface;

  function __construct($account, $availabilityStore, $intellitimeAvailabilityFactory, $serverInterface) {
    $this->account = $account;
    $this->availabilityStore = $availabilityStore;
    $this->intellitimeAvailabilityFactory = $intellitimeAvailabilityFactory;
    $this->serverInterface = $serverInterface;
  }

  function synchronize() {
    $maxIterations = 100;
    $page = $this->bootstrapAvailabilityPage();
    $intellitime_availabilities = $this->fetchIntellitimeAvailabilities();
    $page->setAvailabilities($intellitime_availabilities);
    while ($post = $page->getPost()) {
      $page = $post->post();
      $page->setAvailabilities($intellitime_availabilities);
      if (--$maxIterations <= 0) {
        throw new TZIntellitimeServerException('Iterated too many times when updating availability!');
      }
    }
  }

  private function bootstrapAvailabilityPage() {
    $url = 'Availability/Availability.aspx?MId=Availability';
    return new IntellitimeAvailabilityPage($this->serverInterface->get($url), $this->serverInterface);
  }

  private function fetchIntellitimeAvailabilities() {
    // Start at 00:00:00 today
    $start = date_make_date('now');
    $start->setTime(0, 0, 0);
    // End four weeks later
    $end = clone($start);
    $end->modify('+29 days');
    $intellitime_availabilities = array();
    $availabilities = $this->availabilityStore->findBetween($this->account->uid, $start, $end);
    foreach ($availabilities as $availability) {
      $intellitime_availabilities[] = $this->intellitimeAvailabilityFactory->create($availability);
    }
    return $intellitime_availabilities;
  }
}