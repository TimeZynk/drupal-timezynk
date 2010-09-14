<?php
/**
 * @file implementations of TimeZynk Service calls
 */

/**
 * Login a user using the specified credentials.
 *
 * Note this will transfer a plaintext password.
 *
 * @param $username
 *   Username to be logged in.
 * @param $password
 *   Password, must be plain text and not hashed.
 *
 * @return
 *   A valid user object.
 */
function timezynk_service_login($username, $password) {
  global $user;

  if ($user->uid) {
    // user is already logged in
    return services_error(t('Already logged in as !user.', array('!user' => $user->name)), 406);
  }

  /* Login and run login hooks */
  $user = user_authenticate(array('name' => $username, 'pass' => $password));
  if ($user->uid) {
    watchdog('timezynk_service', 'Login for %name', array('%name' => $user->name));
    
    module_invoke_all('user', 'login', NULL, $user);

    $return = new stdClass();
    $return->uid = $user->uid;
    
    /* Logout and destroy user object */
    session_destroy();
    module_invoke_all('user', 'logout', NULL, $user);
    $user = drupal_anonymous_user();
    
    return $return;
  }
  return services_error(t('Wrong username or password.'), 401);
}

/**
 * Get the most recent time reports for the given user.
 *
 * @param $username
 *   Username to be logged in.
 * @param $password
 *   Password, must be plain text and not hashed.
 * @param $offset
 *   The timereport to start at (default 0)
 * @param $length
 *   Number of time reports to return (default 5)
 *
 * @return
 *   An array of timereports
 */
function timezynk_service_get($offset = 0, $limit = 5) {
  global $user;

  if (!$user->uid) {
    return services_error(t('Not logged in'), 401);
  }
  
  watchdog('timezynk_service', 'Get reports for %name (%offset, %length)', 
    array('%name' => $user->name, '%offset' => $offset, '%length' => $limit));
  
  $list = array();
  
  // Fetch number of available nodes
  $count_query = db_rewrite_sql("SELECT COUNT(DISTINCT n.nid) FROM {node} n WHERE n.uid = '%d' AND n.type = 'timereport'");
  //watchdog('timezynk_service', print_r($count_query, TRUE));
  $count = (int)db_result(db_query($count_query, $user->uid));
  
  $nodes = array();
  
  $list['total'] = $count;
  $list['offset'] = $offset;
  
  // Do we have any nodes left to fetch?
  if($offset < $count) {
    $sql = "SELECT n.nid FROM {node} n INNER JOIN content_type_timereport c ON c.nid = n.nid WHERE n.uid = '%d' AND n.type = 'timereport' ORDER BY c.field_time_value DESC";
    $result = db_query_range(db_rewrite_sql($sql), $user->uid, $offset, $limit);
    while($data = db_fetch_object($result)) {
      $node = node_load($data->nid);
      $start = date_make_date($node->field_time[0]['value'], $node->field_time[0]['timezone']);
      $end = date_make_date($node->field_time[0]['value2'], $node->field_time[0]['timezone']);
      $n = array(
        'nid' => (int)$data->nid,
        'uid' => (int)$node->uid,
        'created' => (int)$node->created,
        'start' => (int)date_format_date($start, 'custom', DATE_FORMAT_UNIX),
        'end' => (int)date_format_date($end, 'custom', DATE_FORMAT_UNIX),
        'break' => (int)$node->field_break[0]['approx_seconds'],
        'kilometers' => (int)$node->field_traveldistance[0]['value'],
        'travelseconds' => (int)$node->field_traveltime[0]['approx_seconds'],
        'signature' => $node->field_signed[0]['value'],
      );
      if(isset($node->field_assignment[0])) {
        $n['assignment_id'] = (int)$node->field_assignment[0]['nid'];
      }
      if(isset($node->field_report_subassignment[0])) {
        $n['subassignment'] = $node->field_report_subassignment[0]['value'];
      }
      $nodes[] = $n;
    }
  }
  
  $list['nodes'] = $nodes;
  return $list;
}