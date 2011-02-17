<?php

function tzuser_username($account) {
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

  if (user_access('access user profiles')) {
    $output = l($name, 'user/' . $account->uid, array('attributes' => array('title' => t('View user profile.'))));
  } else {
    $output = $name;
  }

  return $output;
}