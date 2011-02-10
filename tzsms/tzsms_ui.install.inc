<?php

/**
 * Form definition for send SMS to number UI
 * @param Array $form_state current form state
 */
function tzsms_ui_installation_sms_form(&$form_state) {
  ahah_helper_register($form, $form_state);

  // Check how many number rows we currently show
  $numrows = 10;
  if(isset($form_state['storage']['numrows'])) {
    $numrows = $form_state['storage']['numrows'];
    // Add more button clicked, increase number of rows
    if($form_state['values']['op'] === t('Add more numbers')) {
      $numrows += 10;
    }
  }

  $form_state['storage']['numrows'] = $numrows;

  $form['description'] = array(
    '#value' => '<p>' . t('To send TimeZynk installation SMS, fill in mobile phone numbers in the form below and then press send. If you need more rows use the <em>Add more numbers</em> button.') . '</p>',
  );

  /* We want to theme the form rows as a nice table, so we store
   * them under a dummy 'rows' element which we theme with our
   * own tzsms_ui_installation_rows theme function. This will
   * also make it possible to add more rows through AHAH since
   * the AHAH-handler will now how to style the extra rows.
   */
  $form['rows'] = array(
    '#theme' => 'tzsms_ui_installation_rows',
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
    '#value' => t('Add more numbers'),
    '#ahah' => array(
      'event' => 'click',
      'path' => ahah_helper_path(array('rows')),
      'wrapper' => 'tzsms_ui_number_rows',
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Send installation SMS'),
  );

  $form['reset'] = array(
    '#value' => l(t('Reset'), 'tzsms/installation'),
  );

  return $form;
}

/**
 * Theme the number rows in the SMS form in a nice table
 * @param array $element Element containing the number rows
 */
function theme_tzsms_ui_installation_rows($element) {
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

  // Add our "Add more numbers" button last in the table, with full colspan
  $rows[] = array(array(
    'data' => drupal_render($element['add_more']),
    'colspan' => 4,
  ));

  // Wrap in div for the AHAH-handlers sake
  $output = '<div id="tzsms_ui_number_rows">';

  // Setup headers and output table
  $header = array('', t('Mobile number'), t('Username'), t('Password'));
  $output .= theme('table', $header, $rows);

  $output .= '</div>';
  return $output;
}

/**
 * Validation function for tzsms_ui_to_number_form
 * @param $form the form
 * @param $form_state form state and values
 */
function tzsms_ui_installation_sms_form_validate($form, &$form_state) {
  $numrows = $form_state['storage']['numrows'];
  $seen_numbers = array();
  for($i = 0; $i < $numrows; $i++) {
    $rowid = 'row_' . $i;
    if(!empty($form_state['values']['rows'][$rowid]['number'])) {
      $number = tzsms_validate_phone_number($form_state['values']['rows'][$rowid]['number']);
      if(!$number) {
        form_set_error("rows][$rowid", t('Please enter a valid mobile number'));
      } else if(empty($seen_numbers[$number])) {
        $seen_numbers[$number] = array(
          'username' => $form_state['values']['rows'][$rowid]['username'],
          'password' => $form_state['values']['rows'][$rowid]['password'],
        );
      } else {
        form_set_error("rows][$rowid", t('Number entered twice'));
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
function tzsms_ui_installation_sms_form_submit($form, &$form_state) {
  // We only do full submit if we are called by the submit button
  if($form_state['values']['op'] === t('Send installation SMS')) {
    foreach($form_state['storage']['numbers'] as $mobile_number => $user_data) {
      tzsms_send_new_user_sms($mobile_number, $user_data['username'], $user_data['password']);
      drupal_set_message(t('Sent install SMS to number %number', array('%number' => $mobile_number)));
    }

    // All sent, clear the form
    unset($form_state['storage']);
  }
}