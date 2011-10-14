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
      'email' => !empty($form_state['values']['rows'][$row_id]['email']) ? $form_state['values']['rows'][$row_id]['email'] : '',
      'manager' => $default_manager,
    );

    $form['rows'][$row_id] = tzuser_add_row_fields($i, $row);
  }

  if (!empty($form_state['storage']['bulk_add_rows'])) {
    $bulk_add_rows = $form_state['storage']['bulk_add_rows'];
    $i = 0;
    foreach ($bulk_add_rows as $bulk_row) {
      $row_id = 'row_' . $i;
      $form['rows'][$row_id] = tzuser_add_row_fields($i, $bulk_row);
      $i += 1;
    }
    $numrows = $i;
    unset($form_state['storage']['bulk_add_rows']);
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
    '#description' => t('Fill the fields from file (.csv, .xls, .xlsx). Valid headers are "UserId", "MobilePhone", "Password", "BossUserId"'),
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
        $seen_numbers[$number] = array(
          'mobile' => $number,
          'username' => trim($form_state['values']['rows'][$rowid]['username']),
          'password' => trim($form_state['values']['rows'][$rowid]['password']),
          'email' => trim($form_state['values']['rows'][$rowid]['email']),
          'manager' => intval($form_state['values']['rows'][$rowid]['manager']),
        );
      } else {
        form_set_error("rows][$rowid][number", t('Mobile number entered twice'));
      }
    }
  }
  $form_state['storage']['numbers'] = $seen_numbers;

  // Check and add any rows from CSV-file
  $bulk_file = file_save_upload('file');
  if ($bulk_file) {
    try {
      $parser = new BulkUserParser($bulk_file, 'tzuser_add_find_manager_id');
      $rows = $parser->getRows();
    } catch (Exception $e) {
      form_set_error("file", t('Must be in .csv,.xls,.xlsx or .ods format'));
      return;
    }

    $bulk_add_rows = array_map(function($row) {
      $manager_user_id = 0;
      if (!empty($row->bossuserid)) {
        $manager_user_id = tzuser_add_find_manager_id($row->bossuserid);
        if (!$manager_user_id) {
          drupal_set_message(t('Unknown manager "@manager"', array('@manager' => $row->bossuserid)), 'warning', FALSE);
        }
      }

      return array(
        'mobile' => $row->mobilephone,
        'username' => $row->userid,
        'password' => $row->password,
        'email' => $row->email,
        'manager' => $manager_user_id,
      );
    }, $rows);

    $form_state['storage']['bulk_add_rows'] = $bulk_add_rows;
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
    if (empty($form_state['storage']['bulk_add_rows'])) {
      tzbase_add_progress_js();
      drupal_add_js(array('tzuser_new_entries' => array_values($form_state['storage']['numbers'])), 'setting');
      drupal_add_js(drupal_get_path('module', 'tzuser') . '/javascript/tzuser.add.js');
    }
  }
}

function tzuser_add_textfield($default_value = '', $access = TRUE) {
  return array(
    '#type' => 'textfield',
    '#size' => 20,
    '#default_value' => $default_value,
    '#access' => $access,
  );
}

function tzuser_add_restricted_textfield($default_value = '') {
  return tzuser_add_textfield($default_value, user_access('bulk add employees'));
}


function tzuser_add_find_manager_id($manager_user_name) {
  static $idmap = array();

  if (!empty($idmap[$manager_user_name])) {
    return $idmap[$manager_user_name];
  }

  $account = user_load(array('name' => $manager_user_name));
  if ($account) {
    $idmap[$manager_user_name] = $account->uid;
    return $account->uid;
  }

  return 0;
}

function tzuser_add_row_fields($index, $row) {
  $row_fields = array();
  $row_fields['index'] = array('#value' => $index + 1);
  $row_fields['number'] = tzuser_add_textfield($row['mobile']);
  $row_fields['username'] = tzuser_add_textfield($row['username']);
  $row_fields['password'] = tzuser_add_restricted_textfield($row['password']);
  $row_fields['email'] = tzuser_add_restricted_textfield($row['email']);
  $row_fields['manager'] = tzuser_select_manager_field($row['manager']);
  return $row_fields;
}

function tzuser_add_ajax() {
  $result = array();
  // Read POST data
  $raw_data = file_get_contents('php://input');
  $users = json_decode($raw_data);
  foreach ($users as $u) {
    $account = tzuser_add_user(array(
      'username' => $u->username,
      'mobile' => $u->mobile,
      'password' => $u->password,
      'manager' => $u->manager,
      'email' => $u->email,
    ));

    if ($account) {
      module_invoke_all('tzuser_add_user_notify', $account, $u->password);
      $result[$u->mobile]['message'] = t('Created employee @username with mobile @number',
                                             array('@username' => $account->name,
                                                   '@number' => $account->tzuser['mobile']));
      watchdog('tzuser', $result[$u->mobile]['message']);
    } else {
      $result[$u->mobile]['error'] = t('Failed to add employee @username with mobile @number',
                                           array('@username' => $u>username,
                                                 '@number' => $u->mobile));
      watchdog('tzuser', $result[$u->mobile]['message'], WATCHDOG_WARNING);
    }
  }
  print drupal_json($result);
}
