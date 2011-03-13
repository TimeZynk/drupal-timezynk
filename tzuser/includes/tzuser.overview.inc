<?php

function tzuser_user_overview_headers() {
  return array(
    array(),
    t('Status'),
    t('Username'),
    t('Full name'),
    t('Mobile'),
    t('Due reports'),
    t('Last login'),
    array(),
  );
}

function tzuser_user_overview(&$form_state) {
  $form = array();
  $now = time();

  drupal_add_css(drupal_get_path('module', 'tzuser') . '/style/tzuser.css');
  drupal_add_js(drupal_get_path('module', 'tzuser') . '/javascript/tzuser.overview.js');

  // Get return destination for action links
  $destination = drupal_get_destination();

  $form['filter'] = array(
    '#type' => 'fieldset',
  );

  $status_options = array(
    TZUserStatus::GREEN => t('Employees with all time reported'),
    TZUserStatus::YELLOW => t('Employees with late reports'),
    TZUserStatus::RED => t('Employees inactive for more than @redlimit', array('@redlimit' => format_interval(TZUSER_OVERVIEW_RED_LIMIT))),
    TZUserStatus::GREY => t('Employees who have never logged in'),
  );

  $form['filter']['status'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Show status'),
    '#description' => t('Select which statuses you want to show'),
    '#options' => $status_options,
    '#value' => array_filter($status_options),
  );

  $form['filter']['manager'] = tzuser_select_manager_field($GLOBALS['user']->uid);
  $form['filter']['manager']['#title'] = t('Show with manager');
  $form['filter']['manager']['#description'] = t('Select which managers employees to show');

  $form['filter']['date'] = array(
    '#type' => 'date_popup',
    '#date_format' => 'Y-m-d',
    '#title' => t('Show status until'),
    '#description' => t('Show the status until and including date'),
  );

  return $form;
}

function tzuser_overview_ajax($manager = 0) {
  $now = time();

  if (!empty($_GET['date']) && preg_match('/\d{4}-\d{2}-\d{2}/', $_GET['date'])) {
    $date = date_make_date($_GET['date'] . 'T23:59:59');
    $now = $date->format('U');
  }

  $sql = 'SELECT * FROM {users} u INNER JOIN {tzuser} tzu ON tzu.uid = u.uid WHERE u.uid > 1 AND u.status = 1';
  if (!empty($manager)) {
    $sql .= ' AND tzu.manager = %d';
  }
  $result = db_query($sql, $manager);

  $overview = array();
  while($user_entry = db_fetch_object($result)) {
    $status = tzuser_get_user_status($user_entry, $now);

    $checkbox = array(
      '#type' => 'checkbox',
      '#name' => "selected_users[$user_entry->uid]",
      '#return_value' => $user_entry->uid,
      '#id' => "selected_users_$user_entry->uid",
      '#value' => 0,
      '#parents' => array(''),
    );

    $overview[] = array(
      'checkbox' => drupal_render($checkbox),
      'status' => theme('traffic_light', $status->getStatusCode($now)),
      'status_value' => $status->getStatusCode($now),
      'name' => $user_entry->name,
      'fullname' => $user_entry->fullname,
      'mobile' => $user_entry->mobile,
      'due_count' => theme('due_reports_count', $status->getNumberOfDueReports()),
      'login' => theme('time_ago', $user_entry->login),
      'login_value' => $user_entry->login,
      'operations' => user_edit_access($user_entry) ? l(t('edit'), "user/$user_entry->uid/edit", array('query' => 'destination=tzuser')) : '',
    );
  }
  return drupal_json($overview);
}

function _tzuser_user_overview_operations($user_entry, $destination) {
  return l(t('edit'), "users/$user_entry->uid/edit", array('query' => $destination));
}