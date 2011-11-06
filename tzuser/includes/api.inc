<?php

function tzuser_api_access() {
  $access = array();
  if (isset($_GET['permission'])) {
    if (is_array($_GET['permission'])) {
      foreach ($_GET['permission'] as $perm) {
        $access[$perm] = user_access($perm);
      }
    } else {
      $access[$_GET['permission']] = user_access($_GET['permission']);
    }
  }

  drupal_set_header('Content-Type: application/json; charset=utf-8');
  print json_encode($access);
}

function tzuser_api_list_users() {
  $date = _tzuser_api_list_users_request_date();

  // Handle manager
  $manager = 0;
  if (!empty($_GET['manager']) && is_numeric($_GET['manager'])) {
    $manager = intval($_GET['manager']);
  }

  // Handle status filter
  $status_filter = array();
  if (isset($_GET['status'])) {
    if (is_array($_GET['status'])) {
      foreach ($_GET['status'] as $status) {
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

    $operations = array();
    if (user_edit_access($user_entry)) {
      $operations[t('edit')] = url("user/$user_entry->uid/edit", array('query' => 'destination=tzuser'));
    }
    if (user_access('administer site configuration')) {
      $operations[t('log')] = url("", array('fragment' => ' ', 'external' => TRUE, 'attributes' => array('class' => 'edit-log-link')));
    }

    $overview[] = array(
      'id' => intval($user_entry->uid),
      'username' => $user_entry->name,
      'fullname' => $user_entry->fullname,
      'mobile' => empty($user_entry->mobile) ? '' : $user_entry->mobile,
      'status' => $status_code,
      'status_name' => tzuser_get_user_status_name($status_code),
      'due_count' => $status->getNumberOfDueReports(),
      'last_login' => intval($user_entry->login, 10),
      'user_agent' => $user_entry->user_agent,
      'client_version' => $user_entry->version_code ? intval($user_entry->version_code, 10) : NULL,
      'manager_id' => empty($user_entry->manager) ? NULL : intval($user_entry->manager),
      'manager_url' => empty($user_entry->manager) ? NULL : url('api/users/' . $user_entry->manager, array('absolute' => TRUE)),
      'url' => url('api/users/' . $user_entry->uid, array('absolute' => TRUE)),
    );
  }

  drupal_set_header('Content-Type: application/json; charset=utf-8');
  print json_encode($overview);
}

function _tzuser_api_list_users_request_date() {
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
