<?php
class TZFlags extends PBEnum
{
  const CREATED  = 0;
  const SIGNED_IN  = 1;
  const REPORTED  = 2;
  const APPROVED  = 3;
  const LOCKED  = 4;
  const DELETED  = 255;
}
class TZJobFlags extends PBEnum
{
  const ACTIVE  = 0;
  const INACTIVE  = 255;
}
class TZReport extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
    $this->values["1"] = new PBInt();
    $this->values["1"]->value = 0;
    $this->fields["2"] = "PBInt";
    $this->values["2"] = "";
    $this->values["2"] = new PBInt();
    $this->values["2"]->value = 0;
    $this->fields["3"] = "PBInt";
    $this->values["3"] = "";
    $this->values["3"] = new PBInt();
    $this->values["3"]->value = 0;
    $this->fields["4"] = "PBString";
    $this->values["4"] = "";
    $this->fields["5"] = "PBString";
    $this->values["5"] = "";
    $this->fields["6"] = "PBInt";
    $this->values["6"] = "";
    $this->fields["7"] = "PBInt";
    $this->values["7"] = "";
    $this->fields["8"] = "PBInt";
    $this->values["8"] = "";
    $this->fields["9"] = "PBInt";
    $this->values["9"] = "";
    $this->fields["10"] = "PBInt";
    $this->values["10"] = "";
    $this->fields["11"] = "PBInt";
    $this->values["11"] = "";
    $this->fields["16"] = "PBString";
    $this->values["16"] = "";
    $this->fields["17"] = "PBString";
    $this->values["17"] = "";
    $this->fields["18"] = "PBInt";
    $this->values["18"] = "";
    $this->fields["19"] = "PBInt";
    $this->values["19"] = "";
    $this->fields["20"] = "PBBool";
    $this->values["20"] = "";
    $this->values["20"] = new PBBool();
    $this->values["20"]->value = false;
    $this->fields["21"] = "PBBool";
    $this->values["21"] = "";
    $this->values["21"] = new PBBool();
    $this->values["21"]->value = false;
    $this->fields["22"] = "PBBool";
    $this->values["22"] = "";
    $this->values["22"] = new PBBool();
    $this->values["22"]->value = false;
  }
  function id()
  {
    return $this->_get_value("1");
  }
  function set_id($value)
  {
    return $this->_set_value("1", $value);
  }
  function vid()
  {
    return $this->_get_value("2");
  }
  function set_vid($value)
  {
    return $this->_set_value("2", $value);
  }
  function job_id()
  {
    return $this->_get_value("3");
  }
  function set_job_id($value)
  {
    return $this->_set_value("3", $value);
  }
  function title()
  {
    return $this->_get_value("4");
  }
  function set_title($value)
  {
    return $this->_set_value("4", $value);
  }
  function description()
  {
    return $this->_get_value("5");
  }
  function set_description($value)
  {
    return $this->_set_value("5", $value);
  }
  function assigned_to()
  {
    return $this->_get_value("6");
  }
  function set_assigned_to($value)
  {
    return $this->_set_value("6", $value);
  }
  function begin_time()
  {
    return $this->_get_value("7");
  }
  function set_begin_time($value)
  {
    return $this->_set_value("7", $value);
  }
  function end_time()
  {
    return $this->_get_value("8");
  }
  function set_end_time($value)
  {
    return $this->_set_value("8", $value);
  }
  function break_duration()
  {
    return $this->_get_value("9");
  }
  function set_break_duration($value)
  {
    return $this->_set_value("9", $value);
  }
  function flags()
  {
    return $this->_get_value("10");
  }
  function set_flags($value)
  {
    return $this->_set_value("10", $value);
  }
  function changed()
  {
    return $this->_get_value("11");
  }
  function set_changed($value)
  {
    return $this->_set_value("11", $value);
  }
  function signature()
  {
    return $this->_get_value("16");
  }
  function set_signature($value)
  {
    return $this->_set_value("16", $value);
  }
  function comments()
  {
    return $this->_get_value("17");
  }
  function set_comments($value)
  {
    return $this->_set_value("17", $value);
  }
  function travel_duration()
  {
    return $this->_get_value("18");
  }
  function set_travel_duration($value)
  {
    return $this->_set_value("18", $value);
  }
  function travel_km()
  {
    return $this->_get_value("19");
  }
  function set_travel_km($value)
  {
    return $this->_set_value("19", $value);
  }
  function may_remove()
  {
    return $this->_get_value("20");
  }
  function set_may_remove($value)
  {
    return $this->_set_value("20", $value);
  }
  function may_edit()
  {
    return $this->_get_value("21");
  }
  function set_may_edit($value)
  {
    return $this->_set_value("21", $value);
  }
  function may_reset()
  {
    return $this->_get_value("22");
  }
  function set_may_reset($value)
  {
    return $this->_set_value("22", $value);
  }
}
class TZJob extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
    $this->values["1"] = new PBInt();
    $this->values["1"]->value = 0;
    $this->fields["2"] = "PBInt";
    $this->values["2"] = "";
    $this->fields["3"] = "PBInt";
    $this->values["3"] = "";
    $this->fields["4"] = "PBString";
    $this->values["4"] = "";
    $this->fields["5"] = "PBString";
    $this->values["5"] = "";
    $this->fields["6"] = "PBString";
    $this->values["6"] = "";
    $this->fields["7"] = "PBInt";
    $this->values["7"] = "";
    $this->fields["18"] = "PBBool";
    $this->values["18"] = "";
    $this->values["18"] = new PBBool();
    $this->values["18"]->value = false;
    $this->fields["19"] = "PBBool";
    $this->values["19"] = "";
    $this->values["19"] = new PBBool();
    $this->values["19"]->value = false;
    $this->fields["20"] = "PBBool";
    $this->values["20"] = "";
    $this->values["20"] = new PBBool();
    $this->values["20"]->value = false;
  }
  function id()
  {
    return $this->_get_value("1");
  }
  function set_id($value)
  {
    return $this->_set_value("1", $value);
  }
  function parent_id()
  {
    return $this->_get_value("2");
  }
  function set_parent_id($value)
  {
    return $this->_set_value("2", $value);
  }
  function flags()
  {
    return $this->_get_value("3");
  }
  function set_flags($value)
  {
    return $this->_set_value("3", $value);
  }
  function job_code()
  {
    return $this->_get_value("4");
  }
  function set_job_code($value)
  {
    return $this->_set_value("4", $value);
  }
  function title()
  {
    return $this->_get_value("5");
  }
  function set_title($value)
  {
    return $this->_set_value("5", $value);
  }
  function description()
  {
    return $this->_get_value("6");
  }
  function set_description($value)
  {
    return $this->_set_value("6", $value);
  }
  function changed()
  {
    return $this->_get_value("7");
  }
  function set_changed($value)
  {
    return $this->_set_value("7", $value);
  }
  function may_create_child()
  {
    return $this->_get_value("18");
  }
  function set_may_create_child($value)
  {
    return $this->_set_value("18", $value);
  }
  function may_create_report()
  {
    return $this->_get_value("19");
  }
  function set_may_create_report($value)
  {
    return $this->_set_value("19", $value);
  }
  function may_edit()
  {
    return $this->_get_value("20");
  }
  function set_may_edit($value)
  {
    return $this->_set_value("20", $value);
  }
}
class TZUser extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
    $this->values["1"] = new PBInt();
    $this->values["1"]->value = 0;
    $this->fields["2"] = "PBString";
    $this->values["2"] = "";
    $this->fields["3"] = "PBString";
    $this->values["3"] = "";
    $this->fields["4"] = "PBString";
    $this->values["4"] = "";
    $this->fields["16"] = "PBBool";
    $this->values["16"] = "";
    $this->values["16"] = new PBBool();
    $this->values["16"]->value = false;
  }
  function id()
  {
    return $this->_get_value("1");
  }
  function set_id($value)
  {
    return $this->_set_value("1", $value);
  }
  function username()
  {
    return $this->_get_value("2");
  }
  function set_username($value)
  {
    return $this->_set_value("2", $value);
  }
  function realname()
  {
    return $this->_get_value("3");
  }
  function set_realname($value)
  {
    return $this->_set_value("3", $value);
  }
  function email()
  {
    return $this->_get_value("4");
  }
  function set_email($value)
  {
    return $this->_set_value("4", $value);
  }
  function may_create_job()
  {
    return $this->_get_value("16");
  }
  function set_may_create_job($value)
  {
    return $this->_set_value("16", $value);
  }
}
class TZGetUserCmd extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
    $this->values["1"] = new PBInt();
    $this->values["1"]->value = 0;
  }
  function unused()
  {
    return $this->_get_value("1");
  }
  function set_unused($value)
  {
    return $this->_set_value("1", $value);
  }
}
class TZSyncCmd extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
    $this->fields["2"] = "TZJob";
    $this->values["2"] = array();
    $this->fields["3"] = "TZReport";
    $this->values["3"] = array();
  }
  function last_sync()
  {
    return $this->_get_value("1");
  }
  function set_last_sync($value)
  {
    return $this->_set_value("1", $value);
  }
  function new_job($offset)
  {
    return $this->_get_arr_value("2", $offset);
  }
  function add_new_job()
  {
    return $this->_add_arr_value("2");
  }
  function set_new_job($index, $value)
  {
    $this->_set_arr_value("2", $index, $value);
  }
  function remove_last_new_job()
  {
    $this->_remove_last_arr_value("2");
  }
  function new_job_size()
  {
    return $this->_get_arr_size("2");
  }
  function new_report($offset)
  {
    return $this->_get_arr_value("3", $offset);
  }
  function add_new_report()
  {
    return $this->_add_arr_value("3");
  }
  function set_new_report($index, $value)
  {
    $this->_set_arr_value("3", $index, $value);
  }
  function remove_last_new_report()
  {
    $this->_remove_last_arr_value("3");
  }
  function new_report_size()
  {
    return $this->_get_arr_size("3");
  }
}
class TZGetReportCmd extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
    $this->fields["2"] = "PBInt";
    $this->values["2"] = "";
    $this->values["2"] = new PBInt();
    $this->values["2"]->value = 0;
    $this->fields["3"] = "PBInt";
    $this->values["3"] = "";
    $this->values["3"] = new PBInt();
    $this->values["3"]->value = 0;
    $this->fields["4"] = "PBInt";
    $this->values["4"] = "";
    $this->values["4"] = new PBInt();
    $this->values["4"]->value = 0;
    $this->fields["5"] = "PBBool";
    $this->values["5"] = "";
    $this->values["5"] = new PBBool();
    $this->values["5"]->value = true;
  }
  function report_id()
  {
    return $this->_get_value("1");
  }
  function set_report_id($value)
  {
    return $this->_set_value("1", $value);
  }
  function changed_after()
  {
    return $this->_get_value("2");
  }
  function set_changed_after($value)
  {
    return $this->_set_value("2", $value);
  }
  function limit()
  {
    return $this->_get_value("3");
  }
  function set_limit($value)
  {
    return $this->_set_value("3", $value);
  }
  function offset()
  {
    return $this->_get_value("4");
  }
  function set_offset($value)
  {
    return $this->_set_value("4", $value);
  }
  function include_deleted()
  {
    return $this->_get_value("5");
  }
  function set_include_deleted($value)
  {
    return $this->_set_value("5", $value);
  }
}
class TZGetReportResult extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
    $this->fields["2"] = "PBInt";
    $this->values["2"] = "";
    $this->fields["3"] = "TZReport";
    $this->values["3"] = array();
  }
  function total_report_count()
  {
    return $this->_get_value("1");
  }
  function set_total_report_count($value)
  {
    return $this->_set_value("1", $value);
  }
  function offset()
  {
    return $this->_get_value("2");
  }
  function set_offset($value)
  {
    return $this->_set_value("2", $value);
  }
  function report($offset)
  {
    return $this->_get_arr_value("3", $offset);
  }
  function add_report()
  {
    return $this->_add_arr_value("3");
  }
  function set_report($index, $value)
  {
    $this->_set_arr_value("3", $index, $value);
  }
  function remove_last_report()
  {
    $this->_remove_last_arr_value("3");
  }
  function report_size()
  {
    return $this->_get_arr_size("3");
  }
}
class TZCreateReportCmd extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["2"] = "TZReport";
    $this->values["2"] = "";
  }
  function new_report()
  {
    return $this->_get_value("2");
  }
  function set_new_report($value)
  {
    return $this->_set_value("2", $value);
  }
}
class TZCreateReportResult extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
  }
  function id()
  {
    return $this->_get_value("1");
  }
  function set_id($value)
  {
    return $this->_set_value("1", $value);
  }
}
class TZGetJobCmd extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
    $this->fields["2"] = "PBInt";
    $this->values["2"] = "";
    $this->fields["3"] = "PBInt";
    $this->values["3"] = "";
    $this->fields["4"] = "PBInt";
    $this->values["4"] = "";
  }
  function job_id()
  {
    return $this->_get_value("1");
  }
  function set_job_id($value)
  {
    return $this->_set_value("1", $value);
  }
  function changed_after()
  {
    return $this->_get_value("2");
  }
  function set_changed_after($value)
  {
    return $this->_set_value("2", $value);
  }
  function limit()
  {
    return $this->_get_value("3");
  }
  function set_limit($value)
  {
    return $this->_set_value("3", $value);
  }
  function offset()
  {
    return $this->_get_value("4");
  }
  function set_offset($value)
  {
    return $this->_set_value("4", $value);
  }
}
class TZGetJobResult extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
    $this->fields["2"] = "PBInt";
    $this->values["2"] = "";
    $this->fields["3"] = "TZJob";
    $this->values["3"] = array();
  }
  function total_job_count()
  {
    return $this->_get_value("1");
  }
  function set_total_job_count($value)
  {
    return $this->_set_value("1", $value);
  }
  function offset()
  {
    return $this->_get_value("2");
  }
  function set_offset($value)
  {
    return $this->_set_value("2", $value);
  }
  function job($offset)
  {
    return $this->_get_arr_value("3", $offset);
  }
  function add_job()
  {
    return $this->_add_arr_value("3");
  }
  function set_job($index, $value)
  {
    $this->_set_arr_value("3", $index, $value);
  }
  function remove_last_job()
  {
    $this->_remove_last_arr_value("3");
  }
  function job_size()
  {
    return $this->_get_arr_size("3");
  }
}
class TZCreateJobCmd extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "TZJob";
    $this->values["1"] = "";
  }
  function new_job()
  {
    return $this->_get_value("1");
  }
  function set_new_job($value)
  {
    return $this->_set_value("1", $value);
  }
}
class TZCreateJobResult extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
  }
  function id()
  {
    return $this->_get_value("1");
  }
  function set_id($value)
  {
    return $this->_set_value("1", $value);
  }
}
class TZCommand extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
    $this->values["1"] = new PBInt();
    $this->values["1"]->value = 0;
    $this->fields["2"] = "TZGetUserCmd";
    $this->values["2"] = "";
    $this->fields["3"] = "TZSyncCmd";
    $this->values["3"] = "";
    $this->fields["4"] = "TZGetReportCmd";
    $this->values["4"] = "";
    $this->fields["5"] = "TZCreateReportCmd";
    $this->values["5"] = "";
    $this->fields["6"] = "TZGetJobCmd";
    $this->values["6"] = "";
    $this->fields["7"] = "TZCreateJobCmd";
    $this->values["7"] = "";
  }
  function client_handle()
  {
    return $this->_get_value("1");
  }
  function set_client_handle($value)
  {
    return $this->_set_value("1", $value);
  }
  function get_user_cmd()
  {
    return $this->_get_value("2");
  }
  function set_get_user_cmd($value)
  {
    return $this->_set_value("2", $value);
  }
  function sync_cmd()
  {
    return $this->_get_value("3");
  }
  function set_sync_cmd($value)
  {
    return $this->_set_value("3", $value);
  }
  function get_report_cmd()
  {
    return $this->_get_value("4");
  }
  function set_get_report_cmd($value)
  {
    return $this->_set_value("4", $value);
  }
  function create_report_cmd()
  {
    return $this->_get_value("5");
  }
  function set_create_report_cmd($value)
  {
    return $this->_set_value("5", $value);
  }
  function get_job_cmd()
  {
    return $this->_get_value("6");
  }
  function set_get_job_cmd($value)
  {
    return $this->_set_value("6", $value);
  }
  function create_job_cmd()
  {
    return $this->_get_value("7");
  }
  function set_create_job_cmd($value)
  {
    return $this->_set_value("7", $value);
  }
}
class TZResult extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBInt";
    $this->values["1"] = "";
    $this->values["1"] = new PBInt();
    $this->values["1"]->value = 0;
    $this->fields["2"] = "PBInt";
    $this->values["2"] = "";
    $this->fields["3"] = "PBString";
    $this->values["3"] = "";
    $this->fields["4"] = "TZUser";
    $this->values["4"] = "";
    $this->fields["5"] = "TZReport";
    $this->values["5"] = array();
    $this->fields["6"] = "TZJob";
    $this->values["6"] = array();
    $this->fields["7"] = "TZGetReportResult";
    $this->values["7"] = "";
    $this->fields["8"] = "TZCreateReportResult";
    $this->values["8"] = "";
    $this->fields["9"] = "TZGetJobResult";
    $this->values["9"] = "";
    $this->fields["10"] = "TZCreateJobResult";
    $this->values["10"] = "";
  }
  function client_handle()
  {
    return $this->_get_value("1");
  }
  function set_client_handle($value)
  {
    return $this->_set_value("1", $value);
  }
  function error_code()
  {
    return $this->_get_value("2");
  }
  function set_error_code($value)
  {
    return $this->_set_value("2", $value);
  }
  function error_msg()
  {
    return $this->_get_value("3");
  }
  function set_error_msg($value)
  {
    return $this->_set_value("3", $value);
  }
  function user()
  {
    return $this->_get_value("4");
  }
  function set_user($value)
  {
    return $this->_set_value("4", $value);
  }
  function report($offset)
  {
    return $this->_get_arr_value("5", $offset);
  }
  function add_report()
  {
    return $this->_add_arr_value("5");
  }
  function set_report($index, $value)
  {
    $this->_set_arr_value("5", $index, $value);
  }
  function remove_last_report()
  {
    $this->_remove_last_arr_value("5");
  }
  function report_size()
  {
    return $this->_get_arr_size("5");
  }
  function job($offset)
  {
    return $this->_get_arr_value("6", $offset);
  }
  function add_job()
  {
    return $this->_add_arr_value("6");
  }
  function set_job($index, $value)
  {
    $this->_set_arr_value("6", $index, $value);
  }
  function remove_last_job()
  {
    $this->_remove_last_arr_value("6");
  }
  function job_size()
  {
    return $this->_get_arr_size("6");
  }
  function get_report_result()
  {
    return $this->_get_value("7");
  }
  function set_get_report_result($value)
  {
    return $this->_set_value("7", $value);
  }
  function create_report_result()
  {
    return $this->_get_value("8");
  }
  function set_create_report_result($value)
  {
    return $this->_set_value("8", $value);
  }
  function get_job_result()
  {
    return $this->_get_value("9");
  }
  function set_get_job_result($value)
  {
    return $this->_set_value("9", $value);
  }
  function create_job_result()
  {
    return $this->_get_value("10");
  }
  function set_create_job_result($value)
  {
    return $this->_set_value("10", $value);
  }
}
class TZRequest extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "PBString";
    $this->values["1"] = "";
    $this->fields["2"] = "PBString";
    $this->values["2"] = "";
    $this->fields["3"] = "TZCommand";
    $this->values["3"] = array();
  }
  function username()
  {
    return $this->_get_value("1");
  }
  function set_username($value)
  {
    return $this->_set_value("1", $value);
  }
  function password()
  {
    return $this->_get_value("2");
  }
  function set_password($value)
  {
    return $this->_set_value("2", $value);
  }
  function command($offset)
  {
    return $this->_get_arr_value("3", $offset);
  }
  function add_command()
  {
    return $this->_add_arr_value("3");
  }
  function set_command($index, $value)
  {
    $this->_set_arr_value("3", $index, $value);
  }
  function remove_last_command()
  {
    $this->_remove_last_arr_value("3");
  }
  function command_size()
  {
    return $this->_get_arr_size("3");
  }
}
class TZResponse extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    $this->fields["1"] = "TZResult";
    $this->values["1"] = array();
    $this->fields["2"] = "PBInt";
    $this->values["2"] = "";
  }
  function result($offset)
  {
    return $this->_get_arr_value("1", $offset);
  }
  function add_result()
  {
    return $this->_add_arr_value("1");
  }
  function set_result($index, $value)
  {
    $this->_set_arr_value("1", $index, $value);
  }
  function remove_last_result()
  {
    $this->_remove_last_arr_value("1");
  }
  function result_size()
  {
    return $this->_get_arr_size("1");
  }
  function timestamp()
  {
    return $this->_get_value("2");
  }
  function set_timestamp($value)
  {
    return $this->_set_value("2", $value);
  }
}
?>