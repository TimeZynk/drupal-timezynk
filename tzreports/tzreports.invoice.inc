<?php

define('TZREPORTS_INVOICE_DATE_FORMAT', variable_get('tzreports_invoice_date_format', 'Y-m-d'));
/**
 * @file
 * Invoice generator
 */

function tzreports_invoice(&$form_state) {
  $numargs = func_num_args();
  $arguments = func_get_args();

  if(isset($form_state['values'])) {
    $form_state['storage']['job'] = $form_state['values']['job'];
    foreach(array('start', 'end') as $key) {
      $form_state['storage'][$key] = $form_state['values'][$key];
    }
  }
  $form_state['rebuild'] = TRUE;

  $form['filter'] = _tzreports_invoice_filter_form($form_state);

  if(isset($form_state['values'])) {
    $jobtrail = explode(':', $form_state['values']['job']);
    $parentjob = node_load(end($jobtrail));

    $startdate = _tzreports_datetime_from_formdate($form_state['values']['start']);
    $enddate = _tzreports_datetime_from_formdate($form_state['values']['end']);

    drupal_set_title(t('Summary for @job @startdate - @enddate', array(
      '@job' => $parentjob->title,
      '@startdate' => $startdate->format(TZREPORTS_INVOICE_DATE_FORMAT),
      '@enddate' => $enddate->format(TZREPORTS_INVOICE_DATE_FORMAT),
    )));

    $form['data'] = _tzreports_show_invoices_for_job($parentjob, $startdate, $enddate, array());
  }

  // Form settings
  $form['#redirect'] = FALSE;

  return $form;
}

function _tzreports_show_invoices_for_job(&$job, $startdate, $enddate, $trail) {
  $output = array();

  $trail[] = $job->title;

  // make sure we show reports from startdate midnight up to and including enddate
  $startdate->modify('midnight');
  $enddate->modify('+1 day');
  $enddate->modify('midnight');

  // Load default view
  $view_content = views_embed_view(variable_get('tzreports_invoice_view', 'invoice_report'),
    variable_get('tzreports_invoice_view_display', 'default'),
    $job->nid,
    $startdate->format('U'),
    $enddate->format('U') - 1);

  if(false === strpos($view_content, 'view-content')) {
    $view_content = t('No reports for the selected date span');
  }

  $output['job_' . $job->nid] = array(
    '#type' => 'fieldset',
    '#title' => theme('assignment_trail', $trail),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    'data' => array(
      '#prefix' => '<div>',
      '#value' => $view_content,
      '#suffix' => '</div>'
    ),
  );

  // Find all children and show their results
  $result = _tzreports_get_jobs_for_parent($job->nid);
  while($child = db_fetch_object($result)) {
    $output[] = _tzreports_show_invoices_for_job($child, $startdate, $enddate, $trail);
  }

  return $output;
}

function _tzreports_invoice_filter_form(&$form_state) {
  $date = date_make_date('now');

  // Set default day to first and last day of current month
  $default_date = array();
  $default_date['start'] = array(
    'year' => $date->format('Y'),
    'month' => $date->format('n'),
    'day' => 1,
  );

  $date = _tzreports_datetime_from_formdate($default_date['start']);
  $date->modify('+1 month');
  $date->modify('-1 day');
  $default_date['end'] = array(
    'year' => $date->format('Y'),
    'month' => $date->format('n'),
    'day' => $date->format('j'),
  );

  // Check if users has stored new default values that should override the old
  foreach(array('start', 'end') as $key) {
    if(isset($form_state['storage'][$key])) {
      $default_date[$key] = $form_state['storage'][$key];
    }
  }

  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Select options'),
    '#tree' => FALSE,
    '#collapsible' => TRUE,
    '#collapsed' => isset($form_state['values']),
  );

  $joblistoptions = tzbase_list_jobs();
  array_unshift($joblistoptions, '-- ' . t('Select assignment') . ' --');
  $form['job'] = array(
    '#type' => 'select',
    '#title' => t('Select assignment'),
    '#options' => $joblistoptions,
    '#description' => t('Select which assignment to list time reports for'),
    '#required' => TRUE,
    '#default_value' => isset($form_state['storage']['job']) ? $form_state['storage']['job'] : 0,
  );
  $form['start'] = array(
    '#type' => 'date',
    '#title' => t('Start date'),
    '#default_value' => $default_date['start'],
  );
  $form['end'] = array(
    '#type' => 'date',
    '#title' => t('End date'),
    '#default_value' => $default_date['end'],
  );
  $form['submit'] = array(
    '#name' => '', // prevent from showing up in $_GET.
    '#type' => 'submit',
    '#value' => t('Apply'),
  );

  return $form;
}

function tzreports_invoice_validate($form, &$form_state) {
  if($form_state['values']['job'] === '0') {
    form_set_error('job', t('Please select an assignment'));
  }

  $startdate = _tzreports_datetime_from_formdate($form_state['values']['start']);
  $enddate = _tzreports_datetime_from_formdate($form_state['values']['end']);

  if($enddate < $startdate) {
    form_set_error('end', t('End date must be after start date'));
  }
}

function _tzreports_datetime_from_formdate($formdate) {
  $datestring = sprintf('%04d-%02d-%02d 00:00:00', $formdate['year'], $formdate['month'], $formdate['day']);
  return date_make_date($datestring);
}

/**
 * Fetch all jobs that are children to $parentid
 * @param int $parentid parent id
 */
function _tzreports_get_jobs_for_parent($parentid) {
  $sql = 'SELECT * FROM {tzjob} t INNER JOIN {node} n on t.nid = n.nid WHERE t.parentid = %d ORDER BY n.title';
  return db_query(db_rewrite_sql($sql), $parentid);
}