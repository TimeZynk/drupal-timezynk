<?php

class TZIntellitimeWeekDataFactory {
  /**
   * @var IntellitimeServer
   */
  private $server;

  /**
   * @param IntellitimeServer $server
   */
  public function __construct($server) {
    $this->server = $server;
  }

  /**
   * @param DateTime $date
   * @return TZIntellitimeWeekData
   * @throws TZIntellitimeServerException
   */
  public function createWeekData($date = NULL) {
    // Use today as default date
    if($date === NULL) {
      $date = tzbase_make_date();
    }

    $datestr = $date->format('Y-m-d');
    $dateaction = "TimeReport/TimeReport.aspx?DateInWeek=$datestr";

    $content = $this->server->get($dateaction);
    return new TZIntellitimeWeekData(new IntellitimeWeekPage($content, $this->server));
  }

  public function createWeekDataFromPost($post) {
    if(empty($post)) {
      throw new InvalidArgumentException('Empty post object in TZIntellitimeWeekDataFactory::createWeekDataFromPost()');
    }
    return new TZIntellitimeWeekData($post->post());
  }
}