<?php
/**
 * @file
 * Handler for printing time durations
 */

/**
 * Views handler for interval types
 */
class jadmaker_handler_field_http extends views_handler_field {
  function render($values) {
    $data = $values->{$this->field_alias};
    $variables = unserialize(gzuncompress($data));
    $output = '<pre>';
    foreach($variables as $key => $value) {
      $output .= $key . ': ' . $value . "\n";
    }
    $output .= '</pre>';
    return $output;
  }
}
