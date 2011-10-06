<?php

define('TZUSER_OVERVIEW_REFRESH_RATE', variable_get('tzuser_overview_refresh_rate', 30));

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

  drupal_add_css(drupal_get_path('module', 'tzuser') . '/style/tzuser.css');
  drupal_add_js(drupal_get_path('module', 'tzuser') . '/javascript/tzuser.overview.js');
  drupal_add_js(array(
	  'tzuser' => array('OVERVIEW_REFRESH_RATE' => TZUSER_OVERVIEW_REFRESH_RATE)
    ), 'setting');

  $form['filter'] = array(
    '#type' => 'fieldset',
  );

  $status_options = array(
    TZUserStatus::GREEN => t('Employees with all time reported'),
    TZUserStatus::YELLOW => t('Employees that have or might have late reports'),
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
  $form['filter']['manager']['#options'][0] = '-- ' . t('All') . ' --';

  $form['filter']['date'] = array(
    '#type' => 'date_popup',
    '#date_format' => 'Y-m-d',
    '#title' => t('Show status until'),
    '#description' => t('Show the status until and including date'),
  );

  $form['filter']['progress'] = array(
    '#value' => '<div class="ahah-progress"></div>',
  );

  $form['info'] = array(
    '#value' => t('<strong>Tip:</strong> Hold the SHIFT key to select many employees'),
    '#suffix' => '<br/>',
  );

  return $form;
}

function tzuser_overview_ajax() {
  $date = date_make_date('now');

  // Handle date setting
  if (!empty($_GET['date']['date']) && preg_match('/\d{4}-\d{2}-\d{2}/', $_GET['date']['date'])) {
    $date = date_make_date($_GET['date']['date'] . 'T00:00:00');
  } else {
    $date->setTime(0, 0, 0);
  }
  // The date is up to and including.
  $date->modify('+1 day');

  // Handle manager
  $manager = 0;
  if (!empty($_GET['manager']) && is_numeric($_GET['manager'])) {
    $manager = intval($_GET['manager']);
  }

  // Handle status filter
  $status_filter = array();
  if (!empty($_GET['status'])) {
    foreach ($_GET['status'] as $status => $value) {
      $status_filter[$status] = TRUE;
    }
  }

  $sql = 'SELECT * FROM {users} u INNER JOIN {tzuser} tzu ON tzu.uid = u.uid WHERE u.uid > 1 AND u.status = 1';
  if (!empty($manager)) {
    $sql .= ' AND tzu.manager = %d';
  }
  $result = db_query($sql, $manager);

  $overview = array();
  while($user_entry = db_fetch_object($result)) {
    $status = tzuser_get_user_status($user_entry, $date->format('U'));
    $status_code = $status->getStatusCode();

    if (empty($status_filter[$status_code])) {
      continue;
    }

    $checkbox = array(
      '#type' => 'checkbox',
      '#name' => "selected_users[$user_entry->uid]",
      '#return_value' => $user_entry->uid,
      '#id' => "selected_users_$user_entry->uid",
      '#value' => 0,
      '#parents' => array(''),
    );

    $operations = array();
    if (user_edit_access($user_entry)) {
      $operations[] = l(t('edit'), "user/$user_entry->uid/edit", array('query' => 'destination=tzuser'));
    }
    if (user_access('administer site configuration')) {
      $operations[] = l(t('log'), '', array('fragment' => ' ', 'external' => TRUE, 'attributes' => array('class' => 'edit-log-link')));
    }

    $overview[] = array(
      'checkbox' => drupal_render($checkbox),
      'status' => theme('traffic_light', $status_code),
      'status_value' => $status_code,
      'name' => l($user_entry->name, "user/$user_entry->uid", array('query' => 'destination=tzuser')),
      'name_value' => $user_entry->name,
      'fullname' => $user_entry->fullname,
      'mobile' => empty($user_entry->mobile) ? '' : $user_entry->mobile,
      'due_count' => theme('due_reports_count', $status->getNumberOfDueReports()),
      'login' => theme('time_ago', $user_entry->login),
      'login_value' => $user_entry->login,
      'operations' => implode(' | ', $operations),
    );
  }
  return drupal_json($overview);
}

function _tzuser_user_overview_operations($user_entry, $destination) {
  return l(t('edit'), "users/$user_entry->uid/edit", array('query' => $destination));
}
