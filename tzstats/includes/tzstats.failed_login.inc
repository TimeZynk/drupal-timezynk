<?php

function tzstats_failed_login_page() {
  $result = '';

  $header_wrong_pw = array('ID', 'Attempted username', 'Actual username', 'Time', 'User Agent');
  $table_rows_wrong_pw = array();

  $header_just_wrong = array('ID', 'Attempted username', 'Time', 'User Agent');
  $table_rows_just_wrong = array();

  $query = 'SELECT * FROM {tzstats_failed_login} ORDER BY id DESC';
  $db_result = db_query($query);
  while($row = db_fetch_object($db_result)) {
    $account = _tzstats_get_valid_account($row->name);
    if ($account) {
      if (arg(3) == 'all' || $account->login < $row->timestamp) {
        $table_rows_wrong_pw[] = array(
          $row->id,
          l($row->name, 'user/' .  $account->uid, array('attributes' => array('title' => $account->tzuser['fullname']))),
          $account->name,
          format_date($row->timestamp, 'small'),
          $row->user_agent,
        );
      }
    } else {
      $table_rows_just_wrong[] = array(
        $row->id,
        $row->name,
        format_date($row->timestamp, 'small'),
        $row->user_agent,
      );
    }
  }

  $result .= '<h2>Wrong password</h2>';
  $result .= theme('table', $header_wrong_pw, $table_rows_wrong_pw);
  $result .= '<h2>No such user</h2>';
  $result .= theme('table', $header_just_wrong, $table_rows_just_wrong);
  $result .= l('Show all', 'admin/reports/failed_login/all') . ' | ';
  $result .= l('Show still failing', 'admin/reports/failed_login');

  if (module_exists('tzintellitime')) {
    $result .= '<br/>' . l('Intellitime', TZINTELLITIME_BASE_URL);
  }

  return $result;
}

function _tzstats_get_valid_account($user_name) {
  if ($mobile = tzuser_validate_phone_number($user_name)) {
    $result = db_result(db_query("SELECT uid FROM {tzuser} WHERE mobile = '%s' ORDER BY uid LIMIT 1", $mobile));
    if ($result) {
      return user_load($result);
    }
  } else {
    return user_load(array('name' => $user_name));
  }
  return NULL;
}