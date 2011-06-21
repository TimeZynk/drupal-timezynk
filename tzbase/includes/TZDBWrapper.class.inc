<?php

interface TZDBWrapper {
  /**
   * Run query
   *
   * @param String $query
   * @param args... query arguments
   * @return Query cursor
   */
  public function query($query);

  /**
   * Fetch next object from cursor
   *
   * @param $cursor DB cursor from query()
   * @return Next object or NULL if no objects left
   */
  public function fetchObject($cursor);

  /**
   * Save a record to the database. The table needs to have an ID field named 'id'.
   *
   * @param String $table
   * @param Object $record
   * @return the written record.
   * @throws TZDBWrapperFailedToWriteException on failure to write to DB.
   */
  public function writeRecord($table, $record);

  /**
   * Delete a record from the database.
   *
   * @param String $table
   * @param int $record_id Record ID
   */
  public function delete($table, $record_id);
}