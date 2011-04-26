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

    $default_manager = $GLOBALS['user']->uid;
    if (!empty($form_state['values']['rows'][$rowid]['manager'])) {
      $default_manager = $form_state['values']['rows'][$rowid]['manager'];
    }
    $form['rows'][$rowid]['manager'] = tzuser_select_manager_field($default_manager);
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
      if(empty($number)) {
        form_set_error("rows][$rowid][number", t('Please enter a valid mobile number'));
      } else if(empty($form_state['values']['rows'][$rowid]['username'])) {
        form_set_error("rows][$rowid][username", t('Please enter username'));
      } else if(empty($seen_numbers[$number])) {
        $username = trim($form_state['values']['rows'][$rowid]['username']);
        $password = trim($form_state['values']['rows'][$rowid]['password']);
        $manager = intval($form_state['values']['rows'][$rowid]['manager']);
        $seen_numbers[$number] = array(
          'mobile' => $number,
          'username' => $username,
          'password' => $password,
          'manager' => $manager
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
    $batch = array(
      'operations' => array(),
      'finished' => 'tzuser_add_users_batch_finished',
      'title' => t('Adding employees'),
      'init_message' => t('Starting to add employees'),
      'progress_message' => t('Step @current of @total'),
      'error_message' => t('Errors while adding employees'),
      'file' => __FILE__,
    );

    foreach ($form_state['storage']['numbers'] as $current_user) {
      $batch['operations'][] = array('tzuser_add_users_batch_process', array($current_user));
    }

    batch_set($batch);

    // All sent, clear the form
    unset($form_state['storage']);
  }
}

function tzuser_add_users_batch_process($new_user, &$context) {
  watchdog('tzuser', 'current_user: ' . serialize($new_user));
  $account = tzuser_add_user(
    $new_user['username'],
    $new_user['username'],
    $new_user['password'],
    $new_user['mobile'],
    array(),
    $new_user['manager']
  );

  if($account) {
    module_invoke_all('tzuser_add_user_notify', $account, $new_user['password']);
    $context['message'] = t('Created employee @username with mobile @number',
                           array('@username' => $account->name,
                                '@number' => $account->tzuser['mobile']));
    watchdog('tzuser', $context['message']);
    $context['results'][$account->tzuser['mobile']] = $account->name;
  } else {
    $context['message'] = t('Failed to add employee @username with mobile @number',
                            array('@username' => $new_user['username'],
                                 '@number' => $new_user['mobile']));
  }
}

function tzuser_add_users_batch_finished($success, $results, $operations) {
  if ($success) {
    drupal_set_message(t('All @count employees added successfully!', array('@count' => count($results))));
    foreach ($results as $mobile => $user_name) {
      drupal_set_message(t('Created employee @username with mobile @number', array('@username' => $user_name, '@number' => $mobile)));
    }
  } else {
    $error_operation = reset($operations);
    $operation = array_shift($error_operation);
    $arguments = array_shift($error_operation);
    $arguments_as_string = implode(', ', $arguments);
    drupal_set_message(t('An error occurred during @op with @arguments',
                         array('@op' => $operation, '@arguments' => $arguments_as_string)), 'error');
    watchdog('tzuser', 'An error occurred during @op with @arguments',
             array('@op' => $operation, '@arguments' => $arguments_as_string), WATCHDOG_ERROR);
  }
}
