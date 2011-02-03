<?php

interface TZIntellitimeReportStorage {
  /**
   * @param DateTime $from
   * @param DateTime $to
   * @return array TZReport
   */
  public function getTZReports($from, $to);

  /**
   * @param array $assignments TZJob array
   * @return array TZJob
   */
  public function storeTZJobs($tzjobs);

  /**
   * @param array $tzreports TZReport array
   * @return array TZReport
   */
  public function storeTZReports($tzreports);
}