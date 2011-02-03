<?php

interface TZIntellitimeServerInterface {

  /**
   * @param DateTime $date
   * @return TZIntellitimeWeekData
   * @throws TZIntellitimeServerException
   */
  public function refreshWeek($date);

  /**
   * @param TZIntellitimePostData $postData
   * @return TZIntellitimeWeekData
   * @throws TZIntellitimeServerException
   */
  public function postWeek($postData);
}