<?php

class TZIntellitimeBot {

  /**
   * the cURL handle used for login.
   * @var a handle or NULL
   */
  private $handle = NULL;

  /**
   * The intellitime login URL, unique per customer.
   * @var string
   */
  private $login_url = NULL;

  /**
   * Creates a new TZIntellitimeBot
   * @param string $url the customer login URL for intellitime.
   * @param string $cookiejar the path to a temporary file used to store our login-cookies.
   */
  function __construct($url, $cookiejar) {
    $handle = curl_init();
    $this->handle = $handle;
    $this->login_url = $url;

    // Set standard curl options
    $curl_opt = array(
    CURLOPT_USERAGENT => "TimeZynk/1.0",
    CURLOPT_COOKIEFILE => $cookiejar,
    CURLOPT_COOKIEJAR => $cookiejar,
    );
    curl_setopt_array($this->handle, $curl_opt);
  }

  /**
   * Destroys us!
   * Evil androids hate robots!
   */
  function __destruct() {
    if($this->handle !== NULL) {
      curl_close($this->handle);
    }
  }

  /**
   * Logs in to the intellitime web service at $url
   * @param string $url the customer specific login URL.
   * @param string $username the user to login as
   * @param string $password the password to login with
   * @return bool TRUE if successful, otherwise FALSE.
   */
  public function login($username, $password) {
    $parser = $this->request($this->login_url);
    if (!$parser) {
      return FALSE;
    }
    $post = $parser->build_login_post($username, $password);
    $post_url = $this->build_action_url("Portal/" . $post['action']);
    $parser = $this->request($post_url, $post['data']);
    if ($parser && $parser->is_valid_login()) {
      return TRUE;
    }
    return FALSE;
  }

  public function load_assignments($date = 0) {
    // Use today as default date
    if($date === 0) {
      $date = time();
    }

    $datestr = date("Y-m-d", $date);
    $dateaction = "TimeReport/TimeReport.aspx?DateInWeek=$datestr";

    $url = $this->build_action_url($dateaction);
    $parser = $this->request($url);
    if(!$parser) {
      return NULL;
    }

    $result['assignments'] = $parser->parse_assignments();
    $result['reports'] = $parser->parse_reports();
    return $result;
  }

  /**
   * Logout from the Intellitime Website
   */
  public function logout() {
    $this->request($this->build_action_url('Portal/LogOut.aspx?MId=LogOut'));
  }

  /**
   * Lowlevel HTTP request. Issues a GET or POST request depending on the value of $post_data
   * @param string $url URL to request
   * @param array $post_data optional key => value array of POST parameters
   * @return TZIntellitimeParser Parser on success, otherwise FALSE
   */
  public function request($url, $post_data = NULL) {
    $curl_opts = array(
    CURLOPT_RETURNTRANSFER => TRUE,
    CURLOPT_BINARYTRANSFER => TRUE,
    CURLOPT_FAILONERROR => TRUE,
    CURLOPT_URL => $url,
    CURLOPT_FOLLOWLOCATION => TRUE,
    );

    if($post_data) {
      $curl_opts[CURLOPT_POSTFIELDS] = http_build_query($post_data, '', '&');
      $curl_opts[CURLOPT_POST] = TRUE;
    } else {
      $curl_opts[CURLOPT_POSTFIELDS] = NULL;
      $curl_opts[CURLOPT_POST] = FALSE;
    }
    curl_setopt_array($this->handle, $curl_opts);

    $output = curl_exec($this->handle);
    if($output) {
      return new TZIntellitimeParser($output);
    }
    return FALSE;
  }

  /**
   *  Strip away <Dirname>/<File>.aspx?<query> from current effective URL and append $action instead
   * @param string $url the URL as a string
   * @param string $action the action as a string
   * @return the newly constructed URL
   */
  private function build_action_url($action) {
    $effective_url = curl_getinfo($this->handle, CURLINFO_EFFECTIVE_URL);
    $base_url = preg_replace("/[^\/]+\/[^\/]+\.aspx.*$/", "", $effective_url);
    return $base_url . $action;
  }

  /**
   * Send an updated report to the intellitime web service.
   * @param TZIntellitimeReport $report a report containing the updated values.
   * @return bool TRUE if report was updated on server, else FALSE
   */
  public function update_report($report) {
    $datestr = $report->get_date_string();
    $dateaction = "TimeReport/TimeReport.aspx?DateInWeek=$datestr";

    // We need fresh info about the time reports for build_update_report_post.
    $url = $this->build_action_url($dateaction);
    $parser = $this->request($url);
    $reports = $parser->parse_reports();
    foreach($reports as $old_report) {
      if ($report->id === $old_report->id) {
        if ($report == $old_report) {
          // No need to do anything, the report has not changed.
          return TRUE;
        } else if ($old_report->done) {
          // First unlock the report
          $old_done_value = $report->done;
          $report->done = FALSE;
          $parser = $this->post_report($parser, $dateaction, $report);
          $report->done = $old_done_value;
        }
      }
    }
    // Send the post.
    $parser = $this->post_report($parser, $dateaction, $report);
    $reports = $parser->parse_reports();

    // Check that the report was updated
    foreach($reports as $updated_report) {
      if ($report == $updated_report) {
        return TRUE;
      }
    }
    return FALSE;
  }

  private function post_report($parser, $dateaction, $report) {
    $post_url = $this->build_action_url($dateaction);
    $post = $parser->build_update_report_post($report);
    return $this->request($post_url, $post['data']);
  }

}

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
    /* Supress any warnings from malformed HTML soup.
     * The intellitime docs are declared as ISO-8859-1, but are in fact UTF-8.
     * In order to get rid of the horrible misencodings, we force UTF-8.
     */
    @$doc->loadHTML('<?xml encoding="UTF-8">' . $html_string);
    $this->doc = simplexml_import_dom($doc);
  }

  /**
   * Just checks that if we have managed to login or not.
   * @return bool TRUE if successful, else FALSE.
   */
  public function is_valid_login() {
    if (!$this->doc) {
      return FALSE;
    }

    $list = $this->doc->xpath('//a[@href="LogOut.aspx?MId=LogOut"]');
    if (count($list) === 0) {
      return FALSE;
    }
    return TRUE;
  }

  /**
   * Parses the login form and constructs an array containing all that is
   * needed to submit a login post to intellitime.
   * @param string $username the username as a string
   * @param string $password the password as a string
   * @return Array All that is needed by TZIntellitimeUtility::build_action_url and $this->post
   */
  public function build_login_post($username, $password) {
    if(!$this->doc) {
      return NULL;
    }

    $result['action'] = $this->parse_form_action();
    $result['data'] = $this->parse_form_input_data();
    $result['data']['TextBoxUserName'] = $username;
    $result['data']['TextBoxPassword'] = $password;
    return $result;
  }

  public function build_update_report_post($report) {
    $post = array();
    $post['data'] = array();
    $post['action'] = $this->parse_form_action();
    $post['data'] = array_merge($post['data'], $this->build_selects());
    $post['data'] = array_merge($post['data'], $this->build_inputs($report));
    // There are two submit fields. "UpdateButton" and "DoneButton". We always want the former.
    unset($post['data']['DoneButton']);
    return $post;
  }

  public function build_inputs($report) {
    $row = $this->find_row_by_id($report->id);
    if(!$row) {
      return NULL;
    }
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
    foreach ($row->xpath("*/input") as $input) {
      $input_name = (string)$input['name'];
      $split_input_name = split(":",  $input_name);
      /* The inputs are named like this: "OldRowsRepeater:_ctl6:TextboxTimeTo"
       * We want the last part, i.e. "TextboxTimeTo" */
      $element = $split_input_name[2];
      switch($element) {
        case "TextboxTimeFrom":
          $result[$input_name] = $report->begin;
          break;
        case "TextboxTimeTo":
          $result[$input_name] = $report->end;
          break;
        case "TextboxBreak":
          $result[$input_name] = $report->break_duration_minutes;
          break;
        case "CheckboxDayDone":
          if ($report->done) {
            $result[$input_name] = "on";
          } else {
            unset($result[$input_name]);
          }
          break;
        case "TextboxExplicitOvertime":
          $result[$input_name] = $report->overtime_hours;
          break;
        default:
          // Skip over.
      }
    }
    return $result;
  }

  /**
   * Takes all select fields in the document and builds postable arrays of the selected options.
   * @return Array arrays of the form a[select tag name] = option value
   */
  public function build_selects() {
    $selects = $this->doc->xpath("//select");
    foreach ($selects as $select) {
      $name = (string)$select['name'];
      foreach ($select->option as $option) {
        if ($option['selected']) {
          $value = (string) $option['value'];
          $result[$name] = $value;
        }
      }
    }
    return $result;
  }

  /**
   * Returns the value of the "action" attribute in the login form.
   * @return the value of the "action" attribute in the login form if successful, else NULL.
   */
  public function parse_form_action() {
    // Extract form action
    $actionlist = $this->doc->xpath("*/form/@action");
    if(count($actionlist) === 1) {
      return $actionlist[0]['action'];
    }
  }

  /**
   * Fetches the ASP specific "name" and "value" attributes of the input types "hidden" and
   * "submit". They are needed for login.
   * @return Array all fields found, as an array of "name" => "value" if any are found.
   */
  public function parse_form_input_data() {
    $hiddenlist = $this->doc->xpath('//input[@type="hidden"] | //input[@type="submit"]');
    foreach($hiddenlist as $hidden) {
      $name = $hidden['name'];
      $value = $hidden['value'];
      if($name && $value) {
        $result[strval($name)] = strval($value);
      }
    }
    return $result;
  }

  /**
   * Parse assignment titles and codes from the given document
   * @return Array Map of "<jobcode>" => "<jobtitle>"
   */
  public function parse_assignments() {
    $elems = $this->doc->xpath('//select[@id="CustOrdersDropDown"]');
    foreach($elems as $elem) {
      foreach($elem->option as $option) {
        $jobcode = (string)$option['value'];
        if($jobcode != '0') {
          $result[$jobcode] = trim((string)$option);
        }
      }
    }
    return $result;
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
    $hrefs = $this->doc->xpath('//a/@href');
    foreach ($hrefs as $href) {
      $matches = array();
      if (preg_match("/TimeReport\.aspx\?DateInWeek=(\d{4})-\d{2}-\d{2}/", $href, $matches)) {
        $years[$matches[1]] = TRUE;
      }
    }
    ksort($years);
    $years = array_keys($years);

    $table_rows = $this->doc->xpath('//select[@id="SelectExpenses"]/../..');
    $reports = array();
    foreach($table_rows as $row) {
      $report = $this->parse_report_row($row);
      $report->set_year($years);
      $reports[] = $report;
    }
    return $reports;
  }

  /**
   * Parses a HTML table row containing a time report and populates a TZIntellitimeReport.
   * @param SimpleXMLElement $row a SimpleXML element containing a table row plus children, containing report data.
   * @return the populated report or NULL if parsing failed.
   */
  private function parse_report_row($row) {
    // Find this rows unique id..
    $report = new TZIntellitimeReport();
    $onclick_handlers = $row->xpath('td/select/@onclick');
    $id = preg_replace("/^.*Expense.aspx\?(.*)\'.*$/", "$1", (string)$onclick_handlers[0]['onclick']);
    $report->id =  $id;

    // Index 1 has the "done" checkbox.
    $td = $row->td[1];
    $report->done =  $td->input['checked'] ? TRUE : FALSE;

    //Fourth element has the span with a date label.
    $td = $row->td[3];
    $date_matches = array();
    if ($td && preg_match("/(\d{2}).(\d{2})/", strval($td->span), $date_matches)) {
      $report->month = intval($date_matches[2], 10);
      $report->day = intval($date_matches[1], 10);
    };

    $td = $row->td[5];
    $report->title =  $td ? trim((string)$td->span) : NULL;

    // indices 7, 9, 11 => start time, end time, break duration
    $report->begin =  $this->parse_time_field($row->td[7]);
    $report->end =  $this->parse_time_field($row->td[9]);
    $report->break_duration_minutes =  intval($row->td[11]->input[0]['value'],10);
    // TODO: find out if overtime is float or int..
    // 15 is overtime. In hours...
    $report->overtime_hours =  intval($row->td[15]->input[0]['value'], 10);
    return $report;
  }

  /**
   * Matches dd:dd and returns matches as 'hour' and 'minute'
   * @param SimpleXMLElement $td <td> element containing the <input> fields
   */
  private function parse_time_field($td) {
    if (!$td) {
      return NULL;
    }
    return strval($td->input[0]['value']);
  }

  /**
   * Finds a report table row based on the unique report ID.
   * @param string $id the unique ID parsed out by ::parse_report_row()
   */
  public function find_row_by_id($id) {
    $table_rows = $this->doc->xpath('//select[@id="SelectExpenses"]/../..');
    foreach($table_rows as $row) {
      $row_id = preg_replace("/^.*Expense.aspx\?(.*)\'.*$/", "$1", (string)$row->td[17]->select['onclick']);
      if ($row_id === $id) {
        return $row;
      }
    }
    return FALSE;
  }
}

class TZIntellitimeReport {
  public $id = NULL;
  public $done = FALSE;
  public $year = NULL;
  public $month = NULL;
  public $day = NULL;
  public $title = NULL;
  public $begin = NULL;
  public $end = NULL;
  public $break_duration_minutes = 0;
  public $overtime_hours = 0;

  /**
   * This is a utility method that takes an array of years
   * and sets the correct one based on this report's month value.
   * If the array contains less than one year or more than two years,
   * you're doing it wrong.
   *
   * The first and last week of each year may overlap. In that case we need to
   * correctly determine which year a specific report belongs to.
   *
   * @param Array $years an array containing either one year, or two years, sorted lexicographically.
   */
  public function set_year($years) {
    if ($this->month === NULL) {
      return FALSE;
    }
    switch(count($years)) {
      case 1:
        $this->year = $years[0];
        break;
      case 2:
        switch ($this->month) {
          case 12:
            $this->year = $years[0];
            break;
          case 1:
            $this->year = $years[1];
            break;
          default:
            // Hilfe, we have New Year's outside of dec-jan. Jumping leap years, Batman!
            return FALSE;
        }
        break;
      default:
        // Please don't pass arrays with count($years) < 1 or > 2.
        return FALSE;
        break;
    }
    return TRUE;
  }

  public function get_date_string() {
    return sprintf("%04d-%02d-%02d", $this->year, $this->month, $this->day);
  }
}

