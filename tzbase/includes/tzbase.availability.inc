<?php

function tzbase_availability_form($form_state) {
  $form = array();

  drupal_add_js(drupal_get_path('module', 'tzbase') . '/javascript/tzbase.availability.js');
  drupal_add_css(drupal_get_path('module', 'tzbase') . '/style/tzbase.availability.css');

  $today = date_make_date('now');
  $form['search'] = array(
    '#type' => 'fieldset',
  );
  $form['search']['date'] = array(
    '#type' => 'date_popup',
    '#date_format' => 'Y-m-d',
    '#default_value' => $today->format('Y-m-d'),
    '#title' => t('Date'),
  );
  $form['search']['from'] = array(
    '#type' => 'textfield',
    '#size' => 6,
    '#title' => t('From'),
    '#description' => 'HH:MM',
  );
  $form['search']['to'] = array(
    '#type' => 'textfield',
    '#size' => 6,
    '#title' => t('To'),
    '#description' => 'HH:MM',
  );
  $form['search']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Search'),
  );

  $form['data'] = array('#value' => '<div id="availability-data"></div>');

  return $form;
}

function tzbase_availability_data() {
  if (empty($_GET['date'])) {
    print "";
    return;
  }

  $headers = array(
    array(),
    t('Name'),
    t('Mobile'),
    t('Manager'),
    t('Date'),
    t('Availability'),
  );

  $rows = array();
  $availability_store = tzbase_availability_store();
  $availabilities = NULL;

  if (empty($_GET['from']) || empty($_GET['to'])) {
    $date = date_make_date($_GET['date']['date']);
    $availabilities = $availability_store->findByDate($date);
  } else {
    $from = date_make_date($_GET['date']['date'] . 'T' . $_GET['from']);
    $to = date_make_date($_GET['date']['date'] . 'T' . $_GET['to']);
    $availabilities = $availability_store->findByTimeInterval($from, $to);
  }

  if (empty($availabilities)) {
    $rows[] = array(
      '',
      array(
        'data' => t('No available employees found'),
        'colspan' => 5,
      ),
    );
  } else {
    foreach ($availabilities as $availability) {
      if (!$availability->isAvailable()) {
        continue;
      }

      $user_entry = _tzbase_user_entry($availability->getUserId());
      $checkbox = _tzbase_availability_checkbox($availability->getUserId());
      $rows[] = array(
        drupal_render($checkbox),
        $user_entry->fullname,
        $user_entry->mobile,
        $user_entry->manager,
        $availability->getStartTime()->format('Y-m-d'),
        $availability->getStartTime()->format('H:i') . ' - ' . $availability->getEndTime()->format('H:i')
      );
    }
  }

  print theme('table', $headers, $rows);
}

function _tzbase_user_entry($uid) {
  static $cache = array();

  if (!empty($cache[$uid])) {
    return $cache[$uid];
  }

  $entry = new stdClass();
  $account = user_load($uid);
  $entry->fullname = $account->tzuser['fullname'];
  $entry->mobile = $account->tzuser['mobile'];
  if (!empty($account->tzuser['manager'])) {
    if ($account->tzuser['manager'] == $account->uid) {
      $entry->manager = $entry->fullname;
    } else {
      $manager = _tzbase_user_entry($account->tzuser['manager']);
      $entry->manager = $manager->fullname;
    }
  } else {
    $entry->manager = "";
  }
  $cache[$uid] = $entry;
  return $entry;
}

function _tzbase_availability_checkbox($uid) {
  return array(
    '#type' => 'checkbox',
    '#name' => "selected_users[$uid]",
    '#return_value' => $uid,
    '#id' => "selected_users_$uid",
    '#value' => 0,
    '#parents' => array(''),
  );
}