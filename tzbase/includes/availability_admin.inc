<?php

define('TZBASE_AVAILABILITY_MAX_INTERVALS', 6);

function tzbase_availability_settings_form(&$form_state) {
  $form = array();

  $intervals = tzbase_availability_store()->getAvailabilityIntervals();

  $form['days_in_advance'] = array(
    '#type' => 'textfield',
    '#title' => t('Days in advance'),
    '#description' => t('Number of days in advance that availability must be reported'),
    '#default_value' => isset($intervals->days_in_advance) ? $intervals->days_in_advance : 0,
  );

  $form['intervals'] = array(
    '#type' => 'fieldset',
    '#title' => t('Availability intervals'),
    '#tree' => TRUE,
  );
  $form['intervals']['enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable intervals'),
    '#default_value' => !empty($intervals->enabled),
  );

  for ($i = 0; $i < TZBASE_AVAILABILITY_MAX_INTERVALS; $i++) {
    $default_value = '';
    if (!empty($intervals->list[$i])) {
      $interval = $intervals->list[$i];
      $default_value = ($interval->exclusive ? 'E' : '') .
        $interval->begin . '-' . $interval->end;
    }
    $form['intervals']['rows'][$i] = array(
      '#type' => 'textfield',
      '#default_value' => $default_value,
    );
  }
  $form['intervals']['rows'][0]['#title'] = t('Valid intervals');
  $form['intervals']['rows'][TZBASE_AVAILABILITY_MAX_INTERVALS - 1]['#description'] = t('Availability intervals, written as HH:MM-HH:MM');

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  return $form;
}

function tzbase_availability_settings_form_validate($form, &$form_state) {
  if (!ctype_digit($form_state['values']['days_in_advance'])) {
    form_set_error("days_in_advance", 'Days in advance must be specified as a positive integer');
  }
  for ($i = 0; $i < TZBASE_AVAILABILITY_MAX_INTERVALS; $i++) {
    $row = &$form_state['values']['intervals']['rows'][$i];
    if (empty($row)) {
      continue;
    }
    if(1 !== preg_match('/^E?\d{2}:\d{2}-\d{2}:\d{2}$/', $row)) {
      form_set_error("intervals][rows][$i", 'Enter interval as [E]HH:MM-HH:MM');
    }
  }
}

function tzbase_availability_settings_form_submit($form, &$form_state) {
  $intervals = new stdClass();
  $intervals->enabled = !empty($form_state['values']['intervals']['enabled']);
  $intervals->days_in_advance = intval($form_state['values']['days_in_advance'], 10);
  $intervals->list = array();
  for ($i = 0; $i < TZBASE_AVAILABILITY_MAX_INTERVALS; $i++) {
    $row = &$form_state['values']['intervals']['rows'][$i];
    $m = array();
    if (preg_match('/^(E?)(\d{2}:\d{2})-(\d{2}:\d{2})$/', $row, $m)) {
      $intervals->list[] = (object)array(
        'begin' => $m[2],
        'end' => $m[3],
        'exclusive' => !empty($m[1]),
      );
    }
  }
  tzbase_availability_store()->setAvailabilityIntervals($intervals);
}
