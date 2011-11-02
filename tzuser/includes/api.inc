<?php

function tzuser_api_list_users() {
  $date = _tzuser_api_request_date();

  // Handle manager
  $manager = 0;
  if (!empty($_GET['manager']) && is_numeric($_GET['manager'])) {
    $manager = intval($_GET['manager']);
  }

  // Handle status filter
  $status_filter = array();
  if (isset($_GET['status'])) {
    if (is_array($_GET['status'])) {
      foreach ($_GET['status'] as $status => $value) {
        $status_filter[$status] = TRUE;
      }
    } else {
      $status_filter[$_GET['status']] = TRUE;
    }
  }

  $sql = 'SELECT * FROM {users} u INNER JOIN {tzuser} tzu ON tzu.uid = u.uid WHERE u.uid > 1 AND u.status = 1';
  if (!empty($manager)) {
    $sql .= ' AND tzu.manager = %d';
  }
  $result = db_query($sql, $manager);

  $overview = array();
  while($user_entry = db_fetch_object($result)) {
    $status = tzuser_get_user_status($user_entry, $date->format('U'));
    $status_code = $status->getStatusCode();

    if (empty($status_filter[$status_code])) {
      continue;
    }

    $checkbox = array(
      '#type' => 'checkbox',
      '#name' => "selected_users[$user_entry->uid]",
      '#return_value' => $user_entry->uid,
      '#id' => "selected_users_$user_entry->uid",
      '#value' => 0,
      '#parents' => array(''),
    );

    $operations = array();
    if (user_edit_access($user_entry)) {
      $operations[] = l(t('edit'), "user/$user_entry->uid/edit", array('query' => 'destination=tzuser'));
    }
    if (user_access('administer site configuration')) {
      $operations[] = l(t('log'), '', array('fragment' => ' ', 'external' => TRUE, 'attributes' => array('class' => 'edit-log-link')));
    }

    $overview[] = array(
      'id_value' => $user_entry->uid,
      'checkbox' => drupal_render($checkbox),
      'status' => theme('traffic_light', $status_code),
      'status_value' => $status_code,
      'name' => l($user_entry->name, "user/$user_entry->uid", array('query' => 'destination=tzuser')),
      'name_value' => $user_entry->name,
      'fullname' => $user_entry->fullname,
      'mobile' => empty($user_entry->mobile) ? '' : $user_entry->mobile,
      'due_count' => theme('due_reports_count', $status->getNumberOfDueReports()),
      'login' => theme('time_ago', $user_entry->login),
      'login_value' => $user_entry->login,
      'operations' => implode(' | ', $operations),
    );
  }

  drupal_set_header('Content-Type: application/json; charset=utf-8');
  print json_encode($overview);
}

function _tzuser_api_request_date() {
  $date = date_make_date('now');

  // Handle date setting
  $date_string = "";
  if (isset($_GET['date'])) {
    if (is_array($_GET['date']) && !empty($_GET['date']['date'])) {
      $date_string = $_GET['date']['date'];
    } else if (is_string($_GET['date'])) {
      $date_string = $_GET['date'];
    }
  }

  if (!empty($date_string) && preg_match('/\d{4}-\d{2}-\d{2}/', $date_string)) {
    $date = date_make_date($date_string . 'T00:00:00');
  } else {
    $date->setTime(0, 0, 0);
  }

  // The date is up to and including.
  $date->modify('+1 day');
  return $date;
}
