<?php

class IntellitimeWeekPageParser {
  /**
   * @var SimpleXMLElement
   */
  private $doc;

  function __construct($doc) {
    $this->doc = $doc;
  }

  public function name_exists($name) {
    return 0 < count($this->doc->xpath('//*[@name="' . $name . '"]'));
  }

  /**
   * Find and return the control prefix for a report. Each report uses
   * a naming scheme for the related form elements that look like
   * OldRowsRepeater:_ctlX:<form element name>.
   *
   * @return the OldRowsRepeater:_ctlX: prefix for the matching row.
   * @throws TZIntellitimeReportRowNotFound if report is not found in the DOM
   */
  public function find_report_control_prefix($report) {
    // Check if $report is found among the input rows
    $row = $this->find_row_by_id($report->id);
    if(!$row) {
      throw new TZIntellitimeReportRowNotFound("report with id '$report->id' not found");
    }

    return $this->get_row_control_prefix($row);
  }

  public function parse_assignment_dropdown(&$result, $id, $may_create_report) {
    $elems = $this->doc->xpath('//select[@id="' . $id . '"]/option');
    if(!empty($elems)) {
      foreach($elems as $option) {
        $job_id = trim((string)$option['value']);
        $short_job_title = trim((string)$option);
        if (!empty($result[$job_id])) {
          continue;
        }
        $assignment = $this->parse_assignment($job_id, $short_job_title);
        if (!$assignment) {
          continue;
        }
        $assignment->setMayCreateReport($may_create_report);
        $result[$job_id] = $assignment;
      }
    }
  }

  public function parse_reports() {
    /* Finds the year of the next week and prev week links.
     * If both years match, all reports belong to that year.
     * If not, we need to look at the month of the report,
     * which will tell us which of the years to choose.
     * This works since we look at one week at a time. */
    $adjacentWeeks = $this->next_and_previous_week_dates();

    $reports = array();
    $table_rows = $this->get_report_rows();
    if(empty($table_rows)) {
      return $reports;
    }

    foreach($table_rows as $row) {
      $report = $this->parse_report_row($row);
      $report->year = $this->calculate_report_year($report, $adjacentWeeks);
      $report->break_duration_minutes = $this->calculate_report_breakduration($report);
      $reports[] = $report;
    }

    return $reports;
  }

  public function parse_unfinished_weeks() {
    $unfinishedWeeks = array();
    $elems = $this->doc->xpath('//a[@class="WeeksNav"]');
    if(empty($elems)) {
      return $unfinishedWeeks;
    }

    foreach($elems as $a) {
      $unfinishedWeeks[] = IntellitimeWeekPageParser::make_date_from_link($a['href']);
    }
    return $unfinishedWeeks;
  }

  public static function make_date_from_link($link) {
    $timezone = date_default_timezone(FALSE);
    if(preg_match('/DateInWeek=(\d{4}-\d{2}-\d{2})/', $link, $match)) {
        return date_make_date($match[1], $timezone, DATE_DATETIME, array('year', 'month', 'day'));
    }
    return NULL;
  }

  private function parse_assignment($job_id, $short_job_title) {
      if(!$job_id || $job_id < 0 || !$short_job_title) {
        return NULL;
      }

      $matches = array();
      if (preg_match('/^_AC_(.+)/', $job_id, $matches)) {
        return new TZIntellitimeAssignment($short_job_title, $matches[1], $job_id, TZIntellitimeAssignment::TYPE_ABSENCE);
      } else {
        return new TZIntellitimeAssignment($short_job_title, NULL, $job_id);
      }
  }

  private function get_row_control_prefix($row) {
    $first_input = $row->xpath('*/input[1]');
    $name_segment = explode(':', (string) $first_input[0]['name']);
    array_pop($name_segment);
    $control_prefix = implode(':', $name_segment) . ':';

    return $control_prefix;
  }

  private function get_report_rows() {
    return $this->doc->xpath('//select[@id="SelectExpenses"]/../..');
  }

  /**
   * Finds a report table row based on the unique report ID.
   * @param string $id the unique ID parsed out by ::parse_report_row()
   */
  private function find_row_by_id($id) {
    $table_rows = $this->get_report_rows();
    foreach($table_rows as $row) {
      if($this->get_row_id($row) == $id) {
        return $row;
      }
    }
    return FALSE;
  }

  private function get_row_id($row) {
    $onclick_handlers = $row->xpath('td/select/@onclick');
    return preg_replace("/^.*Expense.aspx\?(.*)\'.*$/", "$1", (string)$onclick_handlers[0]['onclick']);
  }

  /**
   * Parses a HTML table row containing a time report and populates a TZIntellitimeReport.
   * @param SimpleXMLElement $row a SimpleXML element containing a table row plus children, containing report data.
   * @return the populated report or NULL if parsing failed.
   */
  private function parse_report_row($row) {
    $report = new TZIntellitimeReport();

    $report->id = $this->get_row_id($row);
    $report->state = $this->parse_row_state($row);
    $report->stateImmutable = $this->row_has_state_checkbox($row);
    list($report->month, $report->day) = $this->get_row_month_day($row);
    $report->title = $this->get_row_title($row);

    // indices 7, 9, 11 => start time, end time, break duration
    $report->begin = strval($row->td[7]->input[0]['value']);
    $report->end = strval($row->td[9]->input[0]['value']);
    $report->break_duration_minutes = intval($row->td[11]->input[0]['value'],10);

    $report->total_hours = $this->get_row_total_duration($row);
    $report->overtime_hours = $this->get_row_overtime($row);
    $report->comment = $this->get_row_comment($row);

    return $report;
  }

  private function get_row_total_duration($row) {
    $floatString = strval($row->td[13]->span);
    if (empty($floatString)) {
      return NULL;
    }
    $floatDotDecimal = preg_replace('/,/', '.', $floatString);
    return floatval($floatDotDecimal);
  }

  private function get_row_overtime($row) {
    $overtimeString = strval($row->td[15]->input[0]['value']);
    $overtimeDotDecimal = preg_replace('/,/', '.', $overtimeString);
    return floatval($overtimeDotDecimal);
  }

  private function get_row_comment($row) {
    $control_prefix = $this->get_row_control_prefix($row);
    $commentelement = $row->xpath('..//input[@name=\'' . $control_prefix . 'TextboxNote\']');
    $comment = '';
    if (!empty($commentelement[0]['value'])) {
      $comment = trim((string) $commentelement[0]['value']);
    }
    return $comment;
  }

  private function get_row_title($row) {
    return trim((string)$row->td[5]->span);
  }

  private function get_row_month_day($row) {
    $date_matches = array();
    $monthDay = array();
    if (!empty($row->td[3]) && preg_match('/(\d{2}).(\d{2})/', strval($row->td[3]->span), $date_matches)) {
      $monthDay[0] = intval($date_matches[2], 10);
      $monthDay[1] = intval($date_matches[1], 10);
    };
    return $monthDay;
  }

  private function row_has_state_checkbox($row) {
    return (0 === count($row->td[1]->input));
  }

  private function parse_row_state($row) {
    /* Index 1 has either a "done" checkbox, the "locked" icon, nothing at all,
     * or a 'check mark' icon. */
    $td = $row->td[1];
    if ($td->input) {
      if($td->input['checked']) {
        return TZIntellitimeReport::STATE_REPORTED;
      }
      return TZIntellitimeReport::STATE_OPEN;
    }

    if(count($td->img) > 0) {
      foreach($td->img as $img) {
        if(FALSE !== strpos($img['src'], 'Img_Lock')) {
          return TZIntellitimeReport::STATE_LOCKED;
        } elseif(FALSE !== strpos($img['src'], 'Img_GreenCheck')) {
          return TZIntellitimeReport::STATE_REPORTED;
        }
      }
    }

    /* All other state detection strategies failed? Check if expenses selection
     * contains any options. In IT v9 this indicates that the report row is done.
     */
    $expensesFieldWithOptions = $row->xpath('td/select/option');
    if (empty($expensesFieldWithOptions)) {
      /* We only want to mark the report as reported if we are in an editable week */
      $submitButtons = $this->doc->xpath('//input[@type="submit"]');
      if (!empty($submitButtons)) {
        return TZIntellitimeReport::STATE_REPORTED;
      }
    }

    // No indicators found, assume STATE_OPEN
    return TZIntellitimeReport::STATE_OPEN;
  }

  private function next_and_previous_week_dates() {
    // Find next and previous week links
    $dates = array();
    $hrefs = $this->doc->xpath('//a/img[@src="../Images/Btn_ArrowLeft.gif" or @src="../Images/Btn_ArrowRight.gif"]/../@href');
    if(!empty($hrefs)) {
      foreach ($hrefs as $href) {
        $dates[] = IntellitimeWeekPageParser::make_date_from_link($href);
      }
    }
    return $dates;
  }

  private function calculate_report_year($report, $adjacentWeeks) {
    $years = array(
      intval($adjacentWeeks[0]->format('Y'), 10),
      intval($adjacentWeeks[1]->format('Y'), 10),
    );
    sort($years);
    $year = $years[0];

    if($years[0] != $years[1] && $report->month == 1) {
      $year = $years[1];
    }

    return $year;
  }

  private function calculate_report_breakduration($report) {
    $break_duration_minutes = $report->break_duration_minutes;

    /* On days with absence intellitime chooses not to display the break
     * duration as its own input field. Instead in hides the field completely
     * and its value can only be computed by comparing the actual times with
     * the displayed total duration */
    if(empty($break_duration_minutes) && $report->total_hours !== NULL) {
      $begindate = $report->get_begintime();
      $enddate = $report->get_endtime();
      if($enddate < $begindate) {
        $enddate->modify('+1 day');
      }

      $duration_seconds = $enddate->format('U') - $begindate->format('U');
      $specified_duration_seconds = $report->total_hours * 3600;
      $diff_seconds = $duration_seconds - $specified_duration_seconds;

      if($diff_seconds > 72) {
        /* more than 0,02 hours difference between Intellitimes specified
         * duration and our calculated duration should mean that there is
         * a hidden break */
        $break_duration_minutes = floor($diff_seconds / 60);
      }
    }

    return $break_duration_minutes;
  }
}