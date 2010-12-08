<?php

function tzintellitime_mock_setup_users(&$form_state) {
  $form['users'] = array(
    '#tree' => TRUE,
    '#theme' => 'mock_user_table',
  );

  $result = db_query('SELECT * FROM {tzintellitime_mock_users} ORDER BY user_id ASC');
  while($mock_user = db_fetch_object($result)) {
    $form['users'][] = _mock_user_form_entry($mock_user->user_id, $mock_user->login, $mock_user->password);
  }

  for($i = 0; $i < 10; $i++) {
    $form['users'][] = _mock_user_form_entry();
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function tzintellitime_mock_setup_users_submit($form, &$form_state) {
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
}

function theme_mock_user_table($element) {
  // Build up number rows
  $rows = array();
  foreach($element as $rowid => $row) {
    if(isset($element[$rowid]['login']) && is_array($element[$rowid]['login'])) {
      $rows[] = array(
        $element[$rowid]['user_id']['#value'],
        drupal_render($element[$rowid]['login']),
        drupal_render($element[$rowid]['password']),
      );
    }
  }

  // Setup headers and output table
  $header = array(t('ID'), t('Username'), t('Password'));
  $output .= theme('table', $header, $rows);

  return $output;
}

function _mock_user_form_entry($user_id='', $login='', $password='') {
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
    'password' => array(
      '#type' => 'textfield',
      '#default_value' => $password,
      '#size' => 20,
      '#maxlength' => 20,
    ),
  );
}
