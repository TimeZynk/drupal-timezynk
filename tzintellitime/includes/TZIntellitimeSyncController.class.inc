<?php

/**
 * Controller class for handling data synchronization between intellitime and timezynk
 */
class TZIntellitimeSyncController {
  const SYNC_OK = 0;
  const PENDING = 1;
  const NETWORK_FAILURE = 2;
  const AUTH_FAILURE = 3;

  /**
   * @var TZIntellitimeSyncPolicy
   */
  private $syncPolicy;

  /**
   * @var TZIntellitimeWeekFactory
   */
  private $weekFactory;

  /**
   * @var TZIntellitimeReportStorage
   */
  private $reportStorage;

  /**
   * @var  account current user account
   */
  private $account;

//  private $weekStatus = array();

  function __construct($syncPolicy, $weekFactory, $reportStorage, $account = null) {
    tzbase_include_proto_classes();
    $this->syncPolicy = $syncPolicy;
    $this->weekFactory = $weekFactory;
    $this->reportStorage = $reportStorage;
    $this->account = $account;
  }

  /**
   * Synchronize with intellitime.
   * @return SYNC_OK on success, and NETWORK_FAILURE or AUTH_FAILURE respectively on error.
   */
  function synchronize() {
    $status = TZIntellitimeSyncController::SYNC_OK;
    while($weekDate = $this->syncPolicy->getNextWeekToSync()) {
      list($monday, $sunday) = tzintellitime_week_span($weekDate);
      $tzReports = $this->reportStorage->getTZReports($monday, $sunday);
      $week = $this->weekFactory->createWeek($weekDate, $tzReports);

      try {
        $syncResult = $week->sync();
        if(empty($syncResult)) {
          throw new TZNetworkFailureException("No sync results returned for " . $weekDate->format("o\WW"));
        }

        $storedJobs = $this->reportStorage->storeTZJobs($syncResult->tzjobs);
        $mappedReports = $this->mapReportsToJobs($syncResult->tzreports, $storedJobs);
        $changedReports = $this->filterChangedReports($mappedReports, $tzReports);
        $this->reportStorage->storeTZReports($changedReports);
        $this->syncPolicy->addWeeks($syncResult->unfinishedWeeks);

        if (isset($syncResult->exception)) {
          throw $syncResult->exception;
        }
      } catch (TZIntellitimeErrorPageException $e) {
        $this->logException('Error page when syncing', $weekDate, $e);
        $status = TZIntellitimeSyncController::NETWORK_FAILURE;
        continue;
      } catch (TZNetworkFailureException $e) {
        $this->logException('Network failure', $weekDate, $e);
        $status = TZIntellitimeSyncController::NETWORK_FAILURE;
        continue;
      } catch (TZAuthenticationFailureException $e) {
        $this->logException('Session expired', $weekDate, $e);
        $status = TZIntellitimeSyncController::AUTH_FAILURE;
        break;
      }
    }
    return $status;
  }

  private function logException($message, $weekDate, $exception) {
    $message = $weekDate->format('o\WW') . ': ' . $message;
    if (!empty($this->account->name)) {
      $message = $this->account->name . ' ' . $message;
    }
    tzbase_watchdog_with_data('tzintellitime', $message, $exception->getRawData());
  }

  private function mapReportsToJobs($tzreports, $tzjobs) {
    $jobmap = array();
    foreach($tzjobs as $tzjob) {
      $jobmap[$tzjob->intellitime_id] = $tzjob->nid;
    }

    $mappedReports = array();
    foreach($tzreports as $tzreport) {
      $tzreport_clone = clone($tzreport);
      if(!empty($tzreport->intellitime_jobid) &&
          isset($jobmap[$tzreport->intellitime_jobid])) {
        $tzreport_clone->jobid = $jobmap[$tzreport->intellitime_jobid];
      }
      unset($tzreport_clone->intellitime_jobid);
      $mappedReports[] = $tzreport_clone;
    }

    return $mappedReports;
  }

  private function filterChangedReports($modifiedReports, $originalReports) {
    $originalReportMap = array();
    foreach($originalReports as $tzreport) {
      if(!empty($tzreport->nid)) {
        $originalReportMap[$tzreport->nid] = $tzreport;
      }
    }

    $changedReports = array();
    foreach($modifiedReports as $modifiedReport) {
      if(empty($modifiedReport->nid)) {
        // New report, set as changed
        $changedReports[] = $modifiedReport;
      } else {
        $nid = $modifiedReport->nid;
        if(!isset($originalReportMap[$nid])) {
          tzbase_watchdog_with_data('tzintellitime', 'Unknown nid in filterChangedReports',
              print_r($modifiedReports, TRUE) . print_r($originalReportMap, TRUE));
          continue;
        }

        if($originalReportMap[$nid] != $modifiedReport) {
          // Report modified compared to original report
          $changedReports[] = $modifiedReport;
        }
      }
    }
    return $changedReports;
  }
}
