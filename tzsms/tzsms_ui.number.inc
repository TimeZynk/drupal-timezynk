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

  $form['rows'] = array(
    '#type' => 'fieldset',
    '#title' => t('Numbers to send to'),
    '#prefix' => '<div id="tzsms_ui_number_rows">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );

  for($i = 0; $i < $numrows; $i++) {
    $rowid = 'row_' . $i;
    $form['rows'][$rowid] = array(
      '#type' => 'textfield',
      '#size' => 20,
      '#default_value' => isset($form_state['values']['rows'][$rowid]) ? $form_state['values']['rows'][$rowid] : '',
      '#field_prefix' => t('Recipient !index', array('!index' => ($i+1))),
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
 * Validation function for tzsms_ui_to_number_form
 * @param $form the form
 * @param $form_state form state and values
 */
function tzsms_ui_installation_sms_form_validate($form, &$form_state) {
  $numrows = $form_state['storage']['numrows'];
  $seen_numbers = array();
  for($i = 0; $i < $numrows; $i++) {
    $rowid = 'row_' . $i;
    if(!empty($form_state['values']['rows'][$rowid])) {
      $number = tzsms_validate_phone_number($form_state['values']['rows'][$rowid]);
      if(!$number) {
        form_set_error("rows][$rowid", t('Please enter a valid mobile number'));
      } else if(empty($seen_numbers[$number])) {
        $seen_numbers[$number] = TRUE;
      } else {
        form_set_error("rows][$rowid", t('Number entered twice'));
      }
    }
  }
  $form_state['storage']['numbers'] = array_keys($seen_numbers);
}

function tzsms_ui_installation_sms_form_submit($form, &$form_state) {
  // We only do full submit if we are called by the submit button
  if($form_state['values']['op'] === t('Send installation SMS')) {
    foreach($form_state['storage']['numbers'] as $mobile_number) {
      tzsms_send_new_user_sms($mobile_number, $mobile_number);
      drupal_set_message(t('Sent install SMS to number %number', array('%number' => $mobile_number)));
    }

    // Clear the form
    unset($form_state['storage']);
  }
}