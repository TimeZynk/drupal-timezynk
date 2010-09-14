<?php
/**
 * @file
 * Handler for printing time durations
 */

/**
 * Views handler for interval types
 */
class tzbase_handler_field_interval extends views_handler_field {
  function render($values) {
    $duration = $values->{$this->field_alias};
    return tzbase_views_format_duration($duration);
  }
}

/**
 * Views handler for showing the total duration
 */
class tzbase_handler_field_total_duration extends tzbase_handler_field_interval {
  /**
   * Called to add the field to a query.
   */
  function query() {
    $this->ensure_my_table();
    
    // Add fields we depend on
    $this->add_additional_fields(array('begintime', 'endtime', 'breakduration'));
    
    // Create the computed field
    $this->field_alias = $this->query->add_field(NULL, 
    	"($this->table_alias.endtime - $this->table_alias.begintime - $this->table_alias.breakduration)",
        'totalduration');
  }
  
  function render($values) {
    return tzbase_views_format_duration($values->{$this->field_alias});
  }
}
