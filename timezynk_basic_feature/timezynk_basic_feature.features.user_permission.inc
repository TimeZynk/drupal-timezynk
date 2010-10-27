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

  // Exported permission: create tzjob content
  $permissions['create tzjob content'] = array(
    'name' => 'create tzjob content',
    'roles' => array(
      '0' => 'authenticated user',
    ),
  );

  // Exported permission: create tzreport content
  $permissions['create tzreport content'] = array(
    'name' => 'create tzreport content',
    'roles' => array(
      '0' => 'authenticated user',
    ),
  );

  // Exported permission: edit any tzjob content
  $permissions['edit any tzjob content'] = array(
    'name' => 'edit any tzjob content',
    'roles' => array(
      '0' => 'authenticated user',
    ),
  );

  // Exported permission: edit any tzreport content
  $permissions['edit any tzreport content'] = array(
    'name' => 'edit any tzreport content',
    'roles' => array(
      '0' => 'authenticated user',
    ),
  );

  // Exported permission: edit own tzjob content
  $permissions['edit own tzjob content'] = array(
    'name' => 'edit own tzjob content',
    'roles' => array(
      '0' => 'authenticated user',
    ),
  );

  // Exported permission: edit own tzreport content
  $permissions['edit own tzreport content'] = array(
    'name' => 'edit own tzreport content',
    'roles' => array(
      '0' => 'authenticated user',
    ),
  );

  // Exported permission: send installation sms
  $permissions['send installation sms'] = array(
    'name' => 'send installation sms',
    'roles' => array(
      '0' => 'authenticated user',
    ),
  );

  // Exported permission: view any tzjob content
  $permissions['view any tzjob content'] = array(
    'name' => 'view any tzjob content',
    'roles' => array(
      '0' => 'authenticated user',
    ),
  );

  // Exported permission: view any tzreport content
  $permissions['view any tzreport content'] = array(
    'name' => 'view any tzreport content',
    'roles' => array(
      '0' => 'authenticated user',
    ),
  );

  // Exported permission: view own tzjob content
  $permissions['view own tzjob content'] = array(
    'name' => 'view own tzjob content',
    'roles' => array(
      '0' => 'authenticated user',
    ),
  );

  // Exported permission: view own tzreport content
  $permissions['view own tzreport content'] = array(
    'name' => 'view own tzreport content',
    'roles' => array(
      '0' => 'authenticated user',
    ),
  );

  // Exported permission: view tzstats invoice
  $permissions['view tzstats invoice'] = array(
    'name' => 'view tzstats invoice',
    'roles' => array(
      '0' => 'authenticated user',
    ),
  );

  return $permissions;
}
