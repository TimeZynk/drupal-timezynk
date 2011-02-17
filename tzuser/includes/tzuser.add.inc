<?php

/**
 * Form definition for send SMS to number UI
 * @param Array $form_state current form state
 */
function tzuser_add_users_form(&$form_state) {
  ahah_helper_register($form, $form_state);

  // Check how many number rows we currently show
  $numrows = 10;
  if(isset($form_state['storage']['numrows'])) {
    $numrows = $form_state['storage']['numrows'];
    // Add more button clicked, increase number of rows
    if($form_state['values']['op'] === t('Add more rows')) {
      $numrows += 10;
    }
  }

  $form_state['storage']['numrows'] = $numrows;

  $form['description'] = array(
    '#value' => '<p>' . t('To add more users to TimeZynk, fill in their mobile numbers and usernames below. If you need more rows use the <em>Add more rows</em> button.') . '</p>',
  );

  /* We want to theme the form rows as a nice table, so we store
   * them under a dummy 'rows' element which we theme with our
   * own tzuser_add_users_rows theme function. This will
   * also make it possible to add more rows through AHAH since
   * the AHAH-handler will now how to style the extra rows.
   */
  $form['rows'] = array(
    '#theme' => 'tzuser_add_users_rows',
    '#tree' => TRUE,
  );

  for($i = 0; $i < $numrows; $i++) {
    $rowid = 'row_' . $i;
    $form['rows'][$rowid]['index'] = array('#value' => $i +1);
    $form['rows'][$rowid]['number'] = array(
      '#type' => 'textfield',
      '#size' => 20,
      '#default_value' => !empty($form_state['values']['rows'][$rowid]['number']) ? $form_state['values']['rows'][$rowid]['number'] : '',
    );
    $form['rows'][$rowid]['username'] = array(
      '#type' => 'textfield',
      '#size' => 20,
      '#default_value' => !empty($form_state['values']['rows'][$rowid]['username']) ? $form_state['values']['rows'][$rowid]['username'] : '',
    );
    $form['rows'][$rowid]['password'] = array(
      '#type' => 'textfield',
      '#size' => 20,
      '#default_value' => !empty($form_state['values']['rows'][$rowid]['password']) ? $form_state['values']['rows'][$rowid]['password'] : '',
    );
  }

  // Button to add more rows
  $form['rows']['add_more'] = array(
    '#type' => 'submit',
    '#value' => t('Add more rows'),
    '#ahah' => array(
      'event' => 'click',
      'path' => ahah_helper_path(array('rows')),
      'wrapper' => 'tzuser_add_number_rows',
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add employees'),
  );

  $form['reset'] = array(
    '#value' => l(t('Reset'), 'tzuser/add'),
  );

  return $form;
}

/**
 * Theme the number rows in the SMS form in a nice table
 * @param array $element Element containing the number rows
 */
function theme_tzuser_add_users_rows($element) {
  // Build up number rows
  $rows = array();
  foreach($element as $rowid => $row) {
    if(isset($element[$rowid]['index']) && is_array($element[$rowid]['index'])) {
      $rows[] = array(
        drupal_render($element[$rowid]['index']),
        drupal_render($element[$rowid]['number']),
        drupal_render($element[$rowid]['username']),
        drupal_render($element[$rowid]['password']),
      );
    }
  }

  // Add our "Add more rows" button last in the table, with full colspan
  $rows[] = array(array(
    'data' => drupal_render($element['add_more']),
    'colspan' => 4,
  ));

  // Wrap in div for the AHAH-handlers sake
  $output = '<div id="tzuser_add_number_rows">';

  // Setup headers and output table
  $header = array('', t('Mobile number') . '*', t('Username') . '*', t('Password'));
  $output .= theme('table', $header, $rows);

  $output .= '</div>';
  return $output;
}

/**
 * Validation function for tzuser_add_users_form
 * @param $form the form
 * @param $form_state form state and values
 */
function tzuser_add_users_form_validate($form, &$form_state) {
  $numrows = $form_state['storage']['numrows'];
  $seen_numbers = array();
  for($i = 0; $i < $numrows; $i++) {
    $rowid = 'row_' . $i;
    if(!empty($form_state['values']['rows'][$rowid]['number'])) {
      $number = tzuser_validate_phone_number($form_state['values']['rows'][$rowid]['number']);
      if(!$number) {
        form_set_error("rows][$rowid][number", t('Please enter a valid mobile number'));
      } else if(empty($form_state['values']['rows'][$rowid]['username'])) {
        form_set_error("rows][$rowid][username", t('Please enter username'));
      } else if(empty($seen_numbers[$number])) {
        $username = trim($form_state['values']['rows'][$rowid]['username']);
        $password = trim($form_state['values']['rows'][$rowid]['password']);
        $seen_numbers[$number] = array(
          'username' => $username,
          'password' => $password,
        );
      } else {
        form_set_error("rows][$rowid][number", t('Mobile number entered twice'));
      }
    }
  }
  $form_state['storage']['numbers'] = $seen_numbers;
}

/**
 * Submit handler for installation SMS form.
 * Reads the pre-processed data from the validator from $form_state['storage']
 * and then sends the SMS messages.
 *
 * @param Array $form Form
 * @param Array $form_state Current form values
 */
function tzuser_add_users_form_submit($form, &$form_state) {
  // We only do full submit if we are called by the submit button
  if($form_state['values']['op'] === t('Add employees')) {
    foreach($form_state['storage']['numbers'] as $mobile_number => $user_data) {
      $account = tzuser_add_user($user_data['username'], $user_data['username'], $user_data['password'], $mobile_number);
      if($account) {
        module_invoke_all('tzuser_add_user_notify', $account, $user_data['password']);
        drupal_set_message(t('Created user @username with mobile @number', array(
          '@username' => $account->name,
          '@number' => $account->tzuser['mobile'])
        ));
      }
    }

    // All sent, clear the form
    unset($form_state['storage']);
  }
}