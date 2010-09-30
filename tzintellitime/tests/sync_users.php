<?php

global $user;
tzbase_include_proto_classes();
tzintellitime_include_classes();

$account = user_load(array('name' => 'Johan Heander'));
if($account) {
  TZIntellitimeBot::destroy_session_data($account->intellitime_session_data);
  user_save($account, array('intellitime_session_data' => NULL));
}

// Login and logout to refresh cookie
$form_state = array(
  'values' => array(
    'name' => 'Johan Heander',
    'pass' => '0733623516',
    'op' => t('Log in'),
  ),
);
drupal_execute('user_login', $form_state);

// Destroy the current session:
session_destroy();
// Only variables can be passed by reference workaround.
$null = NULL;
user_module_invoke('logout', $null, $user);
// Load the anonymous user
$user = drupal_anonymous_user();


tzintellitime_sync_synchronize_users();

TZIntellitimeBot::destroy_session_data($account->intellitime_session_data);