<?php

function tzuser_username($account, $link = TRUE) {
  $name = '';
  if(!empty($account->tzuser['fullname'])) {
    $name = $account->tzuser['fullname'];
  } else {
    $name = $account->name;
  }

  if(drupal_strlen($name) > 20) {
    $name = substr($name, 0, 15) . '...';
  }

  $name = check_plain($name);

  if ($link && user_access('access user profiles')) {
    $output = l($name, 'user/' . $account->uid, array('attributes' => array('title' => t('View user profile.'))));
  } else {
    $output = $name;
  }

  return $output;
}

function theme_tzuser_user_overview($form) {
  $output = drupal_render($form['filter']);

  $header = tzuser_user_overview_headers();
  $rows = array();
  $rows[] = array(array('data' => t('Loading employees, please wait...'), 'colspan' => count($header)));

  $output .= theme('table', $header, $rows);

  $output .= drupal_render($form);
  return $output;
}

/**
 * Theme the number rows in the add user form in a nice table
 * @param array $element Element containing the number rows
 */
function theme_tzuser_add_users_rows($element) {
  // Build up number rows
  $rows = array();
  foreach($element as $rowid => $row) {
    if(isset($element[$rowid]['index']) && is_array($element[$rowid]['index'])) {
      $rows[] = array(
        drupal_render($element[$rowid]['index']),
        drupal_render($element[$rowid]['number']),
        drupal_render($element[$rowid]['username']),
        drupal_render($element[$rowid]['password']),
        drupal_render($element[$rowid]['manager']),
      );
    }
  }

  // Add our "Add more rows" button last in the table, with full colspan
  $rows[] = array(array(
    'data' => drupal_render($element['add_more']),
    'colspan' => 5,
  ));

  // Wrap in div for the AHAH-handlers sake
  $output = '<div id="tzuser_add_number_rows">';

  // Setup headers and output table
  $header = array('', t('Mobile number') . '*', t('Username') . '*', t('Password'), t('Manager'));
  $output .= theme('table', $header, $rows);

  $output .= '</div>';
  return $output;
}

function theme_time_ago($time) {
  return $time ? t('@time ago', array('@time' => format_interval(time() - $time, 1))) : t('never');
}

function theme_traffic_light($status) {
  if (is_array($status)) {
    $status = $status['#value'];
  }

  $status_table = array(
    TZUserStatus::GREY => 'grey',
    TZUserStatus::RED => 'red',
    TZUserStatus::YELLOW => 'yellow',
    TZUserStatus::GREEN => 'green',
  );
  $class = 'tzuser_status_' . $status_table[$status];
  return '<div class="tzuser_status ' . $class . '"></div>';
}

function theme_due_reports_count($count) {
  if (is_array($count)) {
    $count = $count['#value'];
  }
  if($count == TZUserStatus::NUMBER_OF_REPORTS_NOT_SET) {
    return '';
  } else if ($count == TZUserStatus::NUMBER_OF_REPORTS_UNKNOWN) {
    return t('Unknown');
  }
  return '<span class="due_reports_count">' . $count . '</span>';
}
