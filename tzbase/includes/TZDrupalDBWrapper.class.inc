<?php

class TZDrupalDBWrapper implements TZDBWrapper {

  public function query($query) {
    $args = func_get_args();
    array_shift($args);
    return db_query($query, $args);
  }

  public function fetchObject($cursor) {
    return db_fetch_object($cursor);
  }

  public function writeRecord($table, $record) {
    $record = (object) $record;
    module_invoke_all('db_write_record_' . $table, $record);
    if (empty($record->id)) {
      drupal_write_record($table, $record);
    } else {
      drupal_write_record($table, $record, array('id'));
    }
    return $record;
  }

  public function delete($table, $record_id) {
    if (!empty($table) && !empty($record_id)) {
      db_query('DELETE FROM {%s} WHERE id = %d', $table, $record_id);
    }
  }
}
