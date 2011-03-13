<?php

/**
 * Implementation of hook_user_default_permissions().
 */
function timezynk_basic_feature_user_default_permissions() {
  $permissions = array();

  // Exported permission: access content
  $permissions['access content'] = array(
    'name' => 'access content',
    'roles' => array(
      '0' => 'authenticated user',
    ),
  );

  // Exported permission: access user profiles
  $permissions['access user profiles'] = array(
    'name' => 'access user profiles',
    'roles' => array(
      '0' => 'manager',
    ),
  );

  // Exported permission: administer permissions
  $permissions['administer permissions'] = array(
    'name' => 'administer permissions',
    'roles' => array(
      '0' => 'manager',
    ),
  );

  // Exported permission: administer users
  $permissions['administer users'] = array(
    'name' => 'administer users',
    'roles' => array(
      '0' => 'manager',
    ),
  );

  // Exported permission: configure reminders
  $permissions['configure reminders'] = array(
    'name' => 'configure reminders',
    'roles' => array(
      '0' => 'manager',
    ),
  );

  // Exported permission: edit any tzjob content
  $permissions['edit any tzjob content'] = array(
    'name' => 'edit any tzjob content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'manager',
    ),
  );

  // Exported permission: edit any tzreport content
  $permissions['edit any tzreport content'] = array(
    'name' => 'edit any tzreport content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'manager',
    ),
  );

  // Exported permission: edit own tzjob content
  $permissions['edit own tzjob content'] = array(
    'name' => 'edit own tzjob content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'manager',
    ),
  );

  // Exported permission: edit own tzreport content
  $permissions['edit own tzreport content'] = array(
    'name' => 'edit own tzreport content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'manager',
    ),
  );

  // Exported permission: manage employees
  $permissions['manage employees'] = array(
    'name' => 'manage employees',
    'roles' => array(
      '0' => 'manager',
    ),
  );

  // Exported permission: send sms
  $permissions['send sms'] = array(
    'name' => 'send sms',
    'roles' => array(
      '0' => 'manager',
    ),
  );

  // Exported permission: use timezynk report generator
  $permissions['use timezynk report generator'] = array(
    'name' => 'use timezynk report generator',
    'roles' => array(
      '0' => 'manager',
    ),
  );

  // Exported permission: view any tzjob content
  $permissions['view any tzjob content'] = array(
    'name' => 'view any tzjob content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'manager',
    ),
  );

  // Exported permission: view any tzreport content
  $permissions['view any tzreport content'] = array(
    'name' => 'view any tzreport content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'manager',
    ),
  );

  // Exported permission: view employees
  $permissions['view employees'] = array(
    'name' => 'view employees',
    'roles' => array(
      '0' => 'manager',
    ),
  );

  // Exported permission: view own tzjob content
  $permissions['view own tzjob content'] = array(
    'name' => 'view own tzjob content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'manager',
    ),
  );

  // Exported permission: view own tzreport content
  $permissions['view own tzreport content'] = array(
    'name' => 'view own tzreport content',
    'roles' => array(
      '0' => 'authenticated user',
      '1' => 'manager',
    ),
  );

  // Exported permission: view revisions
  $permissions['view revisions'] = array(
    'name' => 'view revisions',
    'roles' => array(
      '0' => 'manager',
    ),
  );

  return $permissions;
}
