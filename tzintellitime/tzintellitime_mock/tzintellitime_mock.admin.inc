<?php

function tzintellitime_mock_setup(&$form_state) {
  tzintellitime_include_classes();

  $form['tzintellitime_mock_username'] = array(
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#description' => t('The username to validate against during login'),
    '#default_value' => TZINTELLITIME_MOCK_USERNAME,
  );

  $form['tzintellitime_mock_password'] = array(
    '#type' => 'textfield',
    '#title' => t('Password'),
    '#description' => t('The password to validate against during login'),
    '#default_value' => TZINTELLITIME_MOCK_PASSWORD,
  );

  $form['tzintellitime_mock_delay_millis'] = array(
    '#type' => 'textfield',
    '#title' => t('Delay in milliseconds'),
    '#description' => t('Extra delay in milliseconds before returning time reports'),
    '#default_value' => TZINTELLITIME_MOCK_DELAY_MILLIS,
  );

  $form['tzintellitime_mock_reports'] = array(
    '#title' => t('Serialized timereports'),
    '#type' => 'textarea',
    '#description' => t('Serialized array of TZIntellitimeReports'),
    '#default_value' => TZINTELLITIME_MOCK_REPORTS,
  );

  $form['tzintellitime_mock_assignments'] = array(
    '#title' => t('Serialized assignments'),
    '#type' => 'textarea',
    '#description' => t('Serialized array of TZIntellitimeAssignments'),
    '#default_value' => TZINTELLITIME_MOCK_ASSIGNMENTS,
  );

  $form['tzintellitime_mock_enable_comments'] = array(
    '#title' => t('Enable comments'),
    '#type' => 'checkbox',
    '#default_value' => TZINTELLITIME_MOCK_ENABLE_COMMENTS,
  );

  return system_settings_form($form);
}
