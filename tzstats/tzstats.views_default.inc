<?php

/**
 * Implementation of hook_views_default_views().
 */
function tzstats_views_default_views() {
  $views = array();
  $view = new view;
  $view->name = 'tzstats_sync';
  $view->description = '';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'tzstats_sync';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
  'id' => array(
    'label' => 'Sync. ID',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
  ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'set_precision' => FALSE,
    'precision' => 0,
    'decimal' => '.',
    'separator' => ',',
    'prefix' => '',
    'suffix' => '',
    'exclude' => 0,
    'id' => 'id',
    'table' => 'tzstats_sync',
    'field' => 'id',
    'relationship' => 'none',
  ),
  'timestamp' => array(
    'label' => 'Synch Timestamp',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
  ),
    'empty' => 'N/A',
    'hide_empty' => 0,
    'empty_zero' => 1,
    'date_format' => 'small',
    'custom_date_format' => '',
    'exclude' => 0,
    'id' => 'timestamp',
    'table' => 'tzstats_sync',
    'field' => 'timestamp',
    'relationship' => 'none',
  ),
  'module' => array(
    'label' => 'Calling module',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
  ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'exclude' => 0,
    'id' => 'module',
    'table' => 'tzstats_sync',
    'field' => 'module',
    'relationship' => 'none',
  ),
  'successful' => array(
    'label' => '# of successful syncs',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
  ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'set_precision' => FALSE,
    'precision' => 0,
    'decimal' => '.',
    'separator' => ',',
    'prefix' => '',
    'suffix' => '',
    'exclude' => 0,
    'id' => 'successful',
    'table' => 'tzstats_sync',
    'field' => 'successful',
    'relationship' => 'none',
  ),
  'count' => array(
    'label' => 'User count',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => '',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
  ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'set_precision' => FALSE,
    'precision' => 0,
    'decimal' => '.',
    'separator' => ',',
    'prefix' => '',
    'suffix' => '',
    'exclude' => 0,
    'id' => 'count',
    'table' => 'tzstats_sync',
    'field' => 'count',
    'relationship' => 'none',
  ),
  'seconds' => array(
    'label' => 'Time to sync.',
    'alter' => array(
      'alter_text' => 0,
      'text' => '',
      'make_link' => 0,
      'path' => '',
      'link_class' => '',
      'alt' => '',
      'prefix' => '',
      'suffix' => 's',
      'target' => '',
      'help' => '',
      'trim' => 0,
      'max_length' => '',
      'word_boundary' => 1,
      'ellipsis' => 1,
      'html' => 0,
      'strip_tags' => 0,
  ),
    'empty' => '',
    'hide_empty' => 0,
    'empty_zero' => 0,
    'set_precision' => FALSE,
    'precision' => 0,
    'decimal' => '.',
    'separator' => ',',
    'prefix' => '',
    'suffix' => '',
    'exclude' => 0,
    'id' => 'seconds',
    'table' => 'tzstats_sync',
    'field' => 'seconds',
    'relationship' => 'none',
  ),
  ));
  $handler->override_option('sorts', array(
  'timestamp' => array(
    'order' => 'DESC',
    'granularity' => 'second',
    'id' => 'timestamp',
    'table' => 'tzstats_sync',
    'field' => 'timestamp',
    'relationship' => 'none',
  ),
  ));
  $handler->override_option('access', array(
  'type' => 'role',
  'role' => array(
    '2' => 2,
  ),
  ));
  $handler->override_option('cache', array(
  'type' => 'none',
  ));
  $handler->override_option('title', 'Intellitime Sync stats');
  $handler->override_option('items_per_page', 100);
  $handler->override_option('use_pager', 'mini');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
  'grouping' => '',
  'override' => 1,
  'sticky' => 1,
  'order' => 'desc',
  'columns' => array(
    'id' => 'id',
    'timestamp' => 'timestamp',
    'module' => 'module',
    'successful' => 'successful',
    'count' => 'count',
    'seconds' => 'seconds',
  ),
  'info' => array(
    'id' => array(
      'sortable' => 1,
      'separator' => '',
  ),
    'timestamp' => array(
      'sortable' => 1,
      'separator' => '',
  ),
    'module' => array(
      'sortable' => 1,
      'separator' => '',
  ),
    'successful' => array(
      'sortable' => 1,
      'separator' => '',
  ),
    'count' => array(
      'sortable' => 1,
      'separator' => '',
  ),
    'seconds' => array(
      'sortable' => 1,
      'separator' => '',
  ),
  ),
  'default' => 'timestamp',
  ));
  $handler->override_option('row_options', array(
  'inline' => array(
    'id' => 'id',
    'timestamp' => 'timestamp',
  ),
  'separator' => '',
  'hide_empty' => 0,
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'tzstats');
  $handler->override_option('menu', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => '0',
  'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
  'type' => 'none',
  'title' => '',
  'description' => '',
  'weight' => 0,
  'name' => 'navigation',
  ));

  $views[$view->name] = $view;
  return $views;
}