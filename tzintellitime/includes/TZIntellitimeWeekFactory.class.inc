<?php

interface TZIntellitimeWeekFactory {
  /**
   * @param DateTime $datetime the week in question.
   * @param array $tzReports an array of tzreport arrays.
   * @return TZIntellitimeWeek
   */
  public function createWeek($datetime, $tzReports);
}