<?php

function tzbase_user_jobs_form(&$form_state) {
  $form = array();
  $form['user'] = tzbase_select_users_field();
  $form['job'] = tzbase_select_assignment_field();

  $form['start_date'] = array(
    '#type' => 'date_popup',
    '#date_format' => 'Y-m-d',
    '#default_value' => '',
  );

  $form['end_date'] = array(
    '#type' => 'date_popup',
    '#date_format' => 'Y-m-d',
    '#default_value' => '',
  );

  $form['rows'] = array();
  $mappings = tzbase_user_jobs_mapper()->findAll();
  foreach ($mappings as $mapping) {
    $row['id'] = array('#value' => $mapping->getId());

    $account = user_load($mapping->getUserId());
    $row['user'] = array('#value' => theme('username', $account));
    $row['jobid'] = array('#value' => l(theme('assignment_path', $mapping->getJobId()), 'node/' . $mapping->getJobId(), array('html' => TRUE)));

    $start_date = $mapping->getStartTime();
    $row['start_time'] = array('#value' => empty($start_date) ? '' : $start_date->format('Y-m-d H:i:s'));

    $end_date = $mapping->getEndTime();
    $row['end_time'] = array('#value' => empty($end_date) ? '' : $end_date->format('Y-m-d H:i:s'));
    $form['rows'][] = $row;
  }

  $form['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );
  return $form;
}

function tzbase_user_jobs_form_submit($form, &$form_state) {
  if ($form_state['values']['op'] == t('Add')) {
    $mapper = tzbase_user_jobs_mapper();
    $mapping = $mapper->createMapping();
    $mapping->setUserId($form_state['values']['user']);

    $jobid_trail = split(':', $form_state['values']['job']);
    $mapping->setJobId(end($jobid_trail));

    if (!empty($form_state['values']['start_date'])) {
      $mapping->setStartTime(date_make_date($form_state['values']['start_date']));
    }

    if (!empty($form_state['values']['end_date'])) {
      $end_date = date_make_date($form_state['values']['end_date']);
      $end_date->setTime(23, 59, 59);
      $mapping->setEndTime($end_date);
    }

    try{
      $mapping->save();
    } catch (TZDBValidationException $e) {
      foreach ($e->getErrors() as $error) {
        form_set_error('', $error);
      }
    }
  }
}

function tzbase_user_jobs_delete($user_job_mapping_id) {
  $mapper = tzbase_user_jobs_mapper();
  if (is_numeric($user_job_mapping_id)) {
    $mapper->deleteMapping($user_job_mapping_id);
  }
  drupal_goto('user_jobs');
}

function theme_tzbase_user_jobs_form($form) {
  $output = "";

  $headers = array(
    t('ID'),
    t('Employee'),
    t('Assignment'),
    t('Start date'),
    t('End date'),
    '',
  );

  $rows = array();

  foreach ($form['rows'] as $index => $row) {
    if ($index[0] == '#') {
      continue;
    }

    $rows[] = array(
      drupal_render($form['rows'][$index]['id']),
      drupal_render($form['rows'][$index]['user']),
      drupal_render($form['rows'][$index]['jobid']),
      drupal_render($form['rows'][$index]['start_time']),
      drupal_render($form['rows'][$index]['end_time']),
      l(t('delete'), 'user_jobs/' . $form['rows'][$index]['id']['#value'] . '/delete'),
    );
  }

  $rows[] = array(
    '',
    drupal_render($form['user']),
    drupal_render($form['job']),
    drupal_render($form['start_date']),
    drupal_render($form['end_date']),
    drupal_render($form['add']),
  );

  $output .= theme('table', $headers, $rows);
  $output .= drupal_render($form);
  return $output;
}

function tzbase_select_assignment_field() {
  $joblist = tzbase_list_jobs();

  return array(
    '#type' => 'select',
    '#options' => $joblist,
    '#default_value' => 'nid:0',
  );
}

function tzbase_select_users_field() {
  $userlist = tzuser_list_users();
  $useroptions = array();
  foreach($userlist as $account) {
    $useroptions[$account->uid] = $account->tzuser['fullname'];
  }
  return array(
    '#type' => 'select',
    '#options' => $useroptions,
  );
}
