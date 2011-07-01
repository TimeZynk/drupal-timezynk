<?php

/**
 * Form definition to send free-text SMS to a user
 * @param Array $form_state current form state
 */
function tzsms_ui_send_sms_form(&$form_state) {
  $form['message'] = array(
    '#type' => 'textarea',
    '#title' => t('SMS Message'),
    '#description' => t('Enter any text you want to send as SMS. Maximum message length 459 characters.'),
    '#required' => TRUE,
  );

  $form['recipient'] = tzsms_ui_recipient_form();

  $form['send'] = array(
    '#type' => 'submit',
    '#value' => t('Send'),
  );
  return $form;
}

function tzsms_ui_send_sms_form_validate($form, $form_state) {
  $message = trim($form_state['values']['message']);
  if (mb_strlen($message) > 459) {
    form_set_error('message', t('Message cannot exceed 459 characters in length. Current length !length',
        array('!length' => mb_strlen($message))));
  }
}

function tzsms_ui_send_sms_form_submit($form, $form_state) {
  $users = array();
  if(!empty($form_state['values']['recipient']['users'])) {
    $users = array_keys($form_state['values']['recipient']['users']);
  }
  $userFilter = new TZUserFilter(
      $form_state['values']['recipient']['policy'],
      $users
  );

  // Remove whitespace before and after message
  $message = trim($form_state['values']['message']);

  $count = 0;
  $result = db_query('SELECT uid FROM {users} WHERE status = 1 AND uid != 0');
  while($uid = db_result($result)) {
    if(!$userFilter->isIncluded($uid)) {
      continue;
    }
    $account = user_load($uid);
    if(empty($account->tzuser['mobile'])) {
      continue;
    }
    watchdog('tzsms', 'Sent custom message to @number (!user)',
        array('@number' => $account->tzuser['mobile'],
          '!user' => theme('username', $account)));
    if(tzsms_send(TZSMS_TYPE_USER_SMS, $account->tzuser['mobile'], $message)) {
      $count++;
    }
  }
  drupal_set_message(t('Sent message to @count mobiles', array('@count' => $count)));
}
