<?php

function tzuser_support_log_page() {
  $page = '';
  drupal_add_css(drupal_get_path('module', 'tzuser') . '/style/tzuser.css');
  jquery_ui_add(array('ui.core', 'ui.draggable', 'ui.dialog'));
  drupal_add_css(drupal_get_path('module', 'jquery_ui') . '/jquery.ui/themes/base/ui.all.css');
  drupal_add_js(drupal_get_path('module', 'tzuser') . '/javascript/tzuser.support_log.js');

  $result = db_query('SELECT log.*,tzu.*,u.login,u.created,u.name FROM {tzuser_support_log} log ' .
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
      $user .= '<div class="entry">' . t('Last login') . ' ' . format_date($row->login, 'small') . '</div>';
    } else {
      $user .= '<div class="entry">' . t('Never logged in') . '</div>';
    }
    $user .= '<div class="entry">' . t('User added to TimeZynk') . ' ' . format_date($row->created, 'small') . '</div>';
    $user .= '<div class="entry">' . t('Last log update') . ' ' . format_date($row->changed, 'small') . '</div>';
    $user .= '<div class="log-text">' . check_markup($row->support_log) . '</div>';
    if (user_access('administer site configuration')) {
      $user .= '<a href="#" rel="' . $row->uid . '" class="edit-log-link">' . t('edit') . '</a>';
    }
    $user .= '</div>';
    $page .= $user;
  }
  $page .= '<p>' . t('Contact us at <a href="mailto:support@timezynk.com">support@timezynk.com</a>') . '</p>';
  $page .= '<script type="text/javascript">$(function() {' .
    '$(".edit-log-link").editSupportLogLink(function() { return this.attr("rel"); }, function() {location.reload();});' .
  '});</script>';
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
