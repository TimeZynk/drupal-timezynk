<?php
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
      $form_state['storage'][$key] = tzreports_timestamp_from_formdate($form_state['values'][$key]);
    }
  }
  $form_state['rebuild'] = TRUE;

  $form['filter'] = tzreports_invoice_form($form_state);

  if(isset($form_state['values'])) {
    $parentid = $form_state['values']['job'];
    $parentjob = node_load($parentid);

    $startdate = tzreports_timestamp_from_formdate($form_state['values']['start']);
    $enddate = tzreports_timestamp_from_formdate($form_state['values']['end']);

    drupal_set_title(t('Summary for @job @startdate - @enddate', array(
      '@job' => $parentjob->title,
      '@startdate' => date(variable_get('tzreports_invoice_date_format', 'Y-m-d'), $startdate),
      '@enddate' => date(variable_get('tzreports_invoice_date_format', 'Y-m-d'), $enddate),
    )));

    $form['data'] = tzreports_show_invoices_for_job($parentjob, $startdate, $enddate, array());
  }

  // Form settings
  $form['#theme'] = 'tzreports_invoice_form';
  $form['#validate'][] = 'tzreports_invoice_form_validate';
  //$form['#submit'][] = 'tzreports_invoice_form_submit';
  $form['#redirect'] = FALSE;

  // Make sure that we validate because this form might be submitted
  // multiple times per page.
  //$form_state['must_validate'] = TRUE;
  //$form_state['rebuild'] = TRUE;

  return $form;
}

function tzreports_show_invoices_for_job(&$job, $startdate, $enddate, $trail) {
  $output = array();

  $trail[] = $job->title;

  // Load default view
  $view_content = views_embed_view(variable_get('tzreports_invoice_view', 'invoice_report'),
    variable_get('tzreports_invoice_view_display', 'default'),
    $job->nid,
    $startdate,
    $enddate);

  if(false === strpos($view_content, 'view-content')) {
    $view_content = t('No reports for the selected date span');
  }

  $output['job_' . $job->nid] = array(
    '#type' => 'fieldset',
    '#title' => theme('invoice_legend', $trail),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    'data' => array(
      '#prefix' => '<div>',
      '#value' => $view_content,
      '#suffix' => '</div>'
    ),
  );

  // Find all children and show their results
  $result = tzreports_get_jobs_for_parent($job->nid);
  while($child = db_fetch_object($result)) {
    $output[] = tzreports_show_invoices_for_job($child, $startdate, $enddate, $trail);
  }

  return $output;
}


function tzreports_invoice_job_list() {
  $result = tzreports_get_jobs_for_parent($parentid);
  $joblist = array();
  $joblist[0] = t('Select job...');
  while($job = db_fetch_object($result)) {
    $joblist[$job->nid] = $job->title;
  }

  return $joblist;
}

function tzreports_invoice_form(&$form_state) {
  // Set default day to first and last day of current month
  $today = getdate();
  $default_date = array();
  $default_date['start'] = getdate(mktime(0, 0, 0, $today['mon'], 1, $today['year']));
  $default_date['end'] = getdate(mktime(0, 0, 0, $today['mon'] + 1, 0, $today['year']));

  // Check if users has stored new default values that should override the old
  foreach(array('start', 'end') as $key) {
    if(isset($form_state['storage'][$key])) {
      $default_date[$key] = getdate($form_state['storage'][$key]);
    }
  }

  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Select options'),
    '#tree' => FALSE,
    '#collapsible' => TRUE,
    '#collapsed' => isset($form_state['values']),
  );
  $form['job'] = array(
    '#type' => 'select',
    '#title' => t('Job to invoice'),
    '#options' => tzreports_invoice_job_list(),
    '#description' => t('Select which job to list time reports for'),
    '#required' => TRUE,
    '#default_value' => isset($form_state['storage']['job']) ? $form_state['storage']['job'] : 0,
  );
  $form['start'] = array(
    '#type' => 'date',
    '#title' => t('Start date'),
    '#default_value' => array(
      'year' => $default_date['start']['year'],
      'month' => $default_date['start']['mon'],
      'day' => $default_date['start']['mday'],
    ),
  );
  $form['end'] = array(
    '#type' => 'date',
    '#title' => t('End date'),
    '#default_value' => array(
      'year' => $default_date['end']['year'],
      'month' => $default_date['end']['mon'],
      'day' => $default_date['end']['mday'],
    ),
  );
  $form['submit'] = array(
    '#name' => '', // prevent from showing up in $_GET.
    '#type' => 'submit',
    '#value' => t('Apply'),
  );

  return $form;
}

function tzreports_invoice_form_validate($form, &$form_state) {
  if($form_state['values']['job'] == 0) {
    form_set_error('job', t('Please select a job to invoice'));
  }

  $startdate = tzreports_timestamp_from_formdate($form_state['values']['start']);
  $enddate = tzreports_timestamp_from_formdate($form_state['values']['end']);

  if($enddate < $startdate) {
    form_set_error('end', t('End date must be after start date'));
  }
}

function tzreports_invoice_form_submit($form, &$form_state) {

}

function tzreports_timestamp_from_formdate($formdate) {
  return mktime(0, 0, 0, $formdate['month'], $formdate['day'], $formdate['year']);
}

/**
 * Fetch all jobs that are children to $parentid
 * @param int $parentid parent id
 */
function tzreports_get_jobs_for_parent($parentid) {
  $sql = 'SELECT * FROM {tzjob} t INNER JOIN {node} n on t.nid = n.nid WHERE t.parentid = %d ORDER BY n.title';
  return db_query(db_rewrite_sql($sql), $parentid);
}