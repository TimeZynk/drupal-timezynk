<?php

class TZIntellitimeReportRowNotFound extends Exception {}
class TZIntellitimeInconsistentPost extends Exception {}

class TZIntellitimeParser {
  /**
   * @var SimpleXMLElement document to parse
   */
  private $doc = NULL;

  /**
   * Create a new TZIntellitimeParser for the given HTML string
   * @param string $html_string Raw HTML string in UTF-8 encoding
   */
  public function __construct($html_string) {
    $doc = new DOMDocument();
    /* The time report document contains some horrible misencodings
     * if adding absence rows containing '<' or '>'. They are printed
     * raw into the HTML and makes the parser cut the rest of the line.
     * Try to replace them with HTML entities.
     */
    $html_string = preg_replace_callback('/\s([<>])(\d+)/', 'TZIntellitimeParser::replace_lt_gt_with_entities', $html_string);
    /* Supress any warnings from malformed HTML soup.
     * The intellitime docs are declared as ISO-8859-1, but are in fact UTF-8.
     * In order to get rid of the horrible misencodings, we force UTF-8.
     */
    $html_string = mb_convert_encoding($html_string, 'HTML-ENTITIES', 'UTF-8');
    @$doc->loadHTML($html_string);
    $this->doc = simplexml_import_dom($doc);
    if(!$this->doc) {
      throw new TZNetworkFailureException('Unable to parse document');
    }
  }

  /**
   * Replace '<' with '&lt;' and '>' with '&gt;'
   * @param array $matches matches from preg_replace_callback
   */
  public static function replace_lt_gt_with_entities($matches) {
    return ' ' . ($matches[1] == '<' ? '&lt;' : '&gt;') . $matches[2];
  }

  /**
   * Just checks that if we have managed to login or not.
   * @return bool TRUE if successful, else FALSE.
   */
  public function is_valid_login() {
    /* Look for the LogOut link as proof that the login succeeded */
    if (!$this->doc) {
      return FALSE;
    }

    $list = $this->doc->xpath('//img[@src="../Images/Img_LogOut.gif"]');
    if (count($list) === 0) {
      return FALSE;
    }
    return TRUE;
  }

  /**
   * Returns the value of the "action" attribute in the login form.
   * @return the value of the "action" attribute in the login form if successful, else NULL.
   */
  public function parse_form_action() {
    // Extract form action
    $actionlist = $this->doc->xpath("//form/@action");
    if(count($actionlist) === 1) {
      return (string)$actionlist[0]['action'];
    }
  }

  /**
   * Parse assignment titles and codes from the given document
   * @return array TZIntellitimeAssignment array
   */
  public function parse_assignments() {
    $result = array();
    $elems = $this->doc->xpath('//select[@id="CustOrdersDropDown"]/option');
    if(!empty($elems)) {
      foreach($elems as $option) {
        $jobcode = trim((string)$option['value']);
        $jobtitle = trim((string)$option);
        if($jobcode && $jobtitle) {
          $assignment = new TZIntellitimeAssignment();
          $assignment->id = $jobcode;
          $assignment->report_key = $jobtitle;
          $assignment->title = $jobtitle;
          $result[] = $assignment;
        }
      }
    }
    return $result;
  }

  /**
   * Parse absence types and titles from the current document.
   * @return array TZIntellitimeAssignment array
   */
  public function parse_absence_types() {
    $result = array();
    $elems = $this->doc->xpath('//select[@id="AddRowDropDown"]/option');
    if(!empty($elems)) {
      foreach($elems as $option) {
        $jobcode = trim((string)$option['value']);
        $jobtitle = trim((string)$option);
        $matches = array();
        if($jobcode && $jobtitle && preg_match('/^_AC_(.+)/', $jobcode, $matches)) {
          $assignment = new TZIntellitimeAssignment();
          $assignment->id = $jobcode;
          $assignment->report_key = $matches[1];
          $assignment->title = $jobtitle;
          $assignment->type = TZIntellitimeAssignment::TYPE_ABSENCE;
          $result[] = $assignment;
        }
      }
    }
    return $result;
  }

  /**
   * Parse list over which weeks that have unfinished reports.
   * @return Array DateTime array with one of the days in each of the weeks that have unfinished reports
   */
  public function parse_unfinished_weeks() {
    $unfinishedWeeks = array();
    $elems = $this->doc->xpath('//a[@class="WeeksNav"]');
    foreach($elems as $a) {
      $unfinishedWeeks[] = $this->make_date_from_link($a['href']);
    }
    return $unfinishedWeeks;
  }

  /**
   * Gets all reports from the document.
   * @return Array List of all reports as an array
   */
  public function parse_reports() {
    /* Finds the year of the next week and prev week links.
     * If both years match, all reports belong to that year.
     * If not, we need to look at the month of the report,
     * which will tell us which of the years to choose.
     * This works since we look at one week at a time. */
    $adjacentWeeks = $this->next_and_previous_week_dates();

    $reports = array();
    $table_rows = $this->get_report_rows();
    if(!empty($table_rows)) {
      foreach($table_rows as $row) {
        $report = $this->parse_report_row($row);
        $report->year = $this->calculate_report_year($report, $adjacentWeeks);
        $report->break_duration_minutes = $this->calculate_report_breakduration($report);
        $reports[] = $report;
      }
    }
    return $reports;
  }

  /**
   * Parses the login form and constructs an array containing all that is
   * needed to submit a login post to intellitime.
   * @param string $username the username as a string
   * @param string $password the password as a string
   * @return Array All that is needed by TZIntellitimeUtility::build_action_url and $this->post
   */
  public function build_login_post($username, $password) {
    $result['action'] = $this->parse_form_action();
    $result['data'] = $this->build_raw_post();
    $result['data']['TextBoxUserName'] = $username;
    $result['data']['TextBoxPassword'] = $password;
    return $result;
  }

  public function build_update_reports_post(array $itreports, $allReportsDone) {
    $post = $this->build_raw_post();

    foreach($itreports as $itreport) {
      $this->set_report_fields($post, $itreport);
      $this->set_report_state($post, $itreport);
    }

    if($allReportsDone) {
      if(empty($post['DoneButton'])) {
        throw new TZIntellitimeInconsistentPost('Done button not found: ' . print_r($post, TRUE));
      }
      unset($post['UpdateButton']);
      unset($post['ChangeButton']);
    } else {
      if(empty($post['UpdateButton'])) {
        throw new TZIntellitimeInconsistentPost('Update button not found');
      }
      unset($post['DoneButton']);
      unset($post['ChangeButton']);
    }

    return $post;
  }

  /**
   * Build a post that unlocks the listed reports
   * @param array $itreports
   */
  public function build_unlock_post(array $itreports, $unlockImmutable = FALSE) {
    $post = $this->build_raw_post();

    if(isset($post['ChangeButton'])) {
      /* If we find the ChangeButton then all reports are marked as
       * Reported or higher. Unmark all that can be unmarked so that the
       * next POST can change the appropriate values */
      unset($post['DoneButton']);
      unset($post['UpdateButton']);
    } else if($unlockImmutable) {
      /* If we have immutable reports on a partially locked week we have to
       * first lock the week to then be able to unlock it.
       */
      unset($post['ChangeButton']);
      unset($post['UpdateButton']);
    } else {
      // Report states are possible to manipulate, set up the state checkboxes.
      foreach($itreports as $itreport) {
        $itreport_clone = clone($itreport);
        $itreport_clone->state = TZIntellitimeReport::STATE_OPEN;
        $this->set_report_state($post, $itreport_clone);
      }
      unset($post['DoneButton']);
    }

    return $post;
  }

  /**
   * @return array Unmodified POST data from DOM
   */
  private function build_raw_post() {
    $post = array();
    $post = array_merge($post, $this->build_selects());
    $post = array_merge($post, $this->build_inputs());
    return $post;
  }

  /**
   * Process all form inputs and build a POST:able array containing the new data
   * from $report
   *
   * @return Array HTTP POST fields
   */
  private function build_inputs() {
    $inputs = $this->doc->xpath("//input");
    foreach ($inputs as $input) {
      if (!$input['disabled']) {
        if ($input['type'] != "checkbox") {
          $result[(string)$input['name']] = (string)$input['value'];
        } else if ($input['checked']) {
          $result[(string) $input['name']] = "on";
        }
      }
    }
    return $result;
  }

  /**
   * Find and return the control prefix for a report. Each report uses
   * a naming scheme for the related form elements that look like
   * OldRowsRepeater:_ctlX:<form element name>.
   *
   * @return the OldRowsRepeater:_ctlX: prefix for the matching row.
   * @throws TZIntellitimeReportRowNotFound if report is not found in the DOM
   */
  private function find_report_control_prefix($report) {
    // Check if $report is found among the input rows
    $row = $this->find_row_by_id($report->id);
    if(empty($row)) {
      throw new TZIntellitimeReportRowNotFound("report with id '$report->id' not found");
    }

    return $this->get_row_control_prefix($row);
  }

  private function get_row_control_prefix($row) {
    $first_input = $row->xpath('*/input[1]');
    $name_segment = explode(':', (string) $first_input[0]['name']);
    array_pop($name_segment);
    $control_prefix = implode(':', $name_segment) . ':';

    return $control_prefix;
  }

  private function set_report_state(&$post, $report) {
    $control_prefix = $this->find_report_control_prefix($report);

    switch ($report->state) {
      case TZIntellitimeReport::STATE_OPEN:
        unset($post[$control_prefix . "CheckboxDayDone"]);
        unset($post[$control_prefix . "CheckboxDelete"]);
        break;

      case TZIntellitimeReport::STATE_REPORTED:
        $control_name = $control_prefix . "CheckboxDayDone";
        if ($this->name_exists($control_name)) {
          $post[$control_name] = "on";
        }
        unset($post[$control_prefix . "CheckboxDelete"]);
        break;

      case TZIntellitimeReport::STATE_DELETED:
        $post[$control_prefix . "CheckboxDelete"] = "on";
        break;

      default:
        break;
    }
  }

  private function name_exists($name) {
    return 0 < count($this->doc->xpath('//*[@name="' . $name . '"]'));
  }

  /**
   * @param TZIntellitimeReport $report
   */
  private function set_report_fields(&$post, $report) {
    $control_prefix = $this->find_report_control_prefix($report);

    if (isset($post[$control_prefix . "TextboxTimeFrom"])) {
      $post[$control_prefix . "TextboxTimeFrom"] = $report->begin;
    }

    if (isset($post[$control_prefix . "TextboxTimeTo"])) {
      $post[$control_prefix . "TextboxTimeTo"] = $report->end;
    }

    if (isset($post[$control_prefix . "TextboxBreak"])) {
      $post[$control_prefix . "TextboxBreak"] = $report->break_duration_minutes;
    }

    if (isset($post[$control_prefix . "TextboxExplicitOvertime"])) {
      $post[$control_prefix . "TextboxExplicitOvertime"] = $report->overtime_hours;
    }

    if (isset($post[$control_prefix . "TextboxNote"])) {
      $post[$control_prefix . "TextboxNote"] = $report->comment;
    }
  }

  /**
   * Takes all select fields in the document and builds postable arrays of the selected options.
   * @return Array arrays of the form a[select tag name] = option value
   */
  private function build_selects() {
    $result = array();
    $selects = $this->doc->xpath("//select");
    foreach ($selects as $select) {
      if($select['disabled'] || count($select->option) === 0) {
        // Disabled or empty selects are excluded from the POST
        continue;
      }

      $name = (string)$select['name'];

      /* Check if the first option has a value and use that as default
       * According to W3C we should instead fallback to the contained
       * text, but that makes us send the "Expense" selects, which seems
       * to be wrong. Not sure what the correct way to handle it is.
       */
      if(isset($select->option[0]['value'])) {
        $result[$name] = (string)$select->option[0]['value'];
      }

      // Check if another option is selected and overrides the default
      foreach ($select->option as $option) {
        if ($option['selected'] && isset($option['value'])) {
          $value = (string) $option['value'];
          $result[$name] = $value;
        }
      }
    }
    return $result;
  }



  /**
   * Parses a HTML table row containing a time report and populates a TZIntellitimeReport.
   * @param SimpleXMLElement $row a SimpleXML element containing a table row plus children, containing report data.
   * @return the populated report or NULL if parsing failed.
   */
  private function parse_report_row($row) {
    // Find this rows unique id..
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
    if (!empty($td->input)) {
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
      return TZIntellitimeReport::STATE_REPORTED;
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
        $dates[] = $this->make_date_from_link($href);
      }
    }
    return $dates;
  }

  private function make_date_from_link($link) {
    $timezone = date_default_timezone(FALSE);
    if(preg_match('/DateInWeek=(\d{4}-\d{2}-\d{2})/', $link, $match)) {
        return date_make_date($match[1], $timezone, DATE_DATETIME, array('year', 'month', 'day'));
    }
    return NULL;
  }

  private function calculate_report_year($report, $adjacentWeeks) {
    if ($report->month === NULL) {
      return FALSE;
    }
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
    if(empty($break_duration_minutes)) {
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
}