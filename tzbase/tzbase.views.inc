<?php
/**
 * @file 
 * Functions for exposing the tzbase data types to views
 */

/**
 * Expose table information to views
 */
function tzbase_views_data() {
  // View definitions for tzreport table
  // This is a node table, left join with {node}.nid and {node_revisions}.vid
  $data['tzreport']['table'] = array(
    'group' => t('Time Report'),
    'join' => array(
      'node' => array(
        'left_field' => 'nid',
        'field' => 'nid',
      ),
      'node_revisions' => array(
        'left_field' => 'vid',
        'field' => 'vid',
      ),
    ),
  );
  
  // jobid field
  $data['tzreport']['jobid'] = array(
    'title' => 'Job ID',
    'help' => t('The ID of the linked Job'),
    
    // Basic numeric display
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    
    // Link to {tzjob}.nid
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'node',
      'base field' => 'nid',
    )
  );
  
  // assignedto field
  $data['tzreport']['assignedto'] = array(
    'title' => t('Assigned user'),
    'help' => t('The user assigned to this report'),
  
    // Basic numeric display
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    
    // Link to {users}.uid
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'base field' => 'uid',
    )
  );
  
  // begintime field
  $data['tzreport']['begintime'] = array(
    'title' => t('Start time'),
    'help' => t('Time Report start time'),
    
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
    'argument' => array(
      'handler' => 'tzbase_handler_argument_begintime',
    ),
  );
  
  // endtime field
  $data['tzreport']['endtime'] = array(
    'title' => t('End time'),
    'help' => t('Time Report end time'),
    
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
  );
  
  // break duration field
  $data['tzreport']['breakduration'] = array(
    'title' => t('Break duration'),
    'help' => t('Time Report break duration'),
    
    'field' => array(
      'handler' => 'tzbase_handler_field_interval',
      'click sortable' => FALSE,
    ),
  );
  
  // Computed total duration
  $data['tzreport']['totalduration'] = array(
    'title' => t('Total duration'),
    'help' => t('Time Report total duration'),
  
    'field' => array(
      'handler' => 'tzbase_handler_field_total_duration',
      'click sortable' => TRUE,
    ),
  );
  
  // flags field
  $data['tzreport']['flags'] = array(
    'title' => 'Flags',
    'help' => t('Status flags'),

    // Basic numeric display
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );
  
  // View definitions for tzjob table
  // This is a node table, left join with {node}.nid and {node_revisions}.vid
  $data['tzjob']['table'] = array(
    'group' => t('Job'),
    'join' => array(
      'node' => array(
        'left_field' => 'nid',
        'field' => 'nid',
      ),
      'node_revisions' => array(
        'left_field' => 'vid',
        'field' => 'vid',
      ),
    ),
  );
  
  return $data;
}

/**
 * Implementation if hook_views_handlers()
 * Registers view handlers
 */
function tzbase_views_handlers() {
  $data['handlers']['tzbase_handler_field_interval'] = array(
    'parent' => 'views_handler_field',
    'file' => 'tzbase_views_handlers.inc',
  );
  
  $data['handlers']['tzbase_handler_field_total_duration'] = array(
    'parent' => 'tzbase_handler_field_interval',
    'file' => 'tzbase_views_handlers.inc',
  );
  
  $data['handlers']['tzbase_handler_argument_begintime'] = array(
    'parent' => 'views_handler_argument',
  );
  return $data;
}

/**
 * Prints a duration as HH:MM
 * @param int $duration number of seconds in duration
 */
function tzbase_views_format_duration($duration) {
    $hours = (int)($duration/3600);
    $minutes = (int)(($duration - $hours*3600)/60);
    return sprintf("%02d:%02d", $hours, $minutes);
}