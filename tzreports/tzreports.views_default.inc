<?php

/**
 * Implementation of hook_views_default_views().
 */
function tzreports_views_default_views() {
  $views = array();

  $view = _tzreports_timereports_view();
  $views[$view->name] = $view;

  $view = _tzreports_timereports_by_assignment_view();
  $views[$view->name] = $view;

  $view = _tzreports_assignments_view();
  $views[$view->name] = $view;

  return $views;
}

function _tzreports_timereports_view() {
  $view = new view;
  $view->name = 'salary_report';
  $view->description = t('Time Report Overview');
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Default', 'default');
  $handler->override_option('relationships', array(
    'assignedto' => array(
      'label' => t('Assigned user'),
      'required' => 0,
      'id' => 'assignedto',
      'table' => 'tzreport',
      'field' => 'assignedto',
      'relationship' => 'none',
    ),
    'jobid' => array(
      'label' => t('Job ID'),
      'required' => 0,
      'id' => 'jobid',
      'table' => 'tzreport',
      'field' => 'jobid',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('fields', array(
    'fullname' => array(
      'label' => t('Full name'),
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
      'empty' => '<em>Unknown</em>',
      'hide_empty' => 0,
      'empty_zero' => 0,
      'exclude' => 0,
      'link_to_user' => 1,
      'overwrite_anonymous' => 0,
      'anonymous_text' => '',
      'id' => 'fullname',
      'table' => 'tzuser',
      'field' => 'fullname',
      'relationship' => 'assignedto',
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'assignedto' => array(
      'label' => t('Assigned user'),
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
      'exclude' => 1,
      'id' => 'assignedto',
      'table' => 'tzreport',
      'field' => 'assignedto',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'title_1' => array(
      'label' => t('Assignment'),
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
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'title_1',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'jobid',
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'begintime_1' => array(
      'label' => t('Date'),
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
      'date_format' => 'custom',
      'custom_date_format' => 'Y-m-d',
      'exclude' => 0,
      'id' => 'begintime_1',
      'table' => 'tzreport',
      'field' => 'begintime',
      'relationship' => 'none',
    ),
    'begintime' => array(
      'label' => t('Start time'),
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
      'date_format' => 'custom',
      'custom_date_format' => 'H:i',
      'exclude' => 0,
      'id' => 'begintime',
      'table' => 'tzreport',
      'field' => 'begintime',
      'relationship' => 'none',
    ),
    'endtime' => array(
      'label' => t('End time'),
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
      'date_format' => 'custom',
      'custom_date_format' => 'H:i',
      'exclude' => 0,
      'id' => 'endtime',
      'table' => 'tzreport',
      'field' => 'endtime',
      'relationship' => 'none',
    ),
    'breakduration' => array(
      'label' => t('Break duration'),
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
      'date_format' => 'raw time span',
      'custom_date_format' => 'H:i',
      'id' => 'breakduration',
      'table' => 'tzreport',
      'field' => 'breakduration',
      'relationship' => 'none',
    ),
    'totalduration' => array(
      'label' => t('Total duration'),
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
      'id' => 'totalduration',
      'table' => 'tzreport',
      'field' => 'totalduration',
      'relationship' => 'none',
    ),
    'flags' => array(
      'id' => 'flags',
      'table' => 'tzreport',
      'field' => 'flags',
    ),
    'edit_node' => array(
      'label' => '',
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
      'text' => '',
      'exclude' => 0,
      'id' => 'edit_node',
      'table' => 'node',
      'field' => 'edit_node',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'tzreport' => 'tzreport',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'status' => array(
      'operator' => '=',
      'value' => '1',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'relationship' => 'none',
    ),
    'flags' => array(
      'operator' => '!=',
      'value' => array(
        'value' => '255',
        'min' => '',
        'max' => '',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'flags',
      'table' => 'tzreport',
      'field' => 'flags',
      'relationship' => 'none',
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'date_filter' => array(
      'operator' => 'between',
      'value' => array(
        'min' => NULL,
        'max' => NULL,
        'value' => NULL,
        'default_date' => '',
        'default_to_date' => '',
      ),
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'operator' => 'date_filter_op',
        'label' => t('Date'),
        'use_operator' => 0,
        'identifier' => 'date_filter',
        'optional' => 1,
        'remember' => 0,
      ),
      'date_fields' => array(
        'tzreport.begintime' => 'tzreport.begintime',
      ),
      'date_method' => 'OR',
      'granularity' => 'day',
      'form_type' => 'date_popup',
      'default_date' => '',
      'default_to_date' => '',
      'year_range' => '-3:+3',
      'id' => 'date_filter',
      'table' => 'node',
      'field' => 'date_filter',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'title' => array(
      'operator' => 'contains',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'title_op',
        'identifier' => 'assignment',
        'label' => t('Assignment'),
        'optional' => 1,
        'remember' => 0,
      ),
      'case' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'jobid',
    ),
    'uid_1' => array(
      'operator' => 'in',
      'value' => '',
      'group' => '0',
      'exposed' => TRUE,
      'expose' => array(
        'use_operator' => 0,
        'operator' => 'uid_1_op',
        'identifier' => 'uid_1',
        'label' => t('Assigned user'),
        'optional' => 1,
        'remember' => 0,
        'reduce' => 0,
      ),
      'id' => 'uid_1',
      'table' => 'tzuser',
      'field' => 'uid',
      'relationship' => 'assignedto',
      'override' => array(
        'button' => 'Override',
      ),
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'use timezynk report generator',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', t('Time Reports'));
  $handler->override_option('items_per_page', 0);
  $handler->override_option('style_plugin', 'views_calc');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'fullname' => 'fullname',
      'assignedto' => 'assignedto',
      'title_1' => 'title_1',
      'begintime_1' => 'begintime_1',
      'begintime' => 'begintime',
      'endtime' => 'endtime',
      'breakduration' => 'breakduration',
      'totalduration' => 'totalduration',
      'flags' => 'flags',
      'edit_node' => 'edit_node',
    ),
    'info' => array(
      'fullname' => array(
        'sortable' => 0,
        'separator' => '',
        'justification' => 'views_calc_justify_none',
        'has_calc' => 0,
        'calc' => array(),
      ),
      'assignedto' => array(
        'sortable' => 0,
        'separator' => '',
        'justification' => 'views_calc_justify_none',
        'has_calc' => 0,
        'calc' => array(),
      ),
      'title_1' => array(
        'sortable' => 1,
        'separator' => '',
        'justification' => 'views_calc_justify_none',
        'has_calc' => 0,
        'calc' => array(),
      ),
      'begintime_1' => array(
        'sortable' => 1,
        'separator' => '',
        'justification' => 'views_calc_justify_none',
        'has_calc' => 0,
        'calc' => array(),
      ),
      'begintime' => array(
        'sortable' => 0,
        'separator' => '',
        'justification' => 'views_calc_justify_right',
        'has_calc' => 0,
        'calc' => array(),
      ),
      'endtime' => array(
        'sortable' => 0,
        'separator' => '',
        'justification' => 'views_calc_justify_right',
        'has_calc' => 0,
        'calc' => array(),
      ),
      'breakduration' => array(
        'separator' => '',
        'justification' => 'views_calc_justify_right',
        'has_calc' => 0,
        'calc' => array(),
      ),
      'totalduration' => array(
        'sortable' => 0,
        'separator' => '',
        'justification' => 'views_calc_justify_right',
        'has_calc' => 1,
        'calc' => array(
          'SUM' => 'SUM',
        ),
      ),
      'flags' => array(
        'sortable' => 1,
        'separator' => '',
        'justification' => 'views_calc_justify_none',
        'has_calc' => 0,
        'calc' => array(),
      ),
      'edit_node' => array(
        'separator' => '',
        'justification' => 'views_calc_justify_none',
        'has_calc' => 0,
        'calc' => array(),
      ),
    ),
    'default' => 'begintime_1',
    'detailed_values' => '0',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'timereports');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => t('Time Reports'),
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
  $handler = $view->new_display('page', 'Tab', 'page_2');
  $handler->override_option('path', 'timereports/list');
  $handler->override_option('menu', array(
    'type' => 'default tab',
    'title' => t('All'),
    'description' => '',
    'weight' => '-20',
    'name' => 'navigation',
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'description' => '',
    'weight' => 0,
    'name' => 'navigation',
  ));

  return $view;
}

function _tzreports_timereports_by_assignment_view() {
  $view = new view;
  $view->name = 'invoice_report';
  $view->description = 'Invoice overview';
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Default', 'default');
  $handler->override_option('relationships', array(
    'assignedto' => array(
      'label' => t('Assigned user'),
      'required' => 0,
      'id' => 'assignedto',
      'table' => 'tzreport',
      'field' => 'assignedto',
      'relationship' => 'none',
    ),
    'jobid' => array(
      'label' => t('Job ID'),
      'required' => 0,
      'id' => 'jobid',
      'table' => 'tzreport',
      'field' => 'jobid',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('fields', array(
    'fullname' => array(
      'id' => 'fullname',
      'table' => 'tzuser',
      'field' => 'fullname',
      'relationship' => 'assignedto',
    ),
    'body' => array(
      'label' => t('Comment'),
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
      'hide_empty' => 1,
      'empty_zero' => 0,
      'exclude' => 0,
      'id' => 'body',
      'table' => 'node_revisions',
      'field' => 'body',
      'relationship' => 'none',
    ),
    'begintime_1' => array(
      'label' => t('Date'),
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
      'date_format' => 'custom',
      'custom_date_format' => 'Y-m-d',
      'exclude' => 0,
      'id' => 'begintime_1',
      'table' => 'tzreport',
      'field' => 'begintime',
      'relationship' => 'none',
    ),
    'begintime' => array(
      'label' => t('Start time'),
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
      'date_format' => 'custom',
      'custom_date_format' => 'H:i',
      'exclude' => 0,
      'id' => 'begintime',
      'table' => 'tzreport',
      'field' => 'begintime',
      'relationship' => 'none',
    ),
    'endtime' => array(
      'label' => t('End time'),
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
      'date_format' => 'custom',
      'custom_date_format' => 'H:i',
      'exclude' => 0,
      'id' => 'endtime',
      'table' => 'tzreport',
      'field' => 'endtime',
      'relationship' => 'none',
    ),
    'breakduration' => array(
      'label' => t('Break duration'),
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
      'date_format' => 'raw time span',
      'custom_date_format' => 'H:i',
      'id' => 'breakduration',
      'table' => 'tzreport',
      'field' => 'breakduration',
      'relationship' => 'none',
    ),
    'totalduration' => array(
      'label' => t('Total duration'),
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
      'id' => 'totalduration',
      'table' => 'tzreport',
      'field' => 'totalduration',
      'relationship' => 'none',
    ),
    'edit_node' => array(
      'label' => '',
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
      'text' => '',
      'exclude' => 0,
      'id' => 'edit_node',
      'table' => 'node',
      'field' => 'edit_node',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('arguments', array(
    'nid' => array(
      'default_action' => 'summary asc',
      'style_plugin' => 'default_summary',
      'style_options' => array(
        'count' => 1,
        'override' => 1,
        'items_per_page' => '25',
      ),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => 'Time Report summary for %1',
      'breadcrumb' => '',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'node',
      'validate_fail' => 'summary asc',
      'break_phrase' => 0,
      'not' => 0,
      'id' => 'nid',
      'table' => 'node',
      'field' => 'nid',
      'validate_user_argument_type' => 'uid',
      'validate_user_roles' => array(
        '2' => 0,
        '3' => 0,
        '4' => 0,
      ),
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'jobid',
      'default_options_div_prefix' => '',
      'default_argument_user' => 0,
      'default_argument_fixed' => '',
      'default_argument_image_size' => '_original',
      'default_argument_php' => '',
      'validate_argument_node_type' => array(
        'tzjob' => 'tzjob',
        'image' => 0,
        'tzreport' => 0,
        'panel' => 0,
        'assignment' => 0,
        'company' => 0,
        'page' => 0,
        'profile' => 0,
        'story' => 0,
        'timereport' => 0,
      ),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_user_restrict_roles' => 0,
      'image_size' => array(
        '_original' => '_original',
        'thumbnail' => 'thumbnail',
        'preview' => 'preview',
      ),
      'validate_argument_is_member' => 0,
      'validate_argument_php' => '',
    ),
    'begintime' => array(
      'default_action' => 'ignore',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '',
      'breadcrumb' => '',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'operator' => '0',
      'id' => 'begintime',
      'table' => 'tzreport',
      'field' => 'begintime',
      'validate_user_argument_type' => 'uid',
      'validate_user_roles' => array(
        '2' => 0,
        '3' => 0,
        '4' => 0,
      ),
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_argument_user' => 0,
      'default_argument_fixed' => '',
      'default_argument_image_size' => '_original',
      'default_argument_php' => '',
      'validate_argument_node_type' => array(
        'image' => 0,
        'tzreport' => 0,
        'tzjob' => 0,
        'panel' => 0,
        'assignment' => 0,
        'company' => 0,
        'page' => 0,
        'profile' => 0,
        'story' => 0,
        'timereport' => 0,
      ),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_user_restrict_roles' => 0,
      'image_size' => array(
        '_original' => '_original',
        'thumbnail' => 'thumbnail',
        'preview' => 'preview',
      ),
      'validate_argument_is_member' => 0,
      'validate_argument_php' => '',
    ),
    'begintime_1' => array(
      'default_action' => 'ignore',
      'style_plugin' => 'default_summary',
      'style_options' => array(),
      'wildcard' => 'all',
      'wildcard_substitution' => 'All',
      'title' => '',
      'breadcrumb' => '',
      'default_argument_type' => 'fixed',
      'default_argument' => '',
      'validate_type' => 'none',
      'validate_fail' => 'not found',
      'operator' => '1',
      'id' => 'begintime_1',
      'table' => 'tzreport',
      'field' => 'begintime',
      'validate_user_argument_type' => 'uid',
      'validate_user_roles' => array(
        '2' => 0,
        '3' => 0,
        '4' => 0,
      ),
      'relationship' => 'none',
      'default_options_div_prefix' => '',
      'default_argument_user' => 0,
      'default_argument_fixed' => '',
      'default_argument_image_size' => '_original',
      'default_argument_php' => '',
      'validate_argument_node_type' => array(
        'image' => 0,
        'tzreport' => 0,
        'tzjob' => 0,
        'panel' => 0,
        'assignment' => 0,
        'company' => 0,
        'page' => 0,
        'profile' => 0,
        'story' => 0,
        'timereport' => 0,
      ),
      'validate_argument_node_access' => 0,
      'validate_argument_nid_type' => 'nid',
      'validate_user_restrict_roles' => 0,
      'image_size' => array(
        '_original' => '_original',
        'thumbnail' => 'thumbnail',
        'preview' => 'preview',
      ),
      'validate_argument_is_member' => 0,
      'validate_argument_php' => '',
    ),
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'tzreport' => 'tzreport',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'flags' => array(
      'operator' => '!=',
      'value' => array(
        'value' => '255',
        'min' => '',
        'max' => '',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'flags',
      'table' => 'tzreport',
      'field' => 'flags',
      'relationship' => 'none',
    ),
    'status' => array(
      'operator' => '=',
      'value' => '1',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'use timezynk report generator',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', t('Invoice overview'));
  $handler->override_option('items_per_page', 0);
  $handler->override_option('style_plugin', 'views_calc');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'realname' => 'realname',
      'title' => 'title',
      'body' => 'body',
      'begintime_1' => 'begintime_1',
      'begintime' => 'begintime',
      'endtime' => 'endtime',
      'breakduration' => 'breakduration',
      'totalduration' => 'totalduration',
      'edit_node' => 'edit_node',
    ),
    'info' => array(
      'realname' => array(
        'sortable' => 1,
        'separator' => '',
        'justification' => 'views_calc_justify_none',
        'has_calc' => 0,
        'calc' => array(),
      ),
      'title' => array(
        'sortable' => 1,
        'separator' => '',
        'justification' => 'views_calc_justify_none',
        'has_calc' => 0,
        'calc' => array(),
      ),
      'body' => array(
        'separator' => '',
        'justification' => 'views_calc_justify_none',
        'has_calc' => 0,
        'calc' => array(),
      ),
      'begintime_1' => array(
        'sortable' => 1,
        'separator' => '',
        'justification' => 'views_calc_justify_none',
        'has_calc' => 0,
        'calc' => array(),
      ),
      'begintime' => array(
        'sortable' => 0,
        'separator' => '',
        'justification' => 'views_calc_justify_right',
        'has_calc' => 0,
        'calc' => array(),
      ),
      'endtime' => array(
        'sortable' => 0,
        'separator' => '',
        'justification' => 'views_calc_justify_right',
        'has_calc' => 0,
        'calc' => array(),
      ),
      'breakduration' => array(
        'separator' => '',
        'justification' => 'views_calc_justify_right',
        'has_calc' => 0,
        'calc' => array(),
      ),
      'totalduration' => array(
        'sortable' => 1,
        'separator' => '',
        'justification' => 'views_calc_justify_right',
        'has_calc' => 1,
        'calc' => array(
          'SUM' => 'SUM',
        ),
      ),
      'edit_node' => array(
        'separator' => '',
        'justification' => 'views_calc_justify_none',
        'has_calc' => 0,
        'calc' => array(),
      ),
    ),
    'default' => 'begintime_1',
    'detailed_values' => '0',
  ));

  return $view;
}

function _tzreports_assignments_view() {
  $view = new view;
  $view->name = 'assignments';
  $view->description = t('Assignments');
  $view->tag = '';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Default', 'default');
  $handler->override_option('relationships', array(
    'parentid' => array(
      'label' => t('Parent'),
      'required' => 0,
      'id' => 'parentid',
      'table' => 'tzjob',
      'field' => 'parentid',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('fields', array(
    'title' => array(
      'label' => t('Title'),
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
      'link_to_node' => 0,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'flags' => array(
      'label' => t('State'),
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
      'id' => 'flags',
      'table' => 'tzjob',
      'field' => 'flags',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'edit_node' => array(
      'label' => '',
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
      'text' => '',
      'exclude' => 0,
      'id' => 'edit_node',
      'table' => 'node',
      'field' => 'edit_node',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'title_1' => array(
      'label' => t('Parent assignment'),
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
      'empty' => t('Top level'),
      'hide_empty' => 1,
      'empty_zero' => 0,
      'link_to_node' => 0,
      'exclude' => 1,
      'id' => 'title_1',
      'table' => 'node',
      'field' => 'title',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'parentid',
    ),
  ));
  $handler->override_option('sorts', array(
    'title' => array(
      'order' => 'ASC',
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'type' => array(
      'operator' => 'in',
      'value' => array(
        'tzjob' => 'tzjob',
      ),
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'type',
      'table' => 'node',
      'field' => 'type',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
    'status' => array(
      'operator' => '=',
      'value' => '1',
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'override' => array(
        'button' => 'Override',
      ),
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'perm',
    'perm' => 'use timezynk report generator',
  ));
  $handler->override_option('cache', array(
    'type' => 'none',
  ));
  $handler->override_option('title', t('Assignments'));
  $handler->override_option('items_per_page', 0);
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => 'title_1',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'title' => 'title',
      'flags' => 'flags',
      'edit_node' => 'edit_node',
      'title_1' => 'title_1',
    ),
    'info' => array(
      'title' => array(
        'sortable' => 0,
        'separator' => '',
      ),
      'flags' => array(
        'sortable' => 0,
        'separator' => '',
      ),
      'edit_node' => array(
        'separator' => '',
      ),
      'title_1' => array(
        'sortable' => 0,
        'separator' => '',
      ),
    ),
    'default' => '-1',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'assignments');
  $handler->override_option('menu', array(
    'type' => 'normal',
    'title' => t('Assignments'),
    'description' => t('Show and edit available assignments'),
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
  return $view;
}