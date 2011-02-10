<?php


class TZTimeOfDaySpan {
  private $start;
  private $end;
  /**
   * Create a new quiet period checker
   * @param string $start Start of period as HH:MM
   * @param string $end Start of period as HH:MM
   */
  function __construct($start, $end) {
    $this->start = $this->parseTime($start);
    $this->end = $this->parseTime($end);
  }

  function isInsideSpan($datetime) {
    $startDate = clone($datetime);
    $startDate->setTime($this->start['hours'], $this->start['minutes'], 0);

    $endDate = clone($datetime);
    $endDate->setTime($this->end['hours'], $this->end['minutes'], 0);
    if($endDate < $startDate) {
      $endDate->modify('+1 day');
    }

    if($datetime < $startDate) {
      $startDate->modify('-1 day');
      $endDate->modify('-1 day');
    }

    if ($datetime < $startDate || $datetime > $endDate) {
      return FALSE;
    }

    return TRUE;
  }

  private function parseTime($time) {
    $result = array();
    if(preg_match('/^(\d{2}):(\d{2})$/', $time, $match)) {
      $result['hours'] = intval($match[1], 10);
      $result['minutes'] = intval($match[2], 10);
    } else {
      throw new InvalidArgumentException('Invalid time format');
    }
    return $result;
  }
}
