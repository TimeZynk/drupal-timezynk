<?php

tzbase_include_proto_classes();

class TZFixedDateReminderPolicy extends TZAbstractReminderPolicy {

/**
 * The day on which the reports are due.
 * Is evaluated relative to the current month (as given in getQuery).
 * Reports are due if r.endtime < "current_year-current_month-($dueDate+1day)T00:00".
 * If duedate is greater than the number of days in the month, it will truncate.
 * @var int
 */
  private $dueDate;

  /**
   * The hour on which to first attempt to send the reminders..
   * @var int
   */
  private $reminderHour;

  /**
   * Sets the due date and the time to send out the reminder.
   * Any reports due before the end of this day will be eligible for a reminder.
   * The reminder will be sent at this date, at the time specified in $dueDate.
   * @param int $dueDay the day in month on which to check for due reports (1-28)
   * @param int $reminderHour the hour of the day on which to send the reminders (00-23)
   */
  public function __construct($dueDate, $reminderHour) {
    if (empty($dueDate)) {
      throw new InvalidArgumentException("Must set due day");
    }
    if ($dueDate < 1 || $dueDate > 28) {
      throw new InvalidArgumentException("Illegal due day, must be in range 1-28");
    }
    if (!isset($reminderHour)) {
      throw new InvalidArgumentException("Must set reminder hour");
    }
    if ($reminderHour < 0 || $reminderHour > 23) {
      throw new InvalidArgumentException("Illegal reminder hour, must be in range 0-23");
    }
    $this->dueDate = $dueDate;
    $this->reminderHour = $reminderHour;

  }

  public function getQuery($time) {
    if ($time->format('d') != $this->dueDate) {
      return NULL;
    } else if ($time->format('G') < $this->reminderHour) {
      return NULL;
    }
    $queryBuilder = new TZReportQueryBuilder(TZFlags::REPORTED);
    $endtime = clone($time);
    $endtime->setDate($time->format('Y'), $time->format('m'), $this->dueDate+1);
    $endtime->setTime(0, 0, 0);
    $queryBuilder->setEndtimeBefore($endtime);
    return $queryBuilder->build();

  }

  public function shouldSendMessage($timeNow, $timeLastCalled) {
    if ($timeLastCalled > $timeNow) {
      throw new InvalidArgumentException("Seemingly timeNow is less than timeLastCalled. Is time running backwards where you live?");
    }
    // Not the correct day of month
    if ($timeNow->format('d') != $this->dueDate) {
      return FALSE;
    }
    // Too early on the correct day.
    if ($timeNow->format('G') < $this->reminderHour) {
      return FALSE;
    }
    // Has successfully been called today already.
    if ($timeNow->format('Y-m-d') === $timeLastCalled->format('Y-m-d')) {
      return FALSE;
    }
    return TRUE;
  }

  public function getName() {
    return "fixed";
  }

}