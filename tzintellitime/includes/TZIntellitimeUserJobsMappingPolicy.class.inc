<?php

class TZIntellitimeUserJobsMappingPolicy {
  /**
   * @var TZUserJobsMapper
   */
  private $mapper;
  private $account;
  private $mappings;

  function __construct($mapper, $account) {
    $this->mapper = $mapper;
    $this->account = $account;
  }

  /**
   * Sets up User to Job mappings showing which intellitime assignments a given user may report on.
   * @param DateTime $start_date
   * @param DateTime $end_date
   * @param Array $assignments an map containing TZIntellitimeAssignments, keyed on jobid.
   */
  function resolveMappings($start_date, $end_date, $assignments) {
    if (!$this->mappings) {
      $this->loadMappings();
    }

    foreach ($assignments as $jobid => $assignment) {
      if (!$assignment->mayCreateReport()) {
        continue;
      }

      $mapping = $this->mapper->createMapping();
      $mapping->setUserId($this->account->uid);
      $mapping->setJobId($jobid);
      $mapping->setStartTime($start_date);
      $mapping->setEndTime($end_date);

      if (!$this->mappingExists($mapping)) {
        $mapping->save();
        $this->mappings[$jobid][] = $mapping;
      }
    }

    foreach ($this->mappings as $jobid => $mappings) {
      if (empty($assignments[$jobid]) || !$assignments[$jobid]->mayCreateReport()) {
        // No matching assignment, delete?
        foreach ($mappings as $index => $mapping) {
          if ($mapping->getStartTime() == $start_date &&
              $mapping->getEndTime() == $end_date) {
             $this->mapper->deleteMapping($mapping->getId());
             unset($this->mappings[$jobid][$index]);
          }
        }
      }
    }
  }

  private function mappingExists($mapping) {
    $jobid = $mapping->getJobId();
    if (empty($this->mappings[$jobid])) {
      return FALSE;
    }
    foreach ($this->mappings[$jobid] as $existing_mapping) {
      if ($mapping->getStartTime() == $existing_mapping->getStartTime() &&
          $mapping->getEndTime() == $existing_mapping->getEndTime()) {
        return TRUE;
      }
    }
    return FALSE;
  }

  private function loadMappings() {
    $mappings = $this->mapper->find($this->account->uid);
    $this->mappings = array();
    foreach ($mappings as $mapping) {
      $jobid = $mapping->getJobId();
      if (empty($this->mappings[$jobid])) {
        $this->mappings[$jobid] = array();
      }
      $this->mappings[$jobid][] = $mapping;
    }
  }
}
