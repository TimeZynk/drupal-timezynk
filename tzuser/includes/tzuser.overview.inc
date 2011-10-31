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
  drupal_add_js(drupal_get_path('module', 'tzuser') . '/javascript/tzuser.support_log.js');
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

function _tzuser_user_overview_operations($user_entry, $destination) {
  return l(t('edit'), "users/$user_entry->uid/edit", array('query' => $destination));
}
