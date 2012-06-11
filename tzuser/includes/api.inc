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

function tzuser_api_users() {
  switch ($_SERVER['REQUEST_METHOD']) {
    case 'GET':
      return tzuser_api_list_users();
    case 'POST':
      return tzuser_api_create_user();
    case 'DELETE':
      return tzuser_api_delete_user();
  }
}

function tzuser_api_list_users() {
  $date = _tzuser_api_list_users_request_date();
  $single = FALSE;

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
  $sql_args = array();
  if (arg(2)) {
    $sql .= ' AND tzu.uid = %d';
    $sql_args[] = arg(2);
    $single = TRUE;
  } else if (!empty($manager)) {
    $sql .= ' AND tzu.manager = %d';
    $sql_args[] = $manager;
  }

  $result = db_query($sql, $sql_args);

  $overview = array();
  $excluded_users = array();
  if (module_exists('tzstats')) {
    $excluded_users = tzstats_invoice_excluded_users();
  }

  while($user_entry = db_fetch_object($result)) {
    $status = tzuser_get_user_status($user_entry, $date->format('U'));
    $status_code = $status->getStatusCode();

    if (!$single && empty($status_filter[$status_code])) {
      continue;
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
      'url' => url('api/users/' . $user_entry->uid),
      'email' => $user_entry->mail,
      'user_agent' => $user_entry->user_agent,
      'client_version' => $user_entry->version_code ? intval($user_entry->version_code, 10) : NULL,
      'manager' => empty($user_entry->manager) ? NULL : array(
        'id' => intval($user_entry->manager),
        'url' => url('api/users/' . $user_entry->manager),
      ),
      'roles' => tzuser_user_roles($user_entry->uid),
      'created' => intval($user_entry->created),
      'invoice' => empty($excluded_users[$user_entry->uid]) ? TRUE : FALSE,
    );
  }

  drupal_alter('user_list', $overview);

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

function tzuser_api_create_user() {
  $result = array();
  // Read POST data
  $raw_data = file_get_contents('php://input');
  $users = json_decode($raw_data);
  if (!is_array($users)) {
    $users = array($users);
  }

  foreach ($users as $u) {
    $account = tzuser_add_user(array(
      'username' => $u->username,
      'fullname' => $u->fullname,
      'mobile' => $u->mobile,
      'password' => $u->password,
      'manager' => $u->manager,
      'email' => $u->email,
    ));

    if ($account) {
      module_invoke_all('tzuser_add_user_notify', $account, $u->password);
      $result[$u->mobile]['message'] = t('Created employee @username with mobile @number',
                                             array('@username' => $account->name,
                                                   '@number' => $account->tzuser['mobile']));
      watchdog('tzuser', $result[$u->mobile]['message']);
    } else {
      $result[$u->mobile]['error'] = t('Failed to add employee @username with mobile @number',
                                           array('@username' => $u>username,
                                                 '@number' => $u->mobile));
      watchdog('tzuser', $result[$u->mobile]['message'], WATCHDOG_WARNING);
    }
  }
  print drupal_json($result);
}

function tzuser_api_delete_user() {
  $account = user_load(array('uid' => (int) arg(2)));
  if (!$account) {
    drupal_not_found();
    return;
  }

  if ($account->status == 1) {
    user_save($account, array('status' => 0));
  }

  print drupal_json(array('deleted' => $uid));
}

function tzuser_api_list_managers() {
  $managers = array();
  $result = db_query('SELECT * FROM {users_roles} role ' .
    'INNER JOIN {tzuser} tz ON role.uid = tz.uid ' .
    'WHERE rid = %d ORDER BY tz.fullname', tzuser_manager_role_id());
  while ($entry = db_fetch_object($result)) {
    $managers[] = array(
      'user_id' => $entry->uid,
      'fullname' => $entry->fullname
    );
  }
  drupal_set_header('Content-Type: application/json; charset=utf-8');
  print json_encode($managers);
}
