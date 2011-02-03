<?php

class TZIntellitimeDefaultWeekFactory implements TZIntellitimeWeekFactory {

  /**
   * @var TZIntellitimeServerInterface
   */
  private $serverInterface;

  /**
   * @var Object Drupal Account information
   */
  private $account;

  /**
   * Create a new Week Factory
   *
   * @param TZIntellitimeServerInterface $serverInterface
   * @param Object $account
   */
  public function __construct($serverInterface, $account) {
    if(empty($serverInterface) || empty($account)) {
      throw new InvalidArgumentException('Empty arguments passed to TZIntellitimeDefaultWeekFactory constructor');
    }
    $this->serverInterface = $serverInterface;
    $this->account = $account;
  }

  /**
   * @param DateTime $datetime
   * @param Array $tzReports
   * @return TZIntellitimeWeek
   */
  public function createWeek($datetime, $tzReports) {
    return new TZIntellitimeWeek($datetime, $this->serverInterface, $tzReports, $this->account);
  }
}

