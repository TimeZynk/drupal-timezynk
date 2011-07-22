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
  
  /**
   * @param String $action Action URL
   * @param Array $data Raw post data
   * @return Raw HTML string response
   * @throws TZIntellitimeServerException
   */ 
  public function post($action, $data);
  
  /**
   * @param String $action Action URL
   * @return Raw HTML string response
   * @throws TZIntellitimeServerException
   */
  public function get($action);
}