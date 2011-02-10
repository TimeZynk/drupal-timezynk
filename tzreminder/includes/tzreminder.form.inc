<?php

function tzreminder_config_form(&$form_state) {
  $form = array();

  $current_config = tzreminder_get_config();

  $form['quiet_period'] = array(
    '#tree' => TRUE,
    '#type' => 'fieldset',
    '#title' => t('Quiet period'),
    '#description' => t('Configure a time of day when no reminders are sent'),
  );
  $form['quiet_period']['start'] = array(
    '#type' => 'textfield',
    '#title' => t('From (HH:MM)'),
    '#description' => t('Start of the quiet period, written as HH:MM'),
    '#default_value' => $current_config->quiet_start,
  );
  $form['quiet_period']['end'] = array(
    '#type' => 'textfield',
    '#title' => t('To (HH:MM)'),
    '#description' => t('End of the quiet period, written as HH:MM'),
    '#default_value' => $current_config->quiet_end,
  );

  $form['delayed_reminder'] = tzsms_ui_recipient_form(
      $current_config->delay_user_policy,
      $current_config->delay_user_list
  );
  $form['delayed_reminder']['#title'] = t('Delayed reminder settings');
  $form['delayed_reminder']['delay'] = array(
    '#type' => 'textfield',
    '#title' => t('Delay in minutes'),
    '#description' => t('Delay in minutes before the reminder is sent. Leave empty for no reminder.'),
    '#weight' => -10,
    '#default_value' => $current_config->delay_minutes,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

function tzreminder_config_form_validate($form, &$form_state) {
  foreach($form_state['values']['quiet_period'] as $name => $time) {
    if(!_tzreminder_validate_time_string($time)) {
      form_set_error("quiet_period][$name", t('Could not understand time format, please use HH:MM'));
    }
  }
  if(!empty($form_state['values']['quiet_period']['start']) &&
      empty($form_state['values']['quiet_period']['end'])) {
    form_set_error('quiet_period][end', t('Please specify end of quiet period'));
  }
  $delay = trim($form_state['values']['delayed_reminder']['delay']);
  if(!_tzteminder_validate_positive_integer($delay)) {
    form_set_error('delayed_reminder][delay', t('Please specify delay in minutes as a positive integer value'));
  }
}

function tzreminder_config_form_submit($form, &$form_state) {
  $config = tzreminder_get_config();

  $config->quiet_start = $form_state['values']['quiet_period']['start'];
  $config->quiet_end = $form_state['values']['quiet_period']['end'];
  $config->delay_user_policy = $form_state['values']['delayed_reminder']['policy'];
  if(!empty($form_state['values']['delayed_reminder']['users'])) {
    $config->delay_user_list = array_keys($form_state['values']['delayed_reminder']['users']);
  }
  $config->delay_minutes = $form_state['values']['delayed_reminder']['delay'];

  tzreminder_set_config($config);
  drupal_set_message(t('New configuration stored'));
}

function _tzreminder_validate_time_string($time) {
  return empty($time) || preg_match('/^\d{2}:\d{2}$/', $time);
}

function _tzteminder_validate_positive_integer($int) {
  return empty($int) || ctype_digit($int);
}