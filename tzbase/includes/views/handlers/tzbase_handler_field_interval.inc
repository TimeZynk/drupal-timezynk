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
    return tzbase_format_duration($duration);
  }
}
