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
   * An array containing the cookiejar file path and
   * the last known effective URL.
   * Fields are referenced as $session_data['cookiejar']
   * and $session_data['last_effective_url']
   * @var Array
   */
  private $session_data = NULL;

  /**
   * Creates a new TZIntellitimeBot
   * @param string $url the customer login URL for intellitime.
   * @param Array $session_data Optional session data from a previous session
   */
  function __construct($url, $session_data = NULL) {
    $handle = curl_init();
    $this->handle = $handle;
    $this->login_url = $url;

    // Resume session if session data is available, otherwise initialize new
    if($session_data) {
      if(!is_file($session_data['cookiejar']) || !(filesize($session_data['cookiejar']) > 0)) {
        throw new InvalidArgumentException('session_data contained invalid cookie jar');
      }
      $this->session_data = $session_data;
    } else {
      $this->session_data = array(
        'cookiejar' => tempnam(file_directory_temp(), 'tzintellitimebot'),
      );
    }

    // Set standard curl options
    $curl_opt = array(
      CURLOPT_USERAGENT => "TimeZynk/1.0",
      CURLOPT_COOKIEFILE => $this->session_data['cookiejar'],
      CURLOPT_COOKIEJAR => $this->session_data['cookiejar'],
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

  /**
   * Fetch all available intellitime data for the week that contains the given date
   * @param int $date date as timestamp
   * @return FALSE on network failure, empty array on login failure, otherwise array with keys
   *   'assignments', 'reports' and 'unfinished_weeks'.
   */
  public function load_week($date = 0) {
    // Use today as default date
    if($date === 0) {
      $date = time();
    }

    $datestr = date("Y-m-d", $date);
    $dateaction = "TimeReport/TimeReport.aspx?DateInWeek=$datestr";

    $url = $this->build_action_url($dateaction);
    $parser = $this->request($url);
    if(!$parser) {
      return FALSE;
    }

    return $this->week_data($parser);
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
      CURLOPT_SSL_VERIFYPEER => FALSE, // Turn of SSL verification since intellitime uses a non-standard CA
      CURLOPT_SSL_VERIFYHOST => 2,
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
      $this->session_data['last_effective_url'] = curl_getinfo($this->handle, CURLINFO_EFFECTIVE_URL);
      return new TZIntellitimeParser($output);
    }
    watchdog('tzintellitime', 'Request failed: @error', array('@error' => curl_error($this->handle)), WATCHDOG_WARNING);
    return FALSE;
  }

  /**
   *  Strip away <Dirname>/<File>.aspx?<query> from current effective URL and append $action instead
   * @param string $url the URL as a string
   * @param string $action the action as a string
   * @return the newly constructed URL
   */
  private function build_action_url($action) {
    $base_url = preg_replace("/[^\/]+\/[^\/]+\.aspx.*$/", "", $this->session_data['last_effective_url']);
    return $base_url . $action;
  }

  /**
   * Send an updated report to the intellitime web service.
   * @param TZIntellitimeReport $report a report containing the updated values.
   * @return FALSE on network failure, empty array on login failure, otherwise array with keys
   *   'assignments', 'reports' and 'unfinished_weeks'.
   */
  public function update_report($report) {
    $datestr = $report->get_date_string();
    $dateaction = "TimeReport/TimeReport.aspx?DateInWeek=$datestr";

    // We need fresh info about the time reports for build_update_report_post.
    $url = $this->build_action_url($dateaction);
    $parser = $this->request($url);
    $reports = $parser->parse_reports();

    // Check the current state of the report that has changed
    foreach($reports as $old_report) {
      if ($report->id === $old_report->id) {
        if ($report == $old_report) {
          // No need to do anything, the report has not changed.
          return $this->week_data($parser);
        } else if ($old_report->state == TZIntellitimeReport::STATE_REPORTED) {
          // First open the report
          $old_state = $report->state;
          $report->state = TZIntellitimeReport::STATE_OPEN;
          $parser = $this->post_report($parser, $dateaction, $report);
          $report->state = $old_state;
        } else if ($old_report->state == TZIntellitimeReport::STATE_LOCKED) {
          // Report locked, nothing can be done
          return $this->week_data($parser);
        }

        // Send the post.
        $parser = $this->post_report($parser, $dateaction, $report);
        break;
      }
    }

    return $this->week_data($parser);
  }

  private function post_report($parser, $dateaction, $report) {
    $post_url = $this->build_action_url($dateaction);
    $post = $parser->build_update_report_post($report);
    return $this->request($post_url, $post['data']);
  }

  /**
   * Fetches persistable session data that can be used to resume
   * this session with a different TZIntellitimeBot object.
   * @return Array session_data
   */
  public function get_session_data() {
    return $this->session_data;
  }

  /**
   * Parse current week data.
   * @param TZIntellitimeParser $parser
   * @return Array array with keys 'assignments', 'reports' and 'unfinished_weeks' or empty array if not logged in
   */
  private function week_data($parser) {
    $result = array();
    if($parser->is_valid_login()) {
      $assignments = $parser->parse_assignments();
      $absence_types = $parser->parse_absence_types();
      $result['assignments'] = array_merge($assignments, $absence_types);
      $result['reports'] = $parser->parse_reports();
      $result['unfinished_weeks'] = $parser->parse_unfinished_weeks();
    }
    return $result;
  }

  /**
   * Check if session data could describe a valid session
   * @param Array $session_data
   * @return TRUE if session data could be valid, else FALSE
   */
  public static function is_valid_session_data($session_data) {
    return isset($session_data['cookiejar']) &&
      file_exists($session_data['cookiejar']) &&
      isset($session_data['last_effective_url']) &&
      strlen($session_data['last_effective_url']);
  }

  /**
   * Destroy session data and free associated resources
   * @param Array $session_data
   */
  public static function destroy_session_data(&$session_data) {
    if(isset($session_data['cookiejar']) && file_exists($session_data['cookiejar'])) {
      unlink($session_data['cookiejar']);
      unset($session_data['cookiejar']);
    }
    unset($session_data['last_effective_url']);
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

  /**
   * Process all form inputs and build a POST:able array containing the new data
   * from $report
   *
   * @param TZIntellitimeReport $report
   * @return Array HTTP POST fields
   */
  public function build_inputs($report) {
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

    // Check if $report is found among the input rows
    $row = $this->find_row_by_id($report->id);
    if($row) {
      // Found! Update fields from $report
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
            if ($report->state == TZIntellitimeReport::STATE_OPEN) {
              unset($result[$input_name]);
            } else {
              $result[$input_name] = "on";
            }
            break;
          case "TextboxExplicitOvertime":
            $result[$input_name] = $report->overtime_hours;
            break;
          case "CheckBoxDelete":
            if ($report->state == TZIntellitimeReport::STATE_DELETED) {
              $result[$input_name] = "on";
             } else {
               unset($result[$input_name]);
             }
             break;
          default:
            // Skip over.
        }
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
    $result = array();
    $elems = $this->doc->xpath('//select[@id="CustOrdersDropDown"]/option');
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
    return $result;
  }

  /**
   * Parse absence types and titles from the current document.
   * @return Array Map of <absence type code> => <absence type title>
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
   * @return Array Timestamp describing one of the days in the week that have unfinished reports
   */
  public function parse_unfinished_weeks() {
    $elems = $this->doc->xpath('//a[@class="WeeksNav"]');
    foreach($elems as $a) {
      $match = array();
      if(preg_match('/DateInWeek=(\d{4}-\d{2}-\d{2})/', $a['href'], $match)) {
        $result[] = strtotime($match[1]);
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
    $years = array();
    $hrefs = $this->doc->xpath('//a/@href');
    foreach ($hrefs as $href) {
      $matches = array();
      if (preg_match("/TimeReport\.aspx\?DateInWeek=(\d{4})-\d{2}-\d{2}/", (string)$href, $matches)) {
        $years[$matches[1]] = TRUE;
      }
    }
    ksort($years);
    $years = array_keys($years);

    $reports = array();
    $table_rows = $this->doc->xpath('//select[@id="SelectExpenses"]/../..');
    if(!empty($table_rows)) {
      foreach($table_rows as $row) {
        $report = $this->parse_report_row($row);
        $report->set_year($years);
        $reports[] = $report;
      }
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

    // Index 1 has either a "done" checkbox or the "locked" icon.
    $td = $row->td[1];
    if($td->input['checked']) {
      $report->state = TZIntellitimeReport::STATE_REPORTED;
    } elseif(FALSE !== stripos($td->img['src'], 'lock')) {
      $report->state = TZIntellitimeReport::STATE_LOCKED;
    }

    //Fourth element has the span with a date label.
    $td = $row->td[3];
    $date_matches = array();
    if ($td && preg_match('/(\d{2}).(\d{2})/', strval($td->span), $date_matches)) {
      $report->month = intval($date_matches[2], 10);
      $report->day = intval($date_matches[1], 10);
    };

    $td = $row->td[5];
    $report->title =  $td ? trim((string)$td->span) : NULL;

    $rowprefix = (string) $row->td[7]['id'];
    $rowprefix = preg_replace('/_[^_]+$/', '', $rowprefix);

    // indices 7, 9, 11 => start time, end time, break duration
    $report->begin =  $this->parse_time_field($row->td[7]);
    $report->end =  $this->parse_time_field($row->td[9]);
    $report->break_duration_minutes =  intval($row->td[11]->input[0]['value'],10);
    // TODO: find out if overtime is float or int..
    // 15 is overtime. In hours...
    $report->overtime_hours =  intval($row->td[15]->input[0]['value'], 10);

    /*
     * Intellitime places the comment field on the next row,
     * so we cannot check the row td like we do with the time fields.
     */
    $commentelement = $row->xpath('..//input[@id=\'' . $rowprefix . '_TextboxNote\']');
    if (!empty($commentelement[0]['value'])) {
      $report->comment = trim((string) $commentelement[0]['value']);
    }

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
  const STATE_OPEN = 0;
  const STATE_REPORTED = 1;
  const STATE_LOCKED = 2;
  const STATE_DELETED = 255;

  public $id = NULL;
  public $state = TZIntellitimeReport::STATE_OPEN;
  public $year = NULL;
  public $month = NULL;
  public $day = NULL;
  public $title = NULL;
  public $begin = NULL;
  public $end = NULL;
  public $break_duration_minutes = 0;
  public $overtime_hours = 0;
  public $comment = '';

  /**
   * Construct an intellitime report from a tzreport
   */
  public function __construct($tzreport = NULL) {
    if($tzreport) {
      if(isset($tzreport->intellitime_id)) {
        $id = unserialize($tzreport->intellitime_id);
        $this->id = $id[2];
      }
      if($tzreport->flags < TZFlags::REPORTED) {
        $this->state = TZIntellitimeReport::STATE_OPEN;
      } elseif($tzreport->flags == TZFlags::REPORTED) {
        $this->state = TZIntellitimeReport::STATE_REPORTED;
      } elseif($tzreport->flags == TZFlags::DELETED) {
        $this->state = TZIntellitimeReport::STATE_DELETED;
      } else {
        $this->state = TZIntellitimeReport::STATE_LOCKED;
      }

      $this->year = date('Y', $tzreport->begintime);
      $this->month = date('m', $tzreport->begintime);
      $this->day = date('d', $tzreport->begintime);

      $this->title = $tzreport->title;

      $this->begin = date('H:i', $tzreport->begintime);
      $this->end = date('H:i', $tzreport->endtime);

      $this->break_duration_minutes = round($tzreport->breakduration/60);
    }
  }

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

  /**
   * Convert this TZIntellitimeReport to a tzreport node. If a previous
   * node is available, it is updated instead.
   * @param array $account user object associated with this report
   * @param array $tzreport optional tzreport node to update
   * @return array new or updated tzreport node
   */
  public function convert_to_tzreport($account, $tzreport = NULL) {
    tzbase_include_proto_classes();

    if(!$tzreport) {
      $tzreport = new stdClass();
      $tzreport->type = 'tzreport';
      $tzreport->travelduration = 0;
      $tzreport->travelkm = 0;
      $tzreport->signature = '';
      $tzreport->name = $account->name;
      $tzreport->uid = $account->uid;
    }

    $tzreport->title = $this->title;
    $tzreport->body = $this->comment;

    // Convert begin and end to epoch timestamps
    $tzreport->begintime = $this->get_in_epoch($this->begin);
    $tzreport->endtime = $this->get_in_epoch($this->end);
    if ($tzreport->endtime < $tzreport->begintime) {
      // endtime can only be smaller if we have passed midnight. Add one day to endtime
      $time = explode(':', $this->end);
      $tzreport->endtime = mktime($time[0], $time[1], 0, $this->month, $this->day + 1, $this->year);
    }
    $tzreport->breakduration = $this->break_duration_minutes*60;

    $tzreport->intellitime_id = $this->get_intellitime_id($account->uid);

    switch($this->state) {
      case TZIntellitimeReport::STATE_OPEN:
        $tzreport->flags = TZFlags::CREATED;
        break;
      case TZIntellitimeReport::STATE_REPORTED:
        $tzreport->flags = TZFlags::REPORTED;
        break;
      default:
        $tzreport->flags = TZFlags::LOCKED;
        break;
    }

    $tzreport->assignedto = $account->uid;

    return $tzreport;
  }

  /**
   * Get the unique report ID that will be used when saving
   * this report as a tzreport.
   * @param string uid User ID of assigned user
   * @return string unique ID
   */
  public function get_intellitime_id($uid) {
    return serialize(array($this->get_date_string(), (string)$uid, (string)$this->id));
  }

  /**
   * Gets an epoch time stamp based on the reports year and month, day attributes
   * and the passed time string.
   * @param string $time_string e.g "07:00"
   */
  private function get_in_epoch($time_string) {
    return strtotime($this->get_date_string() . "T" . $time_string);
  }
}

class TZIntellitimeAssignment {
  const TYPE_ASSIGNMENT = 0;
  const TYPE_ABSENCE = 1;

  public $id = NULL;
  public $report_key = NULL;
  public $title = NULL;
  public $type = TZIntellitimeAssignment::TYPE_ASSIGNMENT;

  public function convert_to_tzjob($account, $tzjob = NULL) {
    if (!$tzjob) {
      // New job, set default fields
      $tzjob = new stdClass();
      $tzjob->type = 'tzjob';
      $tzjob->body = '';
      $tzjob->jobcode = '';
      $tzjob->name = $account->name;
      $tzjob->uid = $account->uid;
      $tzjob->parentid = 0;
      $tzjob->flags = TZJobFlags::ACTIVE;
    }

    $tzjob->title = $this->title;
    $tzjob->intellitime_id = $this->id;
    return $tzjob;
  }
}

