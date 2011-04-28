<?php

/**
 * Form definition for send SMS to number UI
 * @param Array $form_state current form state
 */
function tzuser_add_users_form(&$form_state) {
  ahah_helper_register($form, $form_state);

  $form['#attributes']['enctype'] = 'multipart/form-data';

  // Check how many number rows we currently show
  $numrows = 10;
  if(isset($form_state['storage']['numrows'])) {
    $numrows = $form_state['storage']['numrows'];
    // Add more button clicked, increase number of rows
    if($form_state['values']['op'] === t('Add more rows')) {
      $numrows += 10;
    }
  }

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
    $row_id = 'row_' . $i;

    $default_manager = $GLOBALS['user']->uid;
    if (!empty($form_state['values']['rows'][$row_id]['manager'])) {
      $default_manager = $form_state['values']['rows'][$row_id]['manager'];
    }
    
    $row = array(
      'mobile' => !empty($form_state['values']['rows'][$row_id]['number']) ? $form_state['values']['rows'][$row_id]['number'] : '',
      'username' => !empty($form_state['values']['rows'][$row_id]['username']) ? $form_state['values']['rows'][$row_id]['username'] : '',
      'password' => !empty($form_state['values']['rows'][$row_id]['password']) ? $form_state['values']['rows'][$row_id]['password'] : '',
      'manager' => $default_manager,
    );

    $form['rows'][$row_id] = tzuser_add_row_fields($i, $row);
  }

  if (!empty($form_state['storage']['csv_rows'])) {
    $csv_rows = $form_state['storage']['csv_rows'];
    $i = 0;
    foreach ($csv_rows as $csv_row) {
      $row_id = 'row_' . $i;
      $form['rows'][$row_id] = tzuser_add_row_fields($i, $csv_row);
      $i += 1;
    }
    $numrows = $i;
    unset($form_state['storage']['csv_rows']);
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

  $form['file'] = array(
    '#type' => 'file',
    '#title' => t('Load from file'),
    '#description' => t('Fill the fields from a CSV file'),
    '#access' => user_access('bulk add employees'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add employees'),
  );

  $form['reset'] = array(
    '#value' => l(t('Reset'), 'tzuser/add'),
  );

  $form_state['storage']['numrows'] = $numrows;

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

  // Validate posted rows
  for($i = 0; $i < $numrows; $i++) {
    $rowid = 'row_' . $i;
    if(!empty($form_state['values']['rows'][$rowid]['number']) ||
       !empty($form_state['values']['rows'][$rowid]['username'])) {
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

  // Check and add any rows from CSV-file
  $csv_file = file_save_upload('file');
  if ($csv_file) {
    if ($csv_file->filemime != 'text/csv') {
      form_set_error("file", t('Must be in CSV format'));
      return;
    }

    $csv_rows = array();
    $parser = new CsvUserListParser($csv_file->filepath);
    while ($row = $parser->getNextRow()) {
      $manager_user_id = 0;


      if (!empty($row->BossUserId)) {
        $manager_user_id = tzuser_add_find_manager_id($row->BossUserId);
        if (!$manager_user_id) {
          drupal_set_message(t('Unknown manager "@manager"', array('@manager' => $row->BossUserId)), 'warning', FALSE);
        }
      }

      $csv_rows[] = array(
        'mobile' => $row->MobilePhone,
        'username' => $row->UserId,
        'password' => $row->Password,
        'manager' => $manager_user_id,
      );
    }
    
    $form_state['storage']['csv_rows'] = $csv_rows;
  }
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

    // Run batch if no file is set
    if (empty($form_state['storage']['csv_rows'])) {
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
}

function tzuser_add_mobile_field($default_value = '') {
  return array(
    '#type' => 'textfield',
    '#size' => 20,
    '#default_value' => $default_value,
  );
}

function tzuser_add_username_field($default_value = '') {
  return array(
    '#type' => 'textfield',
    '#size' => 20,
    '#default_value' => $default_value,
  );
}

function tzuser_add_password_field($default_value = '') {
  return array(
    '#type' => 'textfield',
    '#size' => 20,
    '#default_value' => $default_value,
  );
}

function tzuser_add_find_manager_id($manager_user_name) {
  $account = user_load(array('name' => $manager_user_name));

  if ($account) {
    return $account->uid;
  }
  return 0;
}

function tzuser_add_row_fields($index, $row) {
  $row_fields = array();
  $row_fields['index'] = array('#value' => $index + 1);
  $row_fields['number'] = tzuser_add_mobile_field($row['mobile']);
  $row_fields['username'] = tzuser_add_username_field($row['username']);
  $row_fields['password'] = tzuser_add_password_field($row['password']);
  $row_fields['manager'] = tzuser_select_manager_field($row['manager']);
  return $row_fields;
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
