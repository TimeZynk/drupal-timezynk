<?php

function tzuser_support_log_page() {
  $page = '';
  drupal_add_css(drupal_get_path('module', 'tzuser') . '/style/tzuser.css');

  $result = db_query('SELECT log.*,tzu.*,u.login,u.name FROM {tzuser_support_log} log ' .
                     'INNER JOIN {tzuser} tzu ON log.uid = tzu.uid ' .
                     'INNER JOIN {users} u ON log.uid = u.uid ' .
                     'ORDER BY log.changed DESC');
  while ($row = db_fetch_object($result)) {
    $status = tzuser_get_user_status($row, time());
    $status_code = $status->getStatusCode();

    $user = '<div class="tzuser-log-entry">';
    $user .= theme('traffic_light', $status_code);
    $user .= '<div class="tzuser-log-name"><h2>' . $row->mobile . ' - ' .
             l(check_plain($row->fullname) . ' (' . $row->name . ')', 'user/' . $row->uid) .
             '</h2></div>';
    if ($row->login) {
      $user .= '<div class="entry">' . t('Last login') . ' ' . format_date($row->login, 'short') . '</div>';
    } else {
      $user .= '<div class="entry">' . t('Never logged in') . '</div>';
    }
    $user .= '<div class="entry">' . t('Last log update') . ' ' . format_date($row->changed, 'short') . '</div>';
    $user .= '<div class="log-text">' . check_markup($row->support_log) . '</div>';
    $user .= '</div>';
    $page .= $user;
  }
  return $page;
}

function tzuser_support_log_ajax($uid) {
  if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    $account = user_load($_POST['uid']);
    if (empty($account)) {
      drupal_not_found();
    }

    tzuser_save_support_log($account->uid, $_POST['support_log']);
    drupal_json(array('200' => 'OK'));
  } else {
    $log = tzuser_get_support_log($uid);
    if (empty($log)) {
      $log = (object)array(
        'uid' => $uid,
        'support_log' => '',
        'changed' => 0,
      );
    }
    return drupal_json($log);
  }
}
