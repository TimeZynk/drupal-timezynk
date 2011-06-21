<?php

class TZUserJobsMapper {
  private $dbWrapper;

  function __construct($dbWrapper) {
    $this->dbWrapper = $dbWrapper;
  }

  public function find($uid, $jobid = NULL) {
    $mappings = array();

    $cursor = NULL;
    if ($jobid === NULL) {
      $cursor = $this->dbWrapper->query('SELECT * FROM {tzusers_tzjobs} WHERE uid = %d ORDER BY id', $uid);
    } else {
      $cursor = $this->dbWrapper->query('SELECT * FROM {tzusers_tzjobs} WHERE uid = %d AND jobid = %d ORDER BY id', $uid, $jobid);
    }
    while ($row = $this->dbWrapper->fetchObject($cursor)) {
      $mapping = new TZUserJobMapping($row);
      $mapping->setDBWrapper($this->dbWrapper);
      $mappings[] = $mapping;
    }
    return $mappings;
  }

  public function findAll() {
    $mappings = array();

    $cursor = $this->dbWrapper->query('SELECT * FROM {tzusers_tzjobs} ORDER BY id', $uid);
    while ($row = $this->dbWrapper->fetchObject($cursor)) {
      $mapping = new TZUserJobMapping($row);
      $mapping->setDBWrapper($this->dbWrapper);
      $mappings[] = $mapping;
    }
    return $mappings;
  }

  public function createMapping() {
    $mapping = new TZUserJobMapping();
    $mapping->setDBWrapper($this->dbWrapper);
    return $mapping;
  }

  public function deleteMapping($id) {
    $this->dbWrapper->delete('tzusers_tzjobs', $id);
  }

  public function userMayCreateReport($uid, $jobid, $begintime) {
    $mappings = $this->find($uid, $jobid);
    foreach ($mappings as $mapping) {
      if ($mapping->mayCreateReport($uid, $jobid, $begintime)) {
        return TRUE;
      }
    }
    return FALSE;
  }

  public function deleteAllByJobId($jobid) {
    if (empty($jobid)) {
      throw new InvalidArgumentException('must supply jobid');
    }
    $this->dbWrapper->query('DELETE FROM {tzusers_tzjobs} WHERE jobid = %d', $jobid);
  }

  public function deleteAllByUserId($uid) {
    if (empty($uid)) {
      throw new InvalidArgumentException('must supply jobid');
    }
    $this->dbWrapper->query('DELETE FROM {tzusers_tzjobs} WHERE uid = %d', $uid);
  }
}
