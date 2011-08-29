<?php

function tzstats_administer_pricing() {
  $form['timezynk_day_price'] = array(
    '#type' => 'textfield',
    '#title' => t('Price per day'),
    '#size' => 10,
    '#description' => t('Price per user per day in preferred currency'),
    '#default_value' => TIMEZYNK_DAY_PRICE,
  );
  $form['timezynk_month_price'] = array(
    '#type' => 'textfield',
    '#title' => t('Price per month'),
    '#size' => 10,
    '#description' => t('Maximum price per month per user'),
    '#default_value' => TIMEZYNK_MONTH_PRICE,
  );
  $form['timezynk_availability_price'] = array(
    '#type' => 'textfield',
    '#title' => t('Price per month for availability users'),
    '#size' => 10,
    '#default_value' => TIMEZYNK_AVAILABILITY_PRICE,
  );
  $form['timezynk_sms_price'] = array(
    '#type' => 'textfield',
    '#title' => t('Price per SMS'),
    '#size' => 10,
    '#default_value' => TIMEZYNK_SMS_PRICE,
  );
  $form['timezynk_minimum_month_users'] = array(
    '#type' => 'textfield',
    '#title' => t('Minimum month users'),
    '#size' => 10,
    '#default_value' => TIMEZYNK_MINIMUM_MONTH_USERS,
  );
  $form['timezynk_minimum_month_users_start_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('Minimum month users start date'),
    '#date_format' => 'Y-m-d',
    '#description' => t('After this date, we will never invoice less than minimum month users'),
    '#size' => 10,
    '#default_value' => TIMEZYNK_MINIMUM_MONTH_USERS_START_DATE,
  );
  $form['timezynk_excluded_users'] = array(
    '#type' => 'textfield',
    '#title' => t('Excluded users'),
    '#description' => t('Comma-separated list of user IDs'),
    '#size' => 10,
    '#default_value' => TIMEZYNK_EXCLUDED_USERS,
  );
  return system_settings_form($form);
}