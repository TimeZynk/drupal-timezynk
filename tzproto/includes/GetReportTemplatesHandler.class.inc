<?php

class GetReportTemplatesHandler extends CommandHandler {
  private $db;

  public function __construct($protocol_version, $db) {
    parent::__construct($protocol_version);
    $this->db = $db;
  }

  public function handle($cmd, &$result) {
    global $user;
    if (!user_access('create tzreport content from template')) {
      throw new TZProtoException('access denied', 403);
    }

    $templates_result = new TZGetReportTemplatesResult();
    $sql = 'SELECT n.nid FROM {node} n INNER JOIN {tzreport} tzr ON n.vid = tzr.vid WHERE tzr.assignedto = %d ORDER BY n.created DESC LIMIT 3';
    $db_result = db_query($sql, $user->uid);
    while($data = db_fetch_object($db_result)) {
      $node = node_load($data->nid, NULL, TRUE);
      $tzreport = $templates_result->add_report();
      tzproto_fill_from_node($tzreport, $node, $this->protocol_version);
    }
    $result->set_get_report_templates_result($templates_result);
  }
}
