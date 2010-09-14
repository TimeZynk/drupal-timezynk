<?php
/**
 * @file Settings for the administration page
 */



/**
 * Implementation of settings form
 */
function tzbase_admin_content() {
  $form['import_legacy'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Import legacy nodes'),
    '#options' => array('cck' => 'Import legacy TimeZynk CCK content'),
  );
  $form['delete_all'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Delete all TimeZynk content'),
    '#options' => array('all' => 'Delete all TimeZynk nodes (tzjob and tzreport)'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

function tzbase_admin_content_submit($form, $form_state) {
  $import_legacy = array_filter($form_state['values']['import_legacy']);
  $delete_all = array_filter($form_state['values']['delete_all']);
  if(in_array('cck', $import_legacy)) {
    $batch = array(
      'operations' => array(
        array('tzbase_import_assignments', array()),
        array('tzbase_import_timereports', array()),
      ),
      'finished' => 'tzbase_import_finished',
      'title' => 'Importing legacy CCK nodes',
      'progress_message' => t('Processed @current of @total'),
      'error_message' => t('Error while importing nodes'),
      'file' => dirname(__FILE__) . '/tzbase.import.inc',
    );
    batch_set($batch);
  }
  else if(in_array('all', $delete_all)) {
    // Find all nodes
    $result = db_query("SELECT nid FROM {node} WHERE type = 'tzreport' OR type = 'tzjob'");
    while($data = db_fetch_object($result)) {
      node_delete($data->nid);
    }
  }
}
