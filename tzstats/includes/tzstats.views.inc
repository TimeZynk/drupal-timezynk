<?php

function tzstats_views_data() {
  $data['tzstats_sync']['table'] = array(
    'group' => t('Intellitime'),
    'base' => array(
      'field' => 'id',
      'title' => t('Intellitime Synchronizations'),
      'help' => t('Statistics over Intellitime synchronizations'),
      'weight' => 10,
    ),
  );

  $data['tzstats_sync']['id'] = array(
    'title' => t('Sync. ID'),
    'help' => t('Intellitime sync. ID'),

    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
  );

  $data['tzstats_sync']['timestamp'] = array(
    'title' => t('Synch Timestamp'),
    'help' => t('Date and time for sync.'),

    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  $data['tzstats_sync']['module'] = array(
    'title' => t('Calling module'),
    'help' => t('Name of the module causing synch'),

    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
     ),
  );

  $data['tzstats_sync']['count'] = array(
    'title' => t('User count'),
    'help' => t('Number of users for which sync. was attempted.'),

    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
     ),
  );

  $data['tzstats_sync']['successful'] = array(
    'title' => t('# of successful syncs'),
    'help' => t('Number of users for which sync. was successful.'),

    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
     ),
  );

  $data['tzstats_sync']['seconds'] = array(
    'title' => t('Time to sync.'),
    'help' => t('Number of seconds the synchronization took.'),

    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
     ),
  );

  $data['tzstats_sms']['table'] = array(
    'group' => t('SMS'),
    'base' => array(
      'field' => 'id',
      'title' => t('SMS statistics'),
      'help' => t('Statistics over SMS'),
      'weight' => 10,
    ),
  );

  $data['tzstats_sms']['uid'] = array(
    'title' => t('User ID'),
    'help' => t('User ID'),

    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'base field' => 'uid',
    )
  );

  $data['tzstats_sms']['timestamp'] = array(
    'title' => t('Timestamp'),
    'help' => t('Date and time for SMS.'),

    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  $data['tzstats_sms']['sms_type'] = array(
    'title' => t('SMS Type'),
    'help' => t('SMS Type'),

    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  $data['tzstats_sms']['length'] = array(
    'title' => t('SMS length'),
    'help' => t('SMS length'),

    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
  );

  $data['tzstats_sms']['destination'] = array(
    'title' => t('Destination'),
    'help' => t('Destination mobile number'),

    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
  );

  return $data;
}
