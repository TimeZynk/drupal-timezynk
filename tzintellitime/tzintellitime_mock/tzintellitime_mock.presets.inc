<?php

function tzintellitime_mock_presets(&$form_state) {
  $form['reset'] = array(
    '#type' => 'submit',
    '#value' => 'Reset demo',
  );

  return $form;
}

function tzintellitime_mock_presets_submit($form, $form_state) {
  // Reset all data
  db_query('DELETE FROM {tzintellitime_mock_data');

  // Create assignment
  $assignment = new TZIntellitimeAssignment();
  $assignment->title = 'Demoföretag, demouppgift';
  $assignment_id = tzintellitime_mock_store_object($assignment);

  // Build up new reports
  $today = tzbase_make_date();

  $nbrOfWeeksForward = 2;

  $reports = array();
  for($i = 0; $i <= $nbrOfWeeksForward; $i++) {
    $date = clone($today);
    $date->modify("+$i weeks");
    $weekSpan = tzintellitime_week_span($date);
    $dayOfWeek = clone($weekSpan[0]);
    for($dow = 0; $dow < 5; $dow++, $dayOfWeek->modify('+1 day')) {
      $report = new TZIntellitimeReport();
      $report->title = $assignment->title;
      $report->assignment_id = $assignment_id;
      $report->year = $dayOfWeek->format('Y');
      $report->month = $dayOfWeek->format('m');
      $report->day = $dayOfWeek->format('d');
      $report->begin = '08:00';
      $report->end = '16:30';
      $report->break_duration_minutes = '30';

      $reports[] = $report;
    }
  }

  // Save the reports once for every user
  $result = db_query('SELECT user_id FROM {tzintellitime_mock_users}');
  while($user_id = db_result($result)) {
    foreach($reports as $report) {
      $report_clone = clone($report);
      $report_clone->user_id = $user_id;
      tzintellitime_mock_store_object($report_clone);
    }
  }

  tzintellitime_mock_schedule_all_users();
}