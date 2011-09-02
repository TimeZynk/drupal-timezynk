<?php

define('TZSTATS_INVOICE_CACHE_EXPIRY', 3600);

function tzstats_invoice_page() {
  $month = arg(3);
  if ($month) {
    return _tzstats_invoice_month($month);
  } else {
    return _tzstats_invoice_choose_month();
  }
}

function _tzstats_invoice_choose_month() {
  $output = "";
  $cache_key = 'invoice-select-month';
  if ($cache = cache_get($cache_key)) {
    $output = $cache->data;
  } else {
    $date = date_make_date('now');
    $output = '<ul>';
    for ($i = 0; $i < 6; $i++) {
      $date_string = $date->format('Y-m');
      $invoice = new TZStatsInvoice($date);
      $output .= '<li>' . l($date_string, 'admin/reports/invoice/' . $date_string) .
                 ': ' . theme('money', $invoice->total_cost()) . '</li>';
      $date->modify('-1 month');
    }
    $output .= '</ul>';
    cache_set($cache_key, $output, 'cache', time() + TZSTATS_INVOICE_CACHE_EXPIRY);
  }
  return $output;
}

function _tzstats_invoice_month($month) {
  drupal_set_breadcrumb(array(
    l('Home', '<front>'),
    l('Invoice', 'admin/reports/invoice'),
  ));
  drupal_add_css(drupal_get_path('module', 'tzstats') . '/style/tzstats.css');

  $date = date_make_date($month . "-01");
  $detailed = !empty($_GET['detailed']);
  $invoice = new TZStatsInvoice($date, $detailed);

  drupal_set_title($invoice->title());
  $content = $invoice->format();
  if ($detailed) {
    $content .= l('Hide details', 'admin/reports/invoice/' . $month);
  } else {
    $content .= l('Show details', 'admin/reports/invoice/' . $month, array('query' => array('detailed' => TRUE)));
  }
  return $content;
}

function tzstats_invoice_excluded_users() {
  $list = explode(',', TIMEZYNK_EXCLUDED_USERS);
  $users = array_map('trim', $list);
  $users = array_filter($users);
  $users = array_fill_keys($users, TRUE);
  $users['1'] = TRUE;
  return $users;
}

class TZStatsInvoice {
  /**
   * @var DateTime
   */
  private $start_date;

  /**
   * @var DateTime
   */
  private $end_date;

  private $detailed;
  private $excluded_users;
  private $user_costs;
  private $sms_costs;
  private $minimum_costs;
  private $availability_costs;

  function __construct($month_date, $detailed) {
    $this->start_date = clone($month_date);
    // Move to 00:00:00 first day of month
    $this->start_date->setTime(0, 0, 0);
    $this->start_date->modify('-' . ($this->start_date->format('d') - 1) . ' days');

    // Set end date to 23:59:59 last day of month
    $this->end_date = clone($this->start_date);
    $this->end_date->modify('+1 month');
    $this->end_date->modify('-1 second');

    $this->detailed = $detailed;
    $this->excluded_users = tzstats_invoice_excluded_users();

    $this->user_costs = $this->calculate_user_costs();
    $this->availability_costs = $this->calculate_availability_cost();
    $this->minimum_costs = $this->calculate_minimum_cost($this->user_costs);
    $this->sms_costs = $this->calculate_sms_costs();
  }

  function total_cost() {
    $total_costs = $this->total_user_cost();
    $total_costs += $this->sms_costs->total;
    return $total_costs;
  }

  function format() {
    $headers = array(
      'Item',
      'Count',
      array('data' => '@', 'class' => 'invoice-money'),
      array('data' => 'Total', 'class' => 'invoice-money'),
    );

    $rows = array();
    $this->add_user_cost_rows($rows);
    $this->add_user_total_cost_rows($rows);
    $this->add_sms_cost_rows($rows);
    $this->add_grand_total_row($rows);
    return theme('table', $headers, $rows, array('class' => 'tzstats-invoice-table'));
  }

  function title() {
    return 'Invoice for ' .
      $this->start_date->format('Y-m-d') . ' - ' . $this->end_date->format('Y-m-d');
  }

  private function money_cell($amount) {
    return array(
      'data' => theme('money', $amount),
      'class' => 'invoice-money',
    );
  }

  private function fill_cost_entry_rows(&$rows, $cost_entries) {
    foreach ($cost_entries as $uid => $cost_entry) {
      if (!is_numeric($uid)) {
        continue;
      }
      $account = user_load($uid);
      $rows[] = array(
        $account ? theme('username', $account) : 'User ' . $uid,
        theme('days', $cost_entry->days),
        $this->money_cell($cost_entry->cost/$cost_entry->days),
        $this->money_cell($cost_entry->cost),
      );
    }
  }

  private function format_subtotal($cost_entries, $suffix) {
    return array(
      'data' => array(
        'Subtotal ' . (count($cost_entries) - 2) . ' ' . $suffix,
        theme('days', $cost_entries['total_days']),
        $this->money_cell($cost_entries['total_cost']/$cost_entries['total_days']),
        $this->money_cell($cost_entries['total_cost']),
      ),
      'class' => 'invoice-subtotal',
    );
  }

  private function calculate_minimum_cost(&$rows) {
    $minimum_date = date_make_date(TIMEZYNK_MINIMUM_MONTH_USERS_START_DATE);
    if ($minimum_date > $this->start_date) {
      return $this->actual_user_cost();
    }

    $actual_cost = $this->actual_user_cost();
    $minimum_cost = TIMEZYNK_MINIMUM_MONTH_USERS * TIMEZYNK_MONTH_PRICE;
    return max($actual_cost, $minimum_cost);
  }

  private function add_user_total_cost_rows(&$rows) {
    if ($this->minimum_costs > $this->actual_user_cost()) {
      $rows[] = array(
        'Contracted minimum cost',
        TIMEZYNK_MINIMUM_MONTH_USERS,
        $this->money_cell(TIMEZYNK_MONTH_PRICE),
        $this->money_cell($this->minimum_costs),
      );
      $rows[] = array(
        'Actual user cost',
        theme('days', $this->actual_user_days()),
        $this->money_cell($this->actual_user_cost()/$this->actual_user_days()),
        $this->money_cell(-$this->actual_user_cost()),
      );
    }
    $rows[] = array(
      'data' => array(
        'Total user cost',
        theme('days', $this->actual_user_days()),
        $this->money_cell($this->total_user_cost()/$this->actual_user_days()),
        $this->money_cell($this->total_user_cost()),
      ),
      'class' => 'invoice-subtotal',
    );
  }

  private function add_grand_total_row(&$rows) {
    $rows[] = array(
      'data' => array(
        'Total',
        '',
        '',
        $this->money_cell($this->total_cost()),
      ),
      'class' => 'invoice-total',
    );
  }

  private function add_user_cost_rows(&$rows) {
    if ($this->detailed) {
      $this->fill_cost_entry_rows($rows, $this->user_costs->day);
      $this->fill_cost_entry_rows($rows, $this->user_costs->month);
      $this->fill_availability_cost_rows($rows);
    }
    $rows[] = $this->format_subtotal($this->user_costs->day, 'day users');
    $rows[] = $this->format_subtotal($this->user_costs->month, 'month users');
    $rows[] = $this->format_availability_subtotal();
  }

  private function fill_availability_cost_rows(&$rows) {
    foreach ($this->availability_costs as $uid => $cost_entry) {
      if (!is_numeric($uid)) {
        continue;
      }
      $account = user_load($uid);
      $rows[] = array(
        'Availability ' . ($account ? theme('username', $account) : 'User ' . $uid),
        $cost_entry->count . ' user',
        $this->money_cell($cost_entry->cost/$cost_entry->count),
        $this->money_cell($cost_entry->cost),
      );
    }
  }

  private function format_availability_subtotal() {
    return array(
      'data' => array(
        'Subtotal availability',
        $this->availability_costs['total_count'] . ' users',
        $this->money_cell($this->availability_costs['total_cost']/$this->availability_costs['total_count']),
        $this->money_cell($this->availability_costs['total_cost']),
      ),
      'class' => 'invoice-subtotal',
    );
  }

  private function add_sms_cost_rows(&$rows) {
    $rows[] = array(
      'SMS',
      $this->sms_costs->count . ' SMS',
      $this->money_cell($this->sms_costs->cost),
      $this->money_cell($this->sms_costs->total),
    );
  }

  private function total_user_cost() {
    return $this->minimum_costs;
  }

  private function actual_user_days() {
    return $this->user_costs->day['total_days'] + $this->user_costs->month['total_days'];
  }

  private function actual_user_cost() {
    return $this->user_costs->day['total_cost'] +
           $this->user_costs->month['total_cost'] +
           $this->availability_costs['total_cost'];
  }

  private function calculate_sms_costs() {
    $sql = "SELECT SUM(CEIL(length/160)) FROM {tzstats_sms} sms " .
           "WHERE sms.timestamp >= %d AND sms.timestamp <= %d";

    $result = db_result(db_query($sql, $this->start_date->format('U'), $this->end_date->format('U')));

    $cost_entry = new stdClass();
    $cost_entry->count = $result ? $result : 0;
    $cost_entry->cost = TIMEZYNK_SMS_PRICE;
    $cost_entry->total = $cost_entry->count * $cost_entry->cost;

    return $cost_entry;
  }

  private function calculate_availability_cost() {
    $sql = "SELECT * FROM {tzstats_availability} WHERE timestamp >= %d AND timestamp <= %d";
    $result = db_query($sql, $this->start_date->format('U'), $this->end_date->format('U'));
    $cost_entries = array();
    while ($row = db_fetch_object($result)) {
      if (!empty($this->excluded_users[$row->uid])) {
        continue;
      }
      $cost_entries[$row->uid] = (object)array(
        'uid' => $row->uid,
        'count' => 1,
        'cost' => TIMEZYNK_AVAILABILITY_PRICE,
        'total' => TIMEZYNK_AVAILABILITY_PRICE,
      );
    }
    $cost_entries['total_count'] = count($cost_entries);
    $cost_entries['total_cost'] = $cost_entries['total_count'] * TIMEZYNK_AVAILABILITY_PRICE;
    return $cost_entries;
  }

  private function calculate_user_costs() {
    $user_bucket = $this->fill_user_bucket();

    // Separate bucket into day and month users
    $costs = new stdClass();
    $costs->day = array();
    $costs->day['total_cost'] = 0;
    $costs->day['total_days'] = 0;
    $costs->month = array();
    $costs->month['total_cost'] = 0;
    $costs->month['total_days'] = 0;
    foreach ($user_bucket as $uid => $bucket) {
      $cost_entry = (object)array(
        'days' => count($bucket),
        'cost' => array_sum($bucket),
      );

      if ($cost_entry->cost < TIMEZYNK_MONTH_PRICE) {
        $costs->day[$uid] = $cost_entry;
        $costs->day['total_cost'] += $cost_entry->cost;
        $costs->day['total_days'] += $cost_entry->days;
      } else {
        $cost_entry->cost = TIMEZYNK_MONTH_PRICE;
        $costs->month[$uid] = $cost_entry;
        $costs->month['total_cost'] += $cost_entry->cost;
        $costs->month['total_days'] += $cost_entry->days;
      }
    }

    return $costs;
  }

  private function fill_user_bucket() {
    $user_bucket = array();

    $sql = "SELECT * FROM {tzstats_report} tzstats_report INNER JOIN {tzreport} tzreport ON tzreport.vid = tzstats_report.vid " .
           "WHERE tzstats_report.timestamp >= %d AND tzstats_report.timestamp <= %d " .
           "ORDER BY tzstats_report.uid";

    $result = db_query($sql, $this->start_date->format('U'), $this->end_date->format('U'));
    while ($entry = db_fetch_object($result)) {
      if (!empty($this->excluded_users[$entry->uid])) {
        continue;
      }

      $date = tzbase_make_date($entry->begintime);
      $user_bucket[$entry->uid][$date->format('Ymd')] = TIMEZYNK_DAY_PRICE;
    }

    $sql = "SELECT * FROM {tzstats_login} " .
           "WHERE timestamp >= %d AND timestamp <= %d " .
           "ORDER BY uid";

    $result = db_query($sql, $this->start_date->format('U'), $this->end_date->format('U'));
    while ($entry = db_fetch_object($result)) {
      if (!empty($this->excluded_users[$entry->uid])) {
        continue;
      }

      if (empty($user_bucket[$entry->uid])) {
        continue;
      }

      $date = tzbase_make_date($entry->timestamp);
      $user_bucket[$entry->uid][$date->format('Ymd')] = TIMEZYNK_DAY_PRICE;
    }
    return $user_bucket;
  }
}
