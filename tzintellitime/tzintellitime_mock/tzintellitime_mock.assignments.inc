<?php

function tzintellitime_mock_setup_assignments(&$form_state) {
  tzintellitime_include_classes();

  $form['assignments'] = array(
    '#tree' => TRUE,
    '#theme' => 'mock_assignments_table',
  );

  $result = db_query('SELECT * FROM {tzintellitime_mock_data} WHERE user_id = 0 ORDER BY id ASC');
  while($row = db_fetch_object($result)) {
    $assignment = unserialize($row->data);
    if($assignment instanceof TZIntellitimeAssignment) {
      $form['assignments'][] = _mock_assignment_form_entry($row->id, $assignment);
    }
  }

  for($i = 0; $i < 10; $i++) {
    $form['assignments'][] = _mock_assignment_form_entry();
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function tzintellitime_mock_setup_assignments_submit($form, &$form_state) {
  foreach($form_state['values']['assignments'] as $row) {
    if(!empty($row['title'])) {
      $db_object = array();
      $new_row = FALSE;

      if(empty($row['id'])) {
        // New record, create new ID
        $new_row = TRUE;
        $db_object['data'] = 'new row';
        drupal_write_record('tzintellitime_mock_data', $db_object);
        $row['id'] = $db_object['id'];
      } else {
        $db_object['id'] = $row['id'];
      }

      // Create assignment object and serialize to data column
      $assignment = _assignment_from_form($row);
      $db_object['data'] = serialize($assignment);

      drupal_write_record('tzintellitime_mock_data', $db_object, 'id');
      drupal_set_message(t('@action assignment with mock id @id', array(
          '@action' => $new_row ? 'Created' : 'Updated',
          '@id' => $row['id']
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
  if($values['type'] == TZIntellitimeAssignment::TYPE_ABSENCE) {
    $assignment->id = '_AC_' . $values['id'] . '_' . substr($values['title'], 0, 10);
  } else {
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
  $output .= theme('table', $header, $rows);

  return $output;
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
      '#default_value' => $assignment ? $assignment->type : TZIntellitimeAssignment::TYPE_ASSIGNMENT,
      '#options' => array(
        TZIntellitimeAssignment::TYPE_ASSIGNMENT => t('Assignment'),
        TZIntellitimeAssignment::TYPE_ABSENCE => t('Absence'),
      ),
    ),
  );
}