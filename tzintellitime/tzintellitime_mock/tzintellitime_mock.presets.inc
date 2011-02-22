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
  $nbrOfWeeksBackward = 2;
  $totalNbrOfWeeks = $nbrOfWeeksForward + $nbrOfWeeksBackward + 1;

  $start = clone($today);
  $start->modify("-$nbrOfWeeksBackward weeks");

  $reports = array();
  $todayIndex = 0;
  for($i = 0; $i < $totalNbrOfWeeks; $i++) {
    $date = clone($start);
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

      /* We need this index later to be able to set certain reports
       * as REPORTED
       */
      if ($today->format('Ymd') == $dayOfWeek->format('Ymd')) {
        $todayIndex = count($reports);
      }

      $reports[] = $report;
    }
  }

  // Make sure all our history is reported, except the last 3 working days
  for ($i = 0; $i < $todayIndex - 3; $i++) {
    $reports[$i]->state = TZIntellitimeReport::STATE_REPORTED;
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

  drupal_set_message(t('Last reset @date', array('@date' => date_format_date(tzbase_make_date()))));
}