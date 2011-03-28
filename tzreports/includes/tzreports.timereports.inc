<?php

function tzreports_timereports(&$form_state) {
  drupal_add_css(drupal_get_path('module', 'tzreports') . '/style/tzreports.css');
  drupal_add_js(drupal_get_path('module', 'tzreports') . '/javascript/tzreports.timereports.js');

  $now = tzbase_make_date();
  $form['date'] = tzreport_select_date_field($now);
  $form['users'] = tzreport_select_users_field();
  $form['assignment'] = tzreport_select_assignment_field();
  $form['group_by'] = tzreport_select_group_by_field();
  $form['show_comments'] = tzreport_show_comments_field();
  $form['progress'] = array(
    '#value' => '<div class="ahah-progress"></div>',
  );
  $form['data'] = array(
    '#value' => '<div id="timereports-wrapper"></div>',
  );
  return $form;
}

function tzreports_timereports_ajax() {
  $group_by = TZReportsTimeReports::GROUP_BY_ASSIGNED_TO;
  if (isset($_POST['group_by'])) {
    $group_by = intval($_POST['group_by'], 10);
  }
  $summary = new TZReportsTimeReports(
      $group_by,
      'destination=timereports',
      isset($_POST['show_comments'])
  );

  $query = 'SELECT * FROM {node} n ' .
      'INNER JOIN {node_revisions} rev ON n.vid = rev.vid ' .
      'INNER JOIN {tzreport} tzr ON n.vid = tzr.vid ' .
      'INNER JOIN {tzuser} tzu ON tzr.assignedto = tzu.uid ' .
      'WHERE tzr.flags != 255';
  $query_args = array();

  if (!empty($_POST['start_date']['date'])) {
    $date = date_make_date($_POST['start_date']['date'] . 'T00:00:00');
    $query .= ' AND tzr.begintime >= %d';
    $query_args[] = $date->format('U');
  }

  if (!empty($_POST['end_date']['date'])) {
    $date = date_make_date($_POST['end_date']['date'] . 'T23:59:59');
    $query .= ' AND tzr.begintime <= %d';
    $query_args[] = $date->format('U');
  }

  if (!empty($_POST['users'])) {
    $query .= ' AND(';
    $users_query = array();
    foreach ($_POST['users'] as $uid) {
      $users_query[] = 'tzr.assignedto = %d';
      $query_args[] = $uid;
    }
    $query .= implode(' OR ', $users_query) . ')';
  }

  if (!empty($_POST['assignment'])) {
    $assignment_trail = explode(':', $_POST['assignment']);
    $top_assignment = end($assignment_trail);
    if (!empty($top_assignment) && ctype_digit($top_assignment)) {
      $children = tzbase_children_for_jobid($top_assignment);
      $query .= ' AND (';
      $jobid_query = array();
      foreach ($children as $jobid) {
        $jobid_query[] = 'tzr.jobid = %d';
        $query_args[] = $jobid;
      }
      $query .= implode(' OR ', $jobid_query) . ')';
    }
  }

  $query .= ' ORDER BY tzr.begintime';
  $result = db_query(db_rewrite_sql($query), $query_args);

  while ($report = db_fetch_object($result)) {
    $report->begin_date = tzbase_make_date($report->begintime);
    $report->end_date = tzbase_make_date($report->endtime);
    $report->assignment_path = theme('assignment_path', $report->jobid);
    $summary->addReport($report);
  }
  print theme('table', $summary->getTableHeaders(), $summary->getTableRows());
}

function tzreport_select_date_field($now) {
  $default_start = clone($now);
  $day_of_month = intval($default_start->format('d')) - 1;
  $default_start->modify("-$day_of_month days");

  $default_end = clone($default_start);
  $default_end->modify('+1 month');
  $default_end->modify('-1 day');

  $form = array(
    '#prefix' => '<div id="edit-date-wrapper">',
    '#suffix' => '</div>',
  );
  $form['start_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('Start date'),
    '#date_format' => 'Y-m-d',
    '#default_value' => $default_start->format('Y-m-d'),
  );
  $form['end_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('End date'),
    '#date_format' => 'Y-m-d',
    '#default_value' => $default_end->format('Y-m-d'),
  );

  return $form;
}

function tzreport_select_assignment_field() {
  $joblist = array(
    'nid:0' => '-- ' . t('All assignments') . ' --',
  );
  $joblist += tzbase_list_jobs();

  return array(
    '#type' => 'select',
    '#title' => t('Assignment'),
    '#options' => $joblist,
    '#default_value' => 'nid:0',
  );
}

function tzreport_select_users_field() {
  $userlist = tzuser_list_users();
  $useroptions = array();
  foreach($userlist as $account) {
    $useroptions[$account->uid] = $account->tzuser['fullname'];
  }
  return array(
    '#type' => 'select',
    '#title' => t('Employees'),
    '#multiple' => TRUE,
    '#options' => $useroptions,
    '#size' => 7,
  );
}

function tzreport_select_group_by_field() {
  return array(
    '#type' => 'radios',
    '#title' => t('Group by'),
    '#options' => array(
      TZReportsTimeReports::GROUP_BY_ASSIGNED_TO => t('Employees'),
      TZReportsTimeReports::GROUP_BY_JOBID => t('Assignments'),
    ),
    '#default_value' => TZReportsTimeReports::GROUP_BY_ASSIGNED_TO,
    '#prefix' => '<div id="edit-group-wrapper">',
    '#suffix' => '</div>',
  );
}

function tzreport_show_comments_field() {
  return array(
    '#type' => 'checkboxes',
    '#title' => t('Comments'),
    '#options' => array(
      t('Show comments'),
    ),
    '#default_value' => array(0),
    '#prefix' => '<div id="edit-show-comments-wrapper">',
    '#suffix' => '</div>',
  );
}