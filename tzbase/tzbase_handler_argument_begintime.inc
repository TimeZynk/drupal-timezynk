<?php
/**
 * @file
 * Argument handler for begintime filtering
 * @author johan
 */
class tzbase_handler_argument_begintime extends views_handler_argument {
  function option_definition() {
    $options = parent::option_definition();

    $options['operator'] = array('default' => 0);
 
    return $options;
  }
  
  function options_form(&$form, &$form_state) {
     parent::options_form($form, $form_state);
 
     // allow <= or >=
     $form['operator'] = array(
       '#type' => 'radios',
       '#title' => t('Select comparison'),
       '#description' => t('Controls whether to filter for reports filed before or after the argument date'),
       '#options' => array(
         t('Get reports that start after the argument'),
         t('Get reports that start before the argument')
       ),
       '#default_value' => empty($this->options['operator']) ? 0 : $this->options['operator'],
     );
  }
  
  
  function query() {
    $this->ensure_my_table();
    $operator = '>=';
    if($this->options['operator'] == 1) {
      $operator = '<=';
    }
    $this->query->add_where(0, "$this->table_alias.$this->real_field $operator %d", $this->argument);
  }
}