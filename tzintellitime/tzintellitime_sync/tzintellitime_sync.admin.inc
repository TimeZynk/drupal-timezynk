<?php

function tzintellitime_sync_admin_settings() {
  $form['tzintellitime_sync_interval_minutes'] = array(
    '#type' => 'textfield',
    '#title' => t('Minutes between synchronizations'),
    '#description' => t('Minimum interval in minutes between two synchronizations of the same user. Make sure to setup the cron interval to a equal or shorter interval than this.'),
    '#default_value' => TZINTELLITIME_SYNC_INTERVAL_MINUTES,
  );
  $form['tzintellitime_sync_users_per_run'] = array(
    '#type' => 'textfield',
    '#title' => t('Users per synchronization'),
    '#description' => t('Maximum number of users to synchronize per session'),
    '#default_value' => TZINTELLITIME_SYNC_USERS_PER_RUN,
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
  } elseif($form_state['values']['tzintellitime_sync_interval_minutes'] < 1) {
    form_set_error('tzintellitime_sync_interval_minutes', t('Synchronization interval must be greater than 1'));
  }
}