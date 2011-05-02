<?php

function tzstats_invoice_form(&$form_state) {
  $default_date = tzbase_make_date();
  $default_date->modify('-1 month');

  $form['date'] = tzstats_select_date_field($default_date);
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Invoice!'),
  );

  return $form;
}

function tzstats_invoice_form_submit($form, &$form_state) {
  $user_bucket = array();

  $start_date = date_make_date($form_state['values']['start_date']);
  $end_date = date_make_date($form_state['values']['end_date']);

  $output = '<h1>Invoice ' . $start_date->format('Y-m-d') . ' - ' . $end_date->format('Y-m-d') . '</h1>';
  $output .= '<h2>' . variable_get('site_name', 'Site name') . '</h2>';

  $end_date->modify('+1 day');
  $sql = "SELECT * FROM tzstats_report INNER JOIN tzreport ON tzreport.vid = tzstats_report.vid " .
         "WHERE tzstats_report.timestamp >= %d AND tzstats_report.timestamp < %d " .
         "ORDER BY tzstats_report.uid";

  $result = db_query($sql, $start_date->format('U'), $end_date->format('U'));
  while ($entry = db_fetch_object($result)) {
    if ($entry->uid == 1) {
      continue;
    }

    $date = tzbase_make_date($entry->begintime);
    $user_bucket[$entry->uid][$date->format('Ymd')] = true;
  }

  $sql = "SELECT * FROM tzstats_login " .
         "WHERE tzstats_login.timestamp >= %d AND tzstats_login.timestamp < %d " .
         "ORDER BY tzstats_login.uid";

  $result = db_query($sql, $start_date->format('U'), $end_date->format('U'));
  while ($entry = db_fetch_object($result)) {
    if ($entry->uid == 1) {
      continue;
    }
    
    $date = tzbase_make_date($entry->timestamp);
    $user_bucket[$entry->uid][$date->format('Ymd')] = true;
  }

  $day_users = array();
  $month_users = array();
  foreach ($user_bucket as $uid => $bucket) {
    $day_count = count($bucket);

    if ($day_count <= 12) {
      $day_users[$uid] = $day_count;
    } else {
      $month_users[$uid] = $day_count;
    }
  }

  $output .= '<h2>Day users</h2>';
  $total_day_count = 0;
  foreach ($day_users as $uid => $day_count) {
    $account = user_load($uid);
    if ($account) {
      $output .= '<strong>' . theme('username', $account) . '</strong>: ';
    } else {
      $output .= '<strong><em>Removed user</em> (' . $uid . ')</strong>: ';
    }
    $output .= $day_count . '<br/>';
    $total_day_count += $day_count;
  }
  $output .= '<strong>Total ' . count($day_users) . ' users with ' . $total_day_count . ' days</strong>';

  $output .= '<h2>Month users</h2>';
  foreach ($month_users as $uid => $day_count) {
    $account = user_load($uid);
    if ($account) {
      $output .= '<strong>' . theme('username', $account) . '</strong>: ';
    } else {
      $output .= '<strong><em>Removed user</em> (' . $uid . ')</strong>: ';
    }
    $output .= $day_count . '<br/>';
  }
  $output .= '<strong>Total ' . count($month_users) . ' users</strong>';


  $sql = "SELECT SUM(CEIL(length/160)) FROM tzstats_sms " .
         "WHERE tzstats_sms.timestamp >= %d AND tzstats_sms.timestamp < %d";

  $result = db_query($sql, $start_date->format('U'), $end_date->format('U'));
  $sms_count = db_result($result);
  $output .= '<h2>SMS</h2>';
  $output .= $sms_count . ' messages';

  print $output;
}

function tzstats_select_date_field($now) {
  $default_start = clone($now);
  $day_of_month = intval($default_start->format('d')) - 1;
  $default_start->modify("-$day_of_month days");

  $default_end = clone($default_start);
  $default_end->modify('+1 month');
  $default_end->modify('-1 day');

  $form = array(
    '#prefix' => '<div id="edit-date-wrapper">',
    '#suffix' => '</div>',
  );
  $form['start_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('Start date'),
    '#date_format' => 'Y-m-d',
    '#default_value' => $default_start->format('Y-m-d'),
  );
  $form['end_date'] = array(
    '#type' => 'date_popup',
    '#title' => t('End date'),
    '#date_format' => 'Y-m-d',
    '#default_value' => $default_end->format('Y-m-d'),
  );

  return $form;
}