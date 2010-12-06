<?php

function jadmaker_views_data() {
  $data['jadmaker']['table'] = array(
    'group' => t('JAD Maker'),
    'base' => array(
      'field' => 'download_id',
      'title' => t('JAD Downloads'),
      'help' => t('Statistics over JAD file downloads'),
      'weight' => 10,
    ),
  );

  $data['jadmaker']['download_id'] = array(
    'title' => t('Download ID'),
    'help' => t('JAD Download ID'),

    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
  );

  $data['jadmaker']['timestamp'] = array(
    'title' => t('Download Timestamp'),
    'help' => t('Date and time for JAD download'),

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

  $data['jadmaker']['http_data'] = array(
    'title' => t('HTTP Variables'),
    'help' => t('HTTP variables sent from client'),

    'field' => array(
      'handler' => 'jadmaker_handler_field_http',
      'click sortable' => FALSE,
    ),
  );

  return $data;
}

/**
 * Implementation if hook_views_handlers()
 * Registers view handlers
 */
function jadmaker_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'jadmaker'),
    ),
    'handlers' => array(
      'jadmaker_handler_field_http' => array(
        'parent' => 'views_handler_field',
      ),
    ),
  );
}