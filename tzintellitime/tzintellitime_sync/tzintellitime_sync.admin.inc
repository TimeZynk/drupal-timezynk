<?php

function tzintellitime_sync_admin_settings() {
  $form['tzintellitime_sync_interval_minutes'] = array(
    '#type' => 'textfield',
    '#title' => t('Minutes between synchronizations'),
    '#description' => t('Minimum interval in minutes between two synchronizations of the same user. The actual synchronization times are also affected by the current cron-interval.'),
    '#default_value' => TZINTELLITIME_SYNC_INTERVAL_MINUTES,
  );
  $form['tzintellitime_sync_users_per_thread'] = array(
    '#type' => 'textfield',
    '#title' => t('Maximum users per synchronization thread'),
    '#description' => t('Maximum number of users to synchronize per thread'),
    '#default_value' => TZINTELLITIME_SYNC_USERS_PER_THREAD,
  );
  $form['tzintellitime_sync_weeks_forward'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of weeks forward in time to synchronize'),
    '#description' => t('Enter 0 for only the current week, 1 for the current week and next week, and so on.'),
    '#default_value' => TZINTELLITIME_SYNC_WEEKS_FORWARD,
  );
  $form['tzintellitime_sync_weeks_backward'] = array(
    '#type' => 'textfield',
    '#title' => t('Number of weeks backward in time to synchronize'),
    '#description' => t('Enter 0 for only the current week, 1 for the current week and previous week, and so on.'),
    '#default_value' => TZINTELLITIME_SYNC_WEEKS_BACKWARD,
  );
  $form['tzintellitime_sync_full_sync_probability'] = array(
    '#type' => 'textfield',
    '#title' => 'Probability for a full synchronization',
    '#description' => 'As a decimal number between 0 and 1. At 0 it will never do a full sync and at 1 it will always do a full sync.',
    '#default_value' => TZINTELLITIME_SYNC_FULL_SYNC_PROBABILITY,
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

  $availability_set = array(
    '#type' => 'fieldset',
    '#title' => 'Availability settings',
    '#collapsible' => TRUE,
  );

  $availability_set['tzintellitime_sync_availability_day'] = array(
    '#type' => 'textfield',
    '#title' => 'Which hours to consider as day',
    '#description' => 'Example: "07:00-12:00" synchronizes availability reports between 07:00 and 12:00 to Intellitime as the day checkbox.',
    '#default_value' => TZINTELLITIME_SYNC_AVAILABILITY_DAY
  );

  $availability_set['tzintellitime_sync_availability_evening'] = array(
  	'#type' => 'textfield',
  	'#title' => 'Which hours to consider as evening',
  	'#description' => 'Example: "12:00-18:00" synchronizes availability reports between 12:00 and 18:00 to Intellitime as the evening checkbox.',
  	'#default_value' => TZINTELLITIME_SYNC_AVAILABILITY_EVENING
  );

  $availability_set['tzintellitime_sync_availability_night'] = array(
  	'#type' => 'textfield',
  	'#title' => 'Which hours to consider as night',
  	'#description' => 'Example: "18:00-23:00" synchronizes availability reports between 18:00 and 23:00 to Intellitime as the night checkbox.',
  	'#default_value' => TZINTELLITIME_SYNC_AVAILABILITY_NIGHT
  );

  $form['availability_set'] = $availability_set;

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

  if(!ctype_digit($form_state['values']['tzintellitime_sync_weeks_forward'])) {
    form_set_error('tzintellitime_sync_weeks_forward', t('Enter number of weeks as a positive integer.'));
  }

  if(!ctype_digit($form_state['values']['tzintellitime_sync_weeks_backward'])) {
    form_set_error('tzintellitime_sync_weeks_backward', t('Enter number of weeks as a positive integer.'));
  }

  if(1 !== preg_match('/^\d{2}:\d{2}-\d{2}:\d{2}$/', $form_state['values']['tzintellitime_sync_availability_day'])) {
    form_set_error('tzintellitime_sync_availability_day', 'Enter time span as HH:MM-HH:MM');
  }
  if(1 !== preg_match('/^\d{2}:\d{2}-\d{2}:\d{2}$/', $form_state['values']['tzintellitime_sync_availability_evening'])) {
    form_set_error('tzintellitime_sync_availability_evening', 'Enter time span as HH:MM-HH:MM');
  }
  if(1 !== preg_match('/^\d{2}:\d{2}-\d{2}:\d{2}$/', $form_state['values']['tzintellitime_sync_availability_night'])) {
    form_set_error('tzintellitime_sync_availability_night', 'Enter time span as HH:MM-HH:MM');
  }
}
