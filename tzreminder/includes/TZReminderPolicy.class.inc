<?php

interface TZReminderPolicy {
 
  /**
   * Returns a preconfigured query primed to return results relevant 
   * for the specified time
   * @param DateTime $time the date and time to run the query for
   * @return TZQuery | NULL
   */
  public function getQuery($time);

  /**
   * Returns a translated string containing a suitable reminder message for the
   * policy.
   *
   * @param integer $numberOfReports
   * @return string the translated message.
   */
  public function getMessage($numberOfReports);

  /**
   * Determines if a policy ought to send a message, based on what the time is
   * when the policy is called, when it was last successfully called and any
   * further constraints of the policy
   * @param DateTime $timeNow when the policy is being called.
   * @param DateTime $timeLastCalled when the policy was last successfully called.
   * @return boolean
   */
  public function shouldSendMessage($timeNow, $timeLastCalled);
}