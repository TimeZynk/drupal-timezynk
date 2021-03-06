<?php

function tzintellitime_admin_settings() {
  $form['tzintellitime_base_url'] = array(
    '#type' => 'textfield',
    '#title' => t('Intellitime base URL'),
    '#size' => 100,
    '#description' => t('Customer specific URL for the portal login page. Using HTTPS is strongly recommended!'),
    '#default_value' => TZINTELLITIME_BASE_URL,
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
  } else if ($result['scheme'] != "https" && $result['scheme'] != "http") {
    form_set_error('tzintellitime_base_url', t('We can only handle HTTP or HTTPS connections to intellitime'));
  }
}