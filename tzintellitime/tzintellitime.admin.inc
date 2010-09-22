<?php

function tzintellitime_admin_settings() {
  $form['tzintellitime_base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Intellitime base URL'),
    '#description' => t('Customer specific HTTPS URL for the portal login page'),
    '#default_value' => variable_get('tzintellitime_base_url', ''),
  );

  return system_settings_form($form);
}

/**
 * Checks that the settings we are passed includes
 *  - a valid base url, with scheme https.
 * @param Array $form
 * @param Array $form_state
 */
function tzintellitime_admin_settings_validate($form, &$form_state) {
  @$result = parse_url($form_state['values']['tzintellitime_base_url']);
  if (!$result || !array_key_exists('scheme', $result) || !array_key_exists('host', $result)) {
    form_set_error('tzintellitime_base_url', t('Invalid URL, please enter a correct intelliplan login URL.'));
  } else if ($result['scheme'] != "https") {
    //form_set_error('tzintellitime_base_url', t('We only allow HTTPS (SSL) connections to intelliplan for security reasons.'));
  }
}