<?php
/**
 * @file
 * Handler for printing time durations
 */

/**
 * Views handler for interval types
 */
class tzbase_handler_field_tzreport_flags extends views_handler_field {
  function render($values) {
    $duration = $values->{$this->field_alias};
    return tzbase_format_tzreport_flags($duration);
  }
}
