<?php

class TZAuthenticationFailureException extends TZIntellitimeServerException {}



class TZIntellitimeWeek {

  private $date;
  private $serverInterface = NULL;
  private $reports;
  private $account;

  /**
   * Construct and prepare a week syncer.
   *
   * @param DateTime $datetime
   * @param TZIntellitimeWeekDataPersister $weekDataPersister
   * @param TZIntellitimeServerInterface $serverInterface
   * @param Array $reports
   */
  public function __construct($datetime, $serverInterface, $reports, $account) {
    if(!isset($datetime) || !isset($serverInterface) || !isset($reports) || !isset($account)) {
      throw new InvalidArgumentException('NULL argument supplied to TZIntellitimeWeek constructor');
    }
    $this->date = $datetime;
    $this->serverInterface = $serverInterface;
    $this->reports = $reports;
    $this->account = $account;
  }

  /**
   * Synchronize this week with Intellitime. Check result for
   * reports and assignments that needs to be saved to the database.
   *
   * @return TZIntellitimeWeekSyncResult Result from synchronization
   * @throws TZNetworkFailureException on network problems
   * @throws TZAuthenticationFailureException when session is not authenticated
   */
  public function sync() {
    $result = new TZIntellitimeWeekSyncResult();

    /* RefreshWeek can throw an exception but this is easier to
     * just forward to the caller since no reports have been updated
     * yet in this case.
     */
    $weekData = $this->serverInterface->refreshWeek($this->date);
    $updatedReports = $weekData->updateReports($this->reports, $this->account);

    try {
      $post = $weekData->buildPost($updatedReports);

      while($post) {
        $weekData = $this->serverInterface->postWeek($post);
        if($post->isIntermediate()) {
          $updatedReports = $weekData->updateLastStates($updatedReports);
          $post = $weekData->buildPost($updatedReports);
        } else {
          $post = NULL;
        }
      }

      $updatedReports = $weekData->updateReports($updatedReports, $this->account, TRUE);
    } catch(TZIntellitimeServerException $e) {
      /* We must return the exceptions instead of throwing them. Since even if
       * some of the POSTs have failed, the reports have still been updated with
       * intellitime_last_state and need to be saved by the caller.
       */
      $result->exception = $e;
    } catch(TZIntellitimeInconsistentPost $e) {
      $result->exception = $e;
    }

    $result->tzreports = $updatedReports;
    $result->intellitime_assignments = $weekData->getAssignments();
    $result->unfinishedWeeks = $weekData->getUnfinishedWeeks();
    return $result;
  }
}