<?php
/**
 * @file Settings for the administration page
 */

/**
 * Page callback for tzbase admin/settings page
 */
function tzbase_admin_settings_block() {
  return theme('admin_block_content', system_admin_menu_block(menu_get_item()));
}

/**
 * Implementation of settings form
 */
function tzbase_admin_content() {
  $form['delete_all'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Delete all TimeZynk content'),
    '#options' => array('all' => 'Delete all TimeZynk nodes (tzjob and tzreport)'),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Perform operation'),
  );

  return $form;
}

function tzbase_admin_content_submit($form, $form_state) {
  $delete_all = array_filter($form_state['values']['delete_all']);
  if(in_array('all', $delete_all)) {
    // Find all nodes
    $result = db_query("SELECT nid FROM {node} WHERE type = 'tzreport' OR type = 'tzjob'");
    while($data = db_fetch_object($result)) {
      node_delete($data->nid);
    }
  }
}
