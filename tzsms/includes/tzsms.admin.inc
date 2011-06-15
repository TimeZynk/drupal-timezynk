<?php

/**
 * Provide a simple form to set the default country code
 * @ingroup forms
 * @see system_settings_form()
 */
function tzsms_admin_settings($form_state) {
  $form = array(
    'tzsms_backend' => array(
      '#type' => 'select',
      '#title' => t('SMS Backend'),
      '#default_value' => TZSMS_BACKEND,
      '#options' => SmsBackendFactory::all_backends(),
    ),
    'tzsms_default_country_code' => array(
      '#type' => 'textfield',
      '#title' => t('Default country code for this site'),
      '#default_value' => TZSMS_DEFAULT_COUNTRY_CODE,
      '#description' => t('The country code will be used to send installation SMS to the users registered on this site'),
    ),
    'tzsms_default_sender' => array(
      '#type' => 'textfield',
      '#title' => t('Default SMS sender'),
      '#default_value' => TZSMS_DEFAULT_SENDER,
      '#description' => t('Default SMS sender ID, alphanumeric max 11 characters.')
    ),
    'tzsms_install_sms_template' => array(
      '#type' => 'textarea',
      '#title' => t('The content of the SMS'),
      '#default_value' => variable_get('tzsms_install_sms_template', tzsms_install_sms_template()),
      '#description' => t('The available variables are: !url, !username and !password'),
    ),
    'tzsms_install_sms_no_password_template' => array(
      '#type' => 'textarea',
      '#title' => t('The content of the SMS when keeping the same password'),
      '#default_value' => variable_get('tzsms_install_sms_no_password_template', tzsms_install_sms_no_password_template()),
      '#description' => t('The available variables are: !url and !username'),
    ),
    'tzsms_debug_sms_receiver' => array(
      '#type' => 'textfield',
      '#title' => t('Reciever of debug log SMS'),
      '#default_value' => TZSMS_DEBUG_SMS_RECEIVER,
      '#description' => t('Enter a mobile number unto which debug SMSes will be delivered'),
    ),
  );

  return system_settings_form($form);
}


/**
 * Validate the form to check that admin has entered correct TimeZynk
 * SMS settings.
 * @param $form submitted form
 * @param $form_state submitted form state
 */
function tzsms_admin_settings_validate(&$form, &$form_state) {
  /* Country code must be all numeric and contain a maximum of 4 digits since
   * a couple of countries like Barbados and Jamaica have 4 digit country codes */
  $country_code = $form_state['values']['tzsms_default_country_code'];
  if(strlen($country_code) > 4 || !ctype_digit($country_code)) {
    form_set_error('tzsms_default_country_code', t('You must enter a numeric country code (e.g. 46 for Sweden)!'));
  }

  if(!preg_match('/^[A-Za-z0-9]{1,11}$/', $form_state['values']['tzsms_default_sender'])) {
    form_set_error('tzsms_default_sender', t('Sender must be alphanumeric with maximum 11 characters'));
  }
}