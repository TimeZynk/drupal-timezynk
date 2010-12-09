<?php

function tzintellitime_sync_admin_settings() {
  $form['tzintellitime_sync_interval_minutes'] = array(
    '#type' => 'textfield',
    '#title' => t('Minutes between synchronizations'),
    '#description' => t('Minimum interval in minutes between two synchronizations of the same user. The actual synchronization times are also affected by the current cron-interval.'),
    '#default_value' => TZINTELLITIME_SYNC_INTERVAL_MINUTES,
  );
  $form['tzintellitime_sync_users_per_run'] = array(
    '#type' => 'textfield',
    '#title' => t('Users per synchronization'),
    '#description' => t('Maximum number of users to synchronize per session'),
    '#default_value' => TZINTELLITIME_SYNC_USERS_PER_RUN,
  );
  $form['tzintellitime_sync_parallel_threads'] = array(
    '#type' => 'select',
    '#title' => t('Number of synchronization threads'),
    '#options' => array_combine(range(1,30), range(1,30)),
    '#description' => t('Total number of parallel synchronization threads to start'),
    '#default_value' => TZINTELLITIME_SYNC_PARALLEL_THREADS,
  );
  $form['tzintellitime_sync_weeks_forward'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of weeks forward in time to synchronize'),
    '#description' => t('Enter 0 for only the current week, 1 for the current week and next week, and so on.'),
    '#default_value' => TZINTELLITIME_SYNC_WEEKS_FORWARD,
  );
  $form['tzintellitime_sync_initial_sync'] = array(
    '#type' => 'radios',
    '#title' => t('Initial synchronization'),
    '#options' => array(
      TZINTELLITIME_SYNC_INITIAL_SYNC_BOTH => t('Do a full synchronization for both new users and users with a expired session.'),
      TZINTELLITIME_SYNC_INITIAL_SYNC_NEW => t('Only for new users. Users with expired session are synchronized at the next scheduled synchronization.'),
      TZINTELLITIME_SYNC_INITIAL_SYNC_NONE => t('Never do initial synchronization, always defer to the next scheduled synchronization.'),
    ),
    '#description' => t('Configure how to handle synchronization on login'),
    '#default_value' => TZINTELLITIME_SYNC_INITIAL_SYNC,
  );

  return system_settings_form($form);
}

/**
 * Checks that the settings we are passed includes
 *  - a valid base url, with scheme https.
 * @param Array $form
 * @param Array $form_state
 */
function tzintellitime_sync_admin_settings_validate($form, &$form_state) {
  if(!is_numeric($form_state['values']['tzintellitime_sync_interval_minutes'])) {
    form_set_error('tzintellitime_sync_interval_minutes', t('Invalid synchronization interval, enter number of minutes.'));
  } elseif($form_state['values']['tzintellitime_sync_interval_minutes'] < 0) {
    form_set_error('tzintellitime_sync_interval_minutes', t('Synchronization interval must be greater than 0'));
  }

  if(!is_numeric($form_state['values']['tzintellitime_sync_weeks_forward'])) {
    form_set_error('tzintellitime_sync_weeks_forward', t('Enter number of weeks as a positive integer.'));
  } elseif($form_state['values']['tzintellitime_sync_weeks_forward'] < 0) {
    form_set_error('tzintellitime_sync_weeks_forward', t('Must synchronize at least the current week'));
  }
}