<?php

function tzintellitime_mock_setup_reports(&$form_state) {

  $form['tzintellitime_mock_delay_millis'] = array(
    '#type' => 'textfield',
    '#title' => t('Delay in milliseconds'),
    '#description' => t('Extra delay in milliseconds before returning time reports'),
    '#default_value' => TZINTELLITIME_MOCK_DELAY_MILLIS,
  );

  $form['tzintellitime_mock_enable_comments'] = array(
    '#title' => t('Enable comments'),
    '#type' => 'checkbox',
    '#default_value' => TZINTELLITIME_MOCK_ENABLE_COMMENTS,
  );

  return system_settings_form($form);
}
