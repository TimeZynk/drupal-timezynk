<?php

interface TZIntellitimeSyncPolicy {
  /**
   * @return DateTime pointing out a week.
   */
  public function getNextWeekToSync();
  /**
   * @param array DateTime
   */
  public function addWeeks($weeks);
}