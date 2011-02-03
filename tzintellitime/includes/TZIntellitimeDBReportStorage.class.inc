<?php

class TZIntellitimeDBReportStorage implements TZIntellitimeReportStorage {
  private $account;

  public function __construct($account) {
    $this->account = $account;
  }

  public function getTZReports($from, $to) {
    $reports = array();
    $result = db_query('SELECT n.nid FROM {node} n INNER JOIN {tzreport} t ON n.vid = t.vid WHERE t.assignedto = %d AND t.begintime >= %d AND t.begintime < %d AND t.flags != 255 AND n.status = 1',
                       $this->account->uid, $from->format('U'), $to->format('U'));
    while($nid = db_result($result)) {
      $node = node_load($nid, NULL, TRUE);
      if(!empty($node->intellitime_id)) {
        $reports[] = $node;
      }
    }
    return $reports;
  }

  public function storeTZJobs($tzjobs) {
    $storedTZJobs = array();
    foreach($tzjobs as $tzjob) {
      $storedTZJobs[] = $this->storeTZJob($tzjob);
    }
    return $storedTZJobs;
  }

  public function storeTZReports($tzreports) {
    foreach($tzreports as $tzreport) {
      $this->storeNode($tzreport);
    }
  }

  private function storeTZJob($tzjob) {
    $node = tzintellitime_node_load_by_intellitime_id($tzjob->intellitime_id);
    if(!$node) {
      return $this->storeNode($tzjob);
    }

    $originalNode = clone($node);
    $this->copyTZJobPropertiesToNode($node, $tzjob);
    if($node != $originalNode) {
      $node = $this->storeNode($node);
    }

    return $node;
  }

  private function copyTZJobPropertiesToNode(&$node, $tzjob) {
    $node->title = $tzjob->title;
    $node->jobcode = $tzjob->jobcode;
    $node->flags = $tzjob->flags;
    $node->intellitime_local_changes = 0;
  }

  private function storeNode($node) {
    if(empty($node->intellitime_local_changes)) {
      $node->intellitime_synchronized = TRUE;
    } else {
      unset($node->intellitime_synchronized);
    }

    $operation = empty($node->nid) ? 'Inserted' : 'Updated';
    if($node->flags == TZFlags::DELETED) {
      $operation = 'Deleted';
    }

    if(empty($node->uid)) {
      $node->uid = $this->account->uid;
      $node->name = $this->account->name;
    }

    $node = node_submit($node);
    node_save($node);

    tzbase_watchdog_node_operation('tzintellitime', $node, $operation, $this->account);

    return $node;
  }
}