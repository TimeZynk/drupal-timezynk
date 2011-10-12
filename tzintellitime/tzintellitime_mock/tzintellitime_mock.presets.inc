<?php

define('TZINTELLITIME_MOCK_DEFAULT_TITLE', 'Demo Company, Task One');
define('TZINTELLITIME_MOCK_DEFAULT_ABSENCE_TITLE', 'Semester');

function tzintellitime_mock_presets(&$form_state) {
  $users = tzintellitime_mock_get_users();
  $user_ids = array_map(function($u) { return $u->user_id; }, $users);
  $user_names = array_map(function($u) { return $u->login; }, $users);

  $selected_users = array();
  if (!empty($_COOKIE['tzintellitime_mock_presets_selected_users'])) {
    $selected_users = unserialize($_COOKIE['tzintellitime_mock_presets_selected_users']);
  }

  $form['users'] = array(
    '#type' => 'select',
    '#title' => 'Select user to reset',
    '#options' => array_combine($user_ids, $user_names),
    '#multiple' => TRUE,
    '#size' => 1,
    '#required' => TRUE,
    '#default_value' => $selected_users,
  );
  $form['reset'] = array(
    '#type' => 'submit',
    '#value' => 'Reset demo',
  );

  return $form;
}

function tzintellitime_mock_presets_submit($form, $form_state) {
  $selected_users = array_filter($form_state['values']['users']);
  setcookie('tzintellitime_mock_presets_selected_users', serialize($selected_users), time()+365*24*3600);

  // Reset data for selected users
  $query_fragments = array_fill(0, count($selected_users), "user_id = %d");
  $query_args = array_values($selected_users);
  db_query('DELETE FROM {tzintellitime_mock_data} WHERE ' . implode(' OR ', $query_fragments), $query_args);

  // Find or create assignment
  $assignments = tzintellitime_mock_get_assignments();
  $assignment = _tzintellitime_mock_presets_load_or_create_assignment($assignments, TZINTELLITIME_MOCK_DEFAULT_TITLE);
  $absence = _tzintellitime_mock_presets_load_or_create_assignment($assignments, TZINTELLITIME_MOCK_DEFAULT_ABSENCE_TITLE, TZIntellitimeAssignment::TYPE_ABSENCE);

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
      $report->assignment_id = $assignment->id;
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
  foreach ($selected_users as $user_id) {
    foreach($reports as $report) {
      $report_clone = clone($report);
      $report_clone->user_id = intval($user_id);
      tzintellitime_mock_store_object($report_clone);
    }
    tzintellitime_mock_schedule_user($user_id);
  }

  drupal_set_message(t('Last reset @date', array('@date' => date_format_date(tzbase_make_date()))));
}

function _tzintellitime_mock_presets_load_or_create_assignment($assignments, $title, $type = TZIntellitimeAssignment::TYPE_ASSIGNMENT) {
  $assignment = NULL;
  $assignments = array_filter($assignments, function($a) use ($title) { return $a->title == $title; });
  if (empty($assignments)) {
    $assignment = new TZIntellitimeAssignment($title, NULL, NULL, $type);
    $assignment->id = tzintellitime_mock_store_object($assignment);
  } else {
    $assignment = $assignments[0];
  }
  return $assignment;
}
