<?php

/**
 * Implementation of hook_ctools_plugin_api().
 */
function timezynk_basic_feature_ctools_plugin_api() {
  list($module, $api) = func_get_args();
  if ($module == "strongarm" && $api == "strongarm") {
    return array("version" => 1);
  }
}

/**
 * Implementation of hook_node_info().
 */
function timezynk_basic_feature_node_info() {
  $items = array(
    'profile' => array(
      'name' => t('Profil'),
      'module' => 'features',
      'description' => t('A user profile built as content.'),
      'has_title' => '1',
      'title_label' => t('Titel'),
      'has_body' => '0',
      'body_label' => '',
      'min_word_count' => '0',
      'help' => '',
    ),
  );
  return $items;
}
