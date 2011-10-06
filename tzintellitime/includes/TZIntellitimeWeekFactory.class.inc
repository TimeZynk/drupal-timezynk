<?php

class TZIntellitimeWeekFactory {

  /**
   * @var TZIntellitimeWeekDataFactory
   */
  private $dataFactory;

  /**
   * @var Object Drupal Account information
   */
  private $account;

  /**
   * Create a new Week Factory
   *
   * @param TZIntellitimeWeekDataFactory $dataFactory
   * @param Object $account
   */
  public function __construct($dataFactory, $account) {
    $this->dataFactory = $dataFactory;
    $this->account = $account;
  }

  /**
   * @param DateTime $datetime
   * @param Array $tzReports
   * @return TZIntellitimeWeek
   */
  public function createWeek($datetime, $tzReports) {
    return new TZIntellitimeWeek($datetime, $this->dataFactory, $tzReports, $this->account);
  }
}

