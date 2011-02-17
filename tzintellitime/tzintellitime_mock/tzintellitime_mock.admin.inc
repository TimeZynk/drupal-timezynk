<?php

function tzintellitime_mock_admin_settings(&$form_state) {

  $form['tzintellitime_mock_delay_millis'] = array(
    '#type' => 'textfield',
    '#title' => t('Delay in milliseconds'),
    '#description' => t('Extra delay in milliseconds before returning time reports'),
    '#default_value' => TZINTELLITIME_MOCK_DELAY_MILLIS,
  );

  $form['tzintellitime_mock_enable_comments'] = array(
    '#title' => t('Enable comments'),
    '#type' => 'checkbox',
    '#default_value' => TZINTELLITIME_MOCK_ENABLE_COMMENTS,
  );

  $form['tzintellitime_mock_error_code'] = array(
    '#type' => 'select',
    '#title' => t('Error code'),
    '#options' => array(
      '200' => '200: OK',
      '404' => '404: File not found',
      '500' => '500: Internal server error'
    ),
    '#description' => t('Error code for returned from mock'),
    '#default_value' => TZINTELLITIME_MOCK_ERROR_CODE,
  );

  $form['users'] = _tzintellitime_mock_user_settings();
  $form['assignments'] = _tzintellitime_mock_assignments_settings();

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function _tzintellitime_mock_user_settings() {
  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Users'),
    '#tree' => TRUE,
    '#theme' => 'mock_user_table',
  );

  $result = db_query('SELECT * FROM {tzintellitime_mock_users} ORDER BY user_id ASC');
  while($mock_user = db_fetch_object($result)) {
    $form[] = _mock_user_form_entry($mock_user->user_id, $mock_user->login, $mock_user->fullname, $mock_user->password);
  }

  for($i = 0; $i < 10; $i++) {
    $form[] = _mock_user_form_entry();
  }
  return $form;
}

function _tzintellitime_mock_assignments_settings() {
  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Assignments'),
    '#tree' => TRUE,
    '#theme' => 'mock_assignments_table',
  );

  $result = db_query('SELECT * FROM {tzintellitime_mock_data} WHERE user_id = 0 ORDER BY id ASC');
  while($row = db_fetch_object($result)) {
    $assignment = unserialize($row->data);
    if($assignment instanceof TZIntellitimeAssignment) {
      $form[] = _mock_assignment_form_entry($row->id, $assignment);
    }
  }

  for($i = 0; $i < 10; $i++) {
    $form[] = _mock_assignment_form_entry();
  }

  return $form;
}

function tzintellitime_mock_admin_settings_submit($form, &$form_state) {
  variable_set('tzintellitime_mock_delay_millis', $form_state['values']['tzintellitime_mock_delay_millis']);
  variable_set('tzintellitime_mock_enable_comments', $form_state['values']['tzintellitime_mock_enable_comments']);
  variable_set('tzintellitime_mock_error_code', $form_state['values']['tzintellitime_mock_error_code']);

  foreach($form_state['values']['users'] as $mock_user) {
    if(!empty($mock_user['user_id'])) {
      if(empty($mock_user['login'])) {
        db_query('DELETE FROM {tzintellitime_mock_users} WHERE user_id = %d', $mock_user['user_id']);
        drupal_set_message(t('Deleted user with mock id @id', array('@id' => $mock_user['user_id'])));
      } else {
        drupal_write_record('tzintellitime_mock_users', $mock_user, 'user_id');
        drupal_set_message(t('Updated user with mock id @id', array('@id' => $mock_user['user_id'])));
      }
    } else if(!empty($mock_user['login'])) {
      drupal_write_record('tzintellitime_mock_users', $mock_user);
      drupal_set_message(t('Created new user with mock id @id', array('@id' => $mock_user['user_id'])));
    }
  }

  foreach($form_state['values']['assignments'] as $row) {
    if(!empty($row['title'])) {
      $db_object = array();
      $new_row = empty($row['id']);

      // Create assignment object and serialize to data column
      $assignment = _assignment_from_form($row);
      $id = tzintellitime_mock_store_object($assignment);

      drupal_set_message(t('@action assignment with mock id @id', array(
          '@action' => $new_row ? 'Created' : 'Updated',
          '@id' => $id,
        )
      ));
    } else {
      // Empty title, delete if row found
      if(!empty($row['id'])) {
        db_query('DELETE FROM {tzintellitime_mock_data} WHERE id = %d', $row['id']);
        drupal_set_message(t('Deleted assignment with mock id @id', array('@id' => $row['id'])));
      }
    }
  }
}

function _assignment_from_form($values) {
  $assignment = new TZIntellitimeAssignment();
  if(!empty($values['id'])) {
    $assignment->id = $values['id'];
  }
  $assignment->title = trim($values['title']);
  $assignment->type = $values['type'];

  return $assignment;
}

function theme_mock_assignments_table($element) {
  // Build up number rows
  $rows = array();
  foreach($element as $rowid => $row) {
    if(isset($element[$rowid]['title']) && is_array($element[$rowid]['title'])) {
      $rows[] = array(
        $element[$rowid]['id']['#value'],
        drupal_render($element[$rowid]['title']),
        drupal_render($element[$rowid]['type']),
      );
    }
  }

  // Setup headers and output table
  $header = array(t('ID'), t('Title'), t('Type'));

  return theme('table', $header, $rows);
}

function theme_mock_user_table($element) {
  // Build up number rows
  $rows = array();
  foreach($element as $rowid => $row) {
    if(isset($element[$rowid]['login']) && is_array($element[$rowid]['login'])) {
      $rows[] = array(
        $element[$rowid]['user_id']['#value'],
        drupal_render($element[$rowid]['login']),
        drupal_render($element[$rowid]['fullname']),
        drupal_render($element[$rowid]['password']),
      );
    }
  }

  // Setup headers
  $header = array(t('ID'), t('Username'), t('Full name'), t('Password'));

  return theme('table', $header, $rows);
}

function _mock_user_form_entry($user_id='', $login='', $fullname='', $password='') {
  return array(
    'user_id' => array(
      '#type' => 'value',
      '#value' => $user_id,
    ),
    'login' => array(
      '#type' => 'textfield',
      '#default_value' => $login,
      '#size' => 20,
      '#maxlength' => 20,
    ),
    'fullname' => array(
      '#type' => 'textfield',
      '#default_value' => $fullname,
      '#size' => 40,
    ),
    'password' => array(
      '#type' => 'textfield',
      '#default_value' => $password,
      '#size' => 20,
      '#maxlength' => 20,
    ),
  );
}


function _mock_assignment_form_entry($db_id='', $assignment=NULL) {
  return array(
    'id' => array(
      '#type' => 'value',
      '#value' => $db_id,
    ),
    'title' => array(
      '#type' => 'textfield',
      '#default_value' => $assignment ? $assignment->title : '',
      '#size' => 40,
      '#maxlength' => 64,
    ),
    'type' => array(
      '#type' => 'select',
      '#default_value' => isset($assignment->type) ? $assignment->type : TZIntellitimeAssignment::TYPE_ASSIGNMENT,
      '#options' => array(
        TZIntellitimeAssignment::TYPE_ASSIGNMENT => t('Assignment'),
        TZIntellitimeAssignment::TYPE_ABSENCE => t('Absence'),
      ),
    ),
  );
}