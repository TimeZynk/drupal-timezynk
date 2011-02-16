<?php

function tzuser_views_data() {
  $data['tzuser']['table'] = array(
    'group' => t('User'),
    'join' => array(
      'users' => array(
        'left_field' => 'uid',
        'field' => 'uid',
      ),
    ),
  );

  $data['tzuser']['uid'] = array(
    'title' => 'Uid',
    /* This is a filter with autocomplete on the full
     * name, so we show "full name" to the user instead
     * of Uid, even though the filter is technically on
     * the Uid.
     */
    'help' => 'Users full name',
    'filter' => array(
      'title' => 'Full name',
      'handler' => 'tzuser_handler_filter_user_name',
    ),
  );

  $data['tzuser']['fullname'] = array(
    'title' => 'Full name',
    'help' => t('Users full name'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['tzuser']['mobile'] = array(
    'title' => 'Mobile',
    'help' => t('Users mobile number'),

    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  return $data;
}

function tzuser_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'tzuser') . '/includes',
    ),
    'handlers' => array(
      'tzuser_handler_filter_user_name' => array('parent' => 'views_handler_filter_user_name'),
    )
  );
}