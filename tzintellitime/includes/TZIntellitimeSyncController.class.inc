<?php

/**
 * Controller class for handling data synchronization between intellitime and timezynk
 */
class TZIntellitimeSyncController {
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
   * @var TZIntellitimeUserJobsMappingPolicy
   */
  private $userJobsMappingPolicy;

  /**
   * @var  account current user account
   */
  private $account;

  /**
   * @var TZIntellitimeLogger logger extra logger
   */
  private $logger;

  function __construct($syncPolicy, $weekFactory, $reportStorage, $userJobsMappingPolicy, $account = null) {
    tzbase_include_proto_classes();
    $this->syncPolicy = $syncPolicy;
    $this->weekFactory = $weekFactory;
    $this->reportStorage = $reportStorage;
    $this->userJobsMappingPolicy = $userJobsMappingPolicy;
    $this->account = $account;
  }

  /**
   * Synchronize with intellitime.
   * @return SYNC_OK on success, and NETWORK_FAILURE or AUTH_FAILURE respectively on error.
   */
  function synchronize() {
    $status = TZIntellitimeSyncStatus::SYNC_OK;
    while($weekDate = $this->syncPolicy->getNextWeekToSync()) {
      list($monday, $sunday) = tzintellitime_week_span($weekDate);
      $tzReports = $this->reportStorage->getTZReports($monday, $sunday);
      $week = $this->weekFactory->createWeek($weekDate, $tzReports);

      try {
        $syncResult = $week->sync();

        if(empty($syncResult)) {
          throw new TZNetworkFailureException("No sync results returned for " . $weekDate->format("o\WW"));
        }

        $storedJobs = $this->reportStorage->storeTZJobs($syncResult->getTZJobs());
        $assignmentMap = $this->mapAssignmentsToJobIDs($storedJobs, $syncResult->intellitime_assignments);
        $this->userJobsMappingPolicy->resolveMappings($monday, $sunday, $assignmentMap);
        $mappedReports = $this->mapReportsToJobs($syncResult->tzreports, $storedJobs);
        $changedReports = $this->filterChangedReports($mappedReports, $tzReports);
        $this->reportStorage->storeTZReports($changedReports);
        $this->syncPolicy->addWeeks($syncResult->unfinishedWeeks);
        if (isset($syncResult->exception)) {
          throw $syncResult->exception;
        }
      } catch (TZIntellitimeErrorPageException $e) {
        // An error caused by our sync code is business critical.
        $this->logException('Error page when syncing', $weekDate, $e, TZIntellitimeLogger::CRITICAL);
        $status = TZIntellitimeSyncStatus::NETWORK_FAILURE;
        continue;
      } catch (TZNetworkFailureException $e) {
        $this->logException('Network failure', $weekDate, $e, TZIntellitimeLogger::NOTICE);
        $status = TZIntellitimeSyncStatus::NETWORK_FAILURE;
        continue;
      } catch (TZIntellitimeInconsistentPost $e) {
        // Inconsistent posts require immediate operator intervention.
        $this->logException('Inconsistent post', $weekDate, $e, TZIntellitimeLogger::ALERT);
        $status = TZIntellitimeSyncStatus::NETWORK_FAILURE;
        continue;
      } catch (TZAuthenticationFailureException $e) {
        $this->logException('Session expired', $weekDate, $e, TZIntellitimeLogger::INFO);
        $status = TZIntellitimeSyncStatus::AUTH_FAILURE;
        break;
      } catch (TZIntellitimeServerException $e) {
        $this->logException('Server exception', $weekDate, $e, TZIntellitimeLogger::ALERT);
        $status = TZIntellitimeSyncStatus::NETWORK_FAILURE;
        continue;
      }
    }

    $this->userJobsMappingPolicy->removeExpiredMappings();

    return $status;
  }

  function registerLogger($logger) {
    $this->logger = $logger;
  }

  private function logException($message, $weekDate, $exception, $logLevel) {
    if (empty($this->logger)) {
      return;
    }
    $message = $weekDate->format('o\WW') . ': ' . $message;
    if (!empty($this->account->name)) {
      $message = $this->account->name . ' ' . $message;
    }
    $this->logger->logException($message, $exception, $logLevel);
  }

  private function logData($message, $data, $logLevel) {
    if (empty($this->logger)) {
      return;
    }
    $this->logger->logData($message, $data, $logLevel);
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

  private function mapAssignmentsToJobIDs($storedJobs, $assignments) {

    if(count($storedJobs) != count($assignments)) {
      throw new InvalidArgumentException('Could not map assignments to job IDs, mismatched length of input arrays');
    }

    $intellitime_id_map = array();
    $jobid_map = array();

    foreach ($storedJobs as $job) {
      $jobid_map[$job->nid] = $job->intellitime_id;
    }

    foreach ($assignments as $assignment) {
      $intellitime_id_map[$assignment->id] = $assignment;
    }

    foreach ($jobid_map as $jobid => $intellitime_id) {
      // To conserve memory, do mapping in place.
      $jobid_map[$jobid] = $intellitime_id_map[$intellitime_id];
    }

    return $jobid_map;
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
          $this->logData('Unknown nid in filterChangedReports',
            print_r($modifiedReports, TRUE) . print_r($originalReportMap, TRUE), TZIntellitimeLogger::DEBUG);
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
