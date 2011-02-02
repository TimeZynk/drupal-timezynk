<?php
/**
 * @file
 * Handler for printing time durations
 */

/**
 * Views handler for interval types
 */
class tzbase_handler_field_tzjob_flags extends views_handler_field {
  function render($values) {
    tzbase_include_proto_classes();
    $flags = $values->{$this->field_alias};
    if($flags == TZJobFlags::ACTIVE) {
      return t('Active');
    }
    return t('Inactive');
  }
}
