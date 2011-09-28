<?php

class TZReportsTimeReports {
  const GROUP_BY_ASSIGNED_TO = 0;
  const GROUP_BY_JOBID = 1;

  private $headers;
  private $group_by;
  private $destination;
  private $show_comments;

  private $group_rows = array();
  private $group_total = array();
  private $group_title = array();
  private $total_duration = 0;

  function __construct($group_by, $destination, $show_comments) {
    $this->headers = array(
      array(),
      t('Name'),
      t('Assignment'),
      t('Date'),
      array('data' => t('Start'), 'class' => 'timereports-align-right'),
      array('data' => t('End'), 'class' => 'timereports-align-right'),
      array('data' => t('Break'), 'class' => 'timereports-align-right'),
      array('data' => t('Total'), 'class' => 'timereports-align-right'),
      array(),
    );

    $this->group_by = $group_by;
    $this->destination = $destination;
    $this->show_comments = $show_comments;
  }

  function addReport($report) {
    $body = $report->assignment_path;
    if ($this->show_comments && !empty($report->body)) {
      $body .= '<br/><em>' . check_plain($report->body) . '</em>';
    }

    $row = array(
      theme('timereport_status', $report),
      l($report->fullname, 'user/' . $report->assignedto),
      $body,
      $report->begin_date->format('Y-m-d'),
      array('data' => $report->begin_date->format('H:i'), 'class' => 'timereports-align-right'),
      array('data' => $report->end_date->format('H:i'), 'class' => 'timereports-align-right'),
      array('data' => tzbase_format_duration($report->breakduration), 'class' => 'timereports-align-right'),
      array('data' => tzbase_format_duration($report->totalduration), 'class' => 'timereports-align-right'),
      l(t('edit'), "node/$report->nid/edit", array('query' => $this->destination)),
    );

    $group_key = $report->assignedto;
    $group_title = $report->fullname;
    if ($this->group_by === TZReportsTimeReports::GROUP_BY_JOBID) {
      $group_key = $report->jobid;
      $group_title = $report->assignment_path;
    }
    $this->group_rows[$group_key][] = $row;
    if (empty($this->group_total[$group_key])) {
      $this->group_total[$group_key] = $report->totalduration;
    } else {
      $this->group_total[$group_key] += $report->totalduration;
    }
    $this->group_title[$group_key] = $group_title;
    $this->total_duration += $report->totalduration;
  }

  function getTableHeaders() {
    return $this->headers;
  }

  function getTableRows() {
    $rows = array();

    natcasesort($this->group_title);
    foreach ($this->group_title as $group_key => $group_title) {
      array_splice($rows, count($rows), 0, $this->group_rows[$group_key]);
      if (count($this->group_title) > 1) {
        $rows[] = $this->totalRow(
            t('Subtotal for <em>!path</em>' , array('!path' => $group_title)),
            $this->group_total[$group_key]
        );
      }
    }

    $rows[] = $this->totalRow(t('Total'), $this->total_duration, 'timereports-total');
    return $rows;
  }

  private function totalRow($label, $duration, $class = 'timereports-subtotal') {
    return array(
      'class' => "$class-row",
      'data' => array(
        array('class' => "$class-field"),
        array(
          'data' => $label,
          'colspan' => count($this->headers) - 3,
          'class' => "$class-field label"
        ),
        array(
          'data' => tzbase_format_duration($duration),
          'class' => "$class-field duration timereports-align-right"
        ),
        array('class' => "$class-field"),
        array('class' => "$class-field"),
      ),
    );
  }
}