<?php

class IntellitimeAvailabilitySyncController {
  private $account;
  /**
   * @var AvailabilityStore
   */
  private $availabilityStore;

  /**
   * @var IntellitimeAvailabilityFactory
   */
  private $factory;

  /**
   * @var IntellitimeServer
   */
  private $server;

  /**
   * @var IntellitimeAvailabilityMergeStrategy
   */
  private $mergeStrategy;

  /**
   * @var TZIntellitimeLogger
   */
  private $logger;

  function __construct($account, $availabilityStore, $intellitimeAvailabilityFactory, $server) {
    $this->account = $account;
    $this->availabilityStore = $availabilityStore;
    $this->factory = $intellitimeAvailabilityFactory;
    $this->server = $server;
    $this->mergeStrategy = new IntellitimeAvailabilityMergeStrategy($this->factory);
    $this->logger = new TZCompositeLogger();
  }

  function synchronize() {
    $maxIterations = 100;
    $page = $this->bootstrapAvailabilityPage();
    $availabilities = $this->fetchAvailabilities();
    $intellitime_availabilities = $this->mergeStrategy->mergeToIntellitimeAvailabilities($availabilities, $page->getAvailableDays());
    $page->setAvailabilities($intellitime_availabilities);
    while ($post = $page->getPost()) {
      $page = $post->post();
      $page->setAvailabilities($intellitime_availabilities);
      if (--$maxIterations <= 0) {
        throw new TZIntellitimeServerException('Iterated too many times when updating availability!');
      }
    }
    $availabilities = $this->mergeStrategy->mergeToAvailabilities($availabilities, $page->getAvailableDays());
    $this->storeAvailabilities($availabilities);
    return 100 - $maxIterations;
  }

  function registerLogger($logger) {
    $this->logger = $logger;
  }

  private function bootstrapAvailabilityPage() {
    $url = 'Availability/Availability.aspx?MId=Availability';
    return new IntellitimeAvailabilityPage($this->server->get($url), $this->server);
  }

  private function fetchAvailabilities() {
    // Start at 00:00:00 today
    $start = date_make_date('now');
    $start->setTime(0, 0, 0);
    // End four weeks later
    $end = clone($start);
    $end->modify('+29 days');
    return $this->availabilityStore->findBetween($this->account->uid, $start, $end, Availability::DELETED);
  }

  private function storeAvailabilities($availabilities) {
    foreach($availabilities as $a) {
      if ($a->isDeleted()) {
        $this->availabilityStore->delete($this->account->uid, $a->getId());
      } else {
        $a->setUserId($this->account->uid)
          ->setLocalChanges(FALSE);
        try {
          $a->save();
        } catch(TZDBValidationException $e) {
          $this->logger->logException('Failed to validate availability', $e, TZIntellitimeLogger::WARNING);
        }
      }
    }
  }
}
