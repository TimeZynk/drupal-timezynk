<?php
class TZFlags extends PBEnum
{
  const CREATED  = 0;
  const SIGNED_IN  = 1;
  const REPORTED  = 2;
  const APPROVED  = 3;
  const LOCKED  = 4;
  const DELETED  = 255;

  public function __construct($reader=null)
  {
   	parent::__construct($reader);
 	$this->names = array(
			0 => "CREATED",
			1 => "SIGNED_IN",
			2 => "REPORTED",
			3 => "APPROVED",
			4 => "LOCKED",
			255 => "DELETED");
   }
}
class TZJobFlags extends PBEnum
{
  const ACTIVE  = 0;
  const INACTIVE  = 255;

  public function __construct($reader=null)
  {
   	parent::__construct($reader);
 	$this->names = array(
			0 => "ACTIVE",
			255 => "INACTIVE");
   }
}
class TZJobType extends PBEnum
{
  const PRESENCE  = 0;
  const ABSENCE  = 1;

  public function __construct($reader=null)
  {
   	parent::__construct($reader);
 	$this->names = array(
			0 => "PRESENCE",
			1 => "ABSENCE");
   }
}
class TZConstants extends PBEnum
{
  const VERSION  = 2;

  public function __construct($reader=null)
  {
   	parent::__construct($reader);
 	$this->names = array(
			2 => "VERSION");
   }
}
class TZTime extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZTime"]["1"] = "PBInt";
    $this->values["1"] = "";
    self::$fieldNames["TZTime"]["1"] = "hour";
    self::$fields["TZTime"]["2"] = "PBInt";
    $this->values["2"] = "";
    self::$fieldNames["TZTime"]["2"] = "minute";
  }
  function hour()
  {
    return $this->_get_value("1");
  }
  function set_hour($value)
  {
    return $this->_set_value("1", $value);
  }
  function minute()
  {
    return $this->_get_value("2");
  }
  function set_minute($value)
  {
    return $this->_set_value("2", $value);
  }
}
class TZDate extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZDate"]["1"] = "PBInt";
    $this->values["1"] = "";
    self::$fieldNames["TZDate"]["1"] = "year";
    self::$fields["TZDate"]["2"] = "PBInt";
    $this->values["2"] = "";
    self::$fieldNames["TZDate"]["2"] = "month";
    self::$fields["TZDate"]["3"] = "PBInt";
    $this->values["3"] = "";
    self::$fieldNames["TZDate"]["3"] = "day";
  }
  function year()
  {
    return $this->_get_value("1");
  }
  function set_year($value)
  {
    return $this->_set_value("1", $value);
  }
  function month()
  {
    return $this->_get_value("2");
  }
  function set_month($value)
  {
    return $this->_set_value("2", $value);
  }
  function day()
  {
    return $this->_get_value("3");
  }
  function set_day($value)
  {
    return $this->_set_value("3", $value);
  }
}
class TZDateRange extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZDateRange"]["1"] = "TZDate";
    $this->values["1"] = "";
    self::$fieldNames["TZDateRange"]["1"] = "start";
    self::$fields["TZDateRange"]["2"] = "TZDate";
    $this->values["2"] = "";
    self::$fieldNames["TZDateRange"]["2"] = "end";
  }
  function start()
  {
    return $this->_get_value("1");
  }
  function set_start($value)
  {
    return $this->_set_value("1", $value);
  }
  function end()
  {
    return $this->_get_value("2");
  }
  function set_end($value)
  {
    return $this->_set_value("2", $value);
  }
}
class TZTimeSpan extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZTimeSpan"]["1"] = "TZDate";
    $this->values["1"] = "";
    self::$fieldNames["TZTimeSpan"]["1"] = "date";
    self::$fields["TZTimeSpan"]["2"] = "TZTime";
    $this->values["2"] = "";
    self::$fieldNames["TZTimeSpan"]["2"] = "start";
    self::$fields["TZTimeSpan"]["3"] = "TZTime";
    $this->values["3"] = "";
    self::$fieldNames["TZTimeSpan"]["3"] = "end";
  }
  function date()
  {
    return $this->_get_value("1");
  }
  function set_date($value)
  {
    return $this->_set_value("1", $value);
  }
  function start()
  {
    return $this->_get_value("2");
  }
  function set_start($value)
  {
    return $this->_set_value("2", $value);
  }
  function end()
  {
    return $this->_get_value("3");
  }
  function set_end($value)
  {
    return $this->_set_value("3", $value);
  }
}
class TZReport extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZReport"]["1"] = "PBInt";
    $this->values["1"] = "";
    $this->values["1"] = new PBInt();
    $this->values["1"]->value = 0;
    self::$fieldNames["TZReport"]["1"] = "id";
    self::$fields["TZReport"]["2"] = "PBInt";
    $this->values["2"] = "";
    $this->values["2"] = new PBInt();
    $this->values["2"]->value = 0;
    self::$fieldNames["TZReport"]["2"] = "vid";
    self::$fields["TZReport"]["3"] = "PBInt";
    $this->values["3"] = "";
    $this->values["3"] = new PBInt();
    $this->values["3"]->value = 0;
    self::$fieldNames["TZReport"]["3"] = "job_id";
    self::$fields["TZReport"]["4"] = "PBString";
    $this->values["4"] = "";
    self::$fieldNames["TZReport"]["4"] = "title";
    self::$fields["TZReport"]["5"] = "PBString";
    $this->values["5"] = "";
    self::$fieldNames["TZReport"]["5"] = "description";
    self::$fields["TZReport"]["6"] = "PBInt";
    $this->values["6"] = "";
    self::$fieldNames["TZReport"]["6"] = "assigned_to";
    self::$fields["TZReport"]["7"] = "PBInt";
    $this->values["7"] = "";
    self::$fieldNames["TZReport"]["7"] = "begin_time";
    self::$fields["TZReport"]["8"] = "PBInt";
    $this->values["8"] = "";
    self::$fieldNames["TZReport"]["8"] = "end_time";
    self::$fields["TZReport"]["9"] = "PBInt";
    $this->values["9"] = "";
    self::$fieldNames["TZReport"]["9"] = "break_duration";
    self::$fields["TZReport"]["10"] = "PBInt";
    $this->values["10"] = "";
    self::$fieldNames["TZReport"]["10"] = "flags";
    self::$fields["TZReport"]["11"] = "PBInt";
    $this->values["11"] = "";
    self::$fieldNames["TZReport"]["11"] = "changed";
    self::$fields["TZReport"]["12"] = "TZTimeSpan";
    $this->values["12"] = "";
    self::$fieldNames["TZReport"]["12"] = "worked_time";
    self::$fields["TZReport"]["16"] = "PBString";
    $this->values["16"] = "";
    self::$fieldNames["TZReport"]["16"] = "signature";
    self::$fields["TZReport"]["17"] = "PBString";
    $this->values["17"] = "";
    self::$fieldNames["TZReport"]["17"] = "comments";
    self::$fields["TZReport"]["18"] = "PBInt";
    $this->values["18"] = "";
    self::$fieldNames["TZReport"]["18"] = "travel_duration";
    self::$fields["TZReport"]["19"] = "PBInt";
    $this->values["19"] = "";
    self::$fieldNames["TZReport"]["19"] = "travel_km";
    self::$fields["TZReport"]["20"] = "PBBool";
    $this->values["20"] = "";
    $this->values["20"] = new PBBool();
    $this->values["20"]->value = false;
    self::$fieldNames["TZReport"]["20"] = "may_remove";
    self::$fields["TZReport"]["21"] = "PBBool";
    $this->values["21"] = "";
    $this->values["21"] = new PBBool();
    $this->values["21"]->value = false;
    self::$fieldNames["TZReport"]["21"] = "may_edit";
    self::$fields["TZReport"]["22"] = "PBBool";
    $this->values["22"] = "";
    $this->values["22"] = new PBBool();
    $this->values["22"]->value = false;
    self::$fieldNames["TZReport"]["22"] = "may_reset";
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
  function worked_time()
  {
    return $this->_get_value("12");
  }
  function set_worked_time($value)
  {
    return $this->_set_value("12", $value);
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
    self::$fields["TZJob"]["1"] = "PBInt";
    $this->values["1"] = "";
    $this->values["1"] = new PBInt();
    $this->values["1"]->value = 0;
    self::$fieldNames["TZJob"]["1"] = "id";
    self::$fields["TZJob"]["2"] = "PBInt";
    $this->values["2"] = "";
    self::$fieldNames["TZJob"]["2"] = "parent_id";
    self::$fields["TZJob"]["3"] = "PBInt";
    $this->values["3"] = "";
    self::$fieldNames["TZJob"]["3"] = "flags";
    self::$fields["TZJob"]["4"] = "PBString";
    $this->values["4"] = "";
    self::$fieldNames["TZJob"]["4"] = "job_code";
    self::$fields["TZJob"]["5"] = "PBString";
    $this->values["5"] = "";
    self::$fieldNames["TZJob"]["5"] = "title";
    self::$fields["TZJob"]["6"] = "PBString";
    $this->values["6"] = "";
    self::$fieldNames["TZJob"]["6"] = "description";
    self::$fields["TZJob"]["7"] = "PBInt";
    $this->values["7"] = "";
    self::$fieldNames["TZJob"]["7"] = "changed";
    self::$fields["TZJob"]["8"] = "TZJobType";
    $this->values["8"] = "";
    self::$fieldNames["TZJob"]["8"] = "job_type";
    self::$fields["TZJob"]["18"] = "PBBool";
    $this->values["18"] = "";
    $this->values["18"] = new PBBool();
    $this->values["18"]->value = false;
    self::$fieldNames["TZJob"]["18"] = "may_create_child";
    self::$fields["TZJob"]["19"] = "PBBool";
    $this->values["19"] = "";
    $this->values["19"] = new PBBool();
    $this->values["19"]->value = false;
    self::$fieldNames["TZJob"]["19"] = "may_create_report";
    self::$fields["TZJob"]["20"] = "PBBool";
    $this->values["20"] = "";
    $this->values["20"] = new PBBool();
    $this->values["20"]->value = false;
    self::$fieldNames["TZJob"]["20"] = "may_edit";
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
  function job_type()
  {
    return $this->_get_value("8");
  }
  function set_job_type($value)
  {
    return $this->_set_value("8", $value);
  }
  function job_type_string()
  {
    return $this->values["8"]->get_description();
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
    self::$fields["TZUser"]["1"] = "PBInt";
    $this->values["1"] = "";
    $this->values["1"] = new PBInt();
    $this->values["1"]->value = 0;
    self::$fieldNames["TZUser"]["1"] = "id";
    self::$fields["TZUser"]["2"] = "PBString";
    $this->values["2"] = "";
    self::$fieldNames["TZUser"]["2"] = "username";
    self::$fields["TZUser"]["3"] = "PBString";
    $this->values["3"] = "";
    self::$fieldNames["TZUser"]["3"] = "realname";
    self::$fields["TZUser"]["4"] = "PBString";
    $this->values["4"] = "";
    self::$fieldNames["TZUser"]["4"] = "email";
    self::$fields["TZUser"]["16"] = "PBBool";
    $this->values["16"] = "";
    $this->values["16"] = new PBBool();
    $this->values["16"]->value = false;
    self::$fieldNames["TZUser"]["16"] = "may_create_job";
    self::$fields["TZUser"]["17"] = "PBBool";
    $this->values["17"] = "";
    $this->values["17"] = new PBBool();
    $this->values["17"]->value = false;
    self::$fieldNames["TZUser"]["17"] = "may_create_reports";
    self::$fields["TZUser"]["18"] = "PBBool";
    $this->values["18"] = "";
    $this->values["18"] = new PBBool();
    $this->values["18"]->value = false;
    self::$fieldNames["TZUser"]["18"] = "may_create_availability";
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
  function may_create_reports()
  {
    return $this->_get_value("17");
  }
  function set_may_create_reports($value)
  {
    return $this->_set_value("17", $value);
  }
  function may_create_availability()
  {
    return $this->_get_value("18");
  }
  function set_may_create_availability($value)
  {
    return $this->_set_value("18", $value);
  }
}
class TZAvailabilityType extends PBEnum
{
  const AVAILABLE  = 0;
  const NOT_AVAILABLE  = 1;

  public function __construct($reader=null)
  {
   	parent::__construct($reader);
 	$this->names = array(
			0 => "AVAILABLE",
			1 => "NOT_AVAILABLE");
   }
}
class TZAvailability extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZAvailability"]["1"] = "PBInt";
    $this->values["1"] = "";
    self::$fieldNames["TZAvailability"]["1"] = "id";
    self::$fields["TZAvailability"]["2"] = "TZAvailabilityType";
    $this->values["2"] = "";
    self::$fieldNames["TZAvailability"]["2"] = "type";
    self::$fields["TZAvailability"]["3"] = "TZTimeSpan";
    $this->values["3"] = "";
    self::$fieldNames["TZAvailability"]["3"] = "time_span";
  }
  function id()
  {
    return $this->_get_value("1");
  }
  function set_id($value)
  {
    return $this->_set_value("1", $value);
  }
  function type()
  {
    return $this->_get_value("2");
  }
  function set_type($value)
  {
    return $this->_set_value("2", $value);
  }
  function type_string()
  {
    return $this->values["2"]->get_description();
  }
  function time_span()
  {
    return $this->_get_value("3");
  }
  function set_time_span($value)
  {
    return $this->_set_value("3", $value);
  }
}
class TZAvailabilityInterval extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZAvailabilityInterval"]["1"] = "TZTime";
    $this->values["1"] = "";
    self::$fieldNames["TZAvailabilityInterval"]["1"] = "start";
    self::$fields["TZAvailabilityInterval"]["2"] = "TZTime";
    $this->values["2"] = "";
    self::$fieldNames["TZAvailabilityInterval"]["2"] = "end";
    self::$fields["TZAvailabilityInterval"]["3"] = "PBBool";
    $this->values["3"] = "";
    $this->values["3"] = new PBBool();
    $this->values["3"]->value = false;
    self::$fieldNames["TZAvailabilityInterval"]["3"] = "exclusive";
  }
  function start()
  {
    return $this->_get_value("1");
  }
  function set_start($value)
  {
    return $this->_set_value("1", $value);
  }
  function end()
  {
    return $this->_get_value("2");
  }
  function set_end($value)
  {
    return $this->_set_value("2", $value);
  }
  function exclusive()
  {
    return $this->_get_value("3");
  }
  function set_exclusive($value)
  {
    return $this->_set_value("3", $value);
  }
}
class TZGetUserCmd extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZGetUserCmd"]["1"] = "PBInt";
    $this->values["1"] = "";
    $this->values["1"] = new PBInt();
    $this->values["1"]->value = 0;
    self::$fieldNames["TZGetUserCmd"]["1"] = "version_code";
  }
  function version_code()
  {
    return $this->_get_value("1");
  }
  function set_version_code($value)
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
    self::$fields["TZSyncCmd"]["1"] = "PBInt";
    $this->values["1"] = "";
    self::$fieldNames["TZSyncCmd"]["1"] = "last_sync";
    self::$fields["TZSyncCmd"]["2"] = "TZJob";
    $this->values["2"] = array();
    self::$fieldNames["TZSyncCmd"]["2"] = "new_job";
    self::$fields["TZSyncCmd"]["3"] = "TZReport";
    $this->values["3"] = array();
    self::$fieldNames["TZSyncCmd"]["3"] = "new_report";
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
  function set_all_new_jobs($values)
  {
    return $this->_set_arr_values("2", $values);
  }
  function remove_last_new_job()
  {
    $this->_remove_last_arr_value("2");
  }
  function new_jobs_size()
  {
    return $this->_get_arr_size("2");
  }
  function get_new_jobs()
  {
    return $this->_get_value("2");
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
  function set_all_new_reports($values)
  {
    return $this->_set_arr_values("3", $values);
  }
  function remove_last_new_report()
  {
    $this->_remove_last_arr_value("3");
  }
  function new_reports_size()
  {
    return $this->_get_arr_size("3");
  }
  function get_new_reports()
  {
    return $this->_get_value("3");
  }
}
class TZGetReportCmd extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZGetReportCmd"]["1"] = "PBInt";
    $this->values["1"] = "";
    self::$fieldNames["TZGetReportCmd"]["1"] = "report_id";
    self::$fields["TZGetReportCmd"]["2"] = "PBInt";
    $this->values["2"] = "";
    $this->values["2"] = new PBInt();
    $this->values["2"]->value = 0;
    self::$fieldNames["TZGetReportCmd"]["2"] = "changed_after";
    self::$fields["TZGetReportCmd"]["3"] = "PBInt";
    $this->values["3"] = "";
    $this->values["3"] = new PBInt();
    $this->values["3"]->value = 0;
    self::$fieldNames["TZGetReportCmd"]["3"] = "limit";
    self::$fields["TZGetReportCmd"]["4"] = "PBInt";
    $this->values["4"] = "";
    $this->values["4"] = new PBInt();
    $this->values["4"]->value = 0;
    self::$fieldNames["TZGetReportCmd"]["4"] = "offset";
    self::$fields["TZGetReportCmd"]["5"] = "TZFlags";
    $this->values["5"] = "";
    $this->values["5"] = new TZFlags();
    $this->values["5"]->value = TZFlags::DELETED;
    self::$fieldNames["TZGetReportCmd"]["5"] = "max_flag";
    self::$fields["TZGetReportCmd"]["6"] = "PBInt";
    $this->values["6"] = "";
    self::$fieldNames["TZGetReportCmd"]["6"] = "before";
    self::$fields["TZGetReportCmd"]["7"] = "PBInt";
    $this->values["7"] = "";
    self::$fieldNames["TZGetReportCmd"]["7"] = "after";
    self::$fields["TZGetReportCmd"]["8"] = "TZDateRange";
    $this->values["8"] = "";
    self::$fieldNames["TZGetReportCmd"]["8"] = "date_range";
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
  function max_flag()
  {
    return $this->_get_value("5");
  }
  function set_max_flag($value)
  {
    return $this->_set_value("5", $value);
  }
  function max_flag_string()
  {
    return $this->values["5"]->get_description();
  }
  function before()
  {
    return $this->_get_value("6");
  }
  function set_before($value)
  {
    return $this->_set_value("6", $value);
  }
  function after()
  {
    return $this->_get_value("7");
  }
  function set_after($value)
  {
    return $this->_set_value("7", $value);
  }
  function date_range()
  {
    return $this->_get_value("8");
  }
  function set_date_range($value)
  {
    return $this->_set_value("8", $value);
  }
}
class TZGetReportResult extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZGetReportResult"]["1"] = "PBInt";
    $this->values["1"] = "";
    self::$fieldNames["TZGetReportResult"]["1"] = "total_report_count";
    self::$fields["TZGetReportResult"]["2"] = "PBInt";
    $this->values["2"] = "";
    self::$fieldNames["TZGetReportResult"]["2"] = "offset";
    self::$fields["TZGetReportResult"]["3"] = "TZReport";
    $this->values["3"] = array();
    self::$fieldNames["TZGetReportResult"]["3"] = "report";
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
  function set_all_reports($values)
  {
    return $this->_set_arr_values("3", $values);
  }
  function remove_last_report()
  {
    $this->_remove_last_arr_value("3");
  }
  function reports_size()
  {
    return $this->_get_arr_size("3");
  }
  function get_reports()
  {
    return $this->_get_value("3");
  }
}
class TZCreateReportCmd extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZCreateReportCmd"]["2"] = "TZReport";
    $this->values["2"] = "";
    self::$fieldNames["TZCreateReportCmd"]["2"] = "new_report";
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
    self::$fields["TZCreateReportResult"]["1"] = "PBInt";
    $this->values["1"] = "";
    self::$fieldNames["TZCreateReportResult"]["1"] = "id";
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
    self::$fields["TZGetJobCmd"]["1"] = "PBInt";
    $this->values["1"] = "";
    self::$fieldNames["TZGetJobCmd"]["1"] = "job_id";
    self::$fields["TZGetJobCmd"]["2"] = "PBInt";
    $this->values["2"] = "";
    self::$fieldNames["TZGetJobCmd"]["2"] = "changed_after";
    self::$fields["TZGetJobCmd"]["3"] = "PBInt";
    $this->values["3"] = "";
    self::$fieldNames["TZGetJobCmd"]["3"] = "limit";
    self::$fields["TZGetJobCmd"]["4"] = "PBInt";
    $this->values["4"] = "";
    self::$fieldNames["TZGetJobCmd"]["4"] = "offset";
    self::$fields["TZGetJobCmd"]["5"] = "PBBool";
    $this->values["5"] = "";
    self::$fieldNames["TZGetJobCmd"]["5"] = "may_create_report";
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
  function may_create_report()
  {
    return $this->_get_value("5");
  }
  function set_may_create_report($value)
  {
    return $this->_set_value("5", $value);
  }
}
class TZGetJobResult extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZGetJobResult"]["1"] = "PBInt";
    $this->values["1"] = "";
    self::$fieldNames["TZGetJobResult"]["1"] = "total_job_count";
    self::$fields["TZGetJobResult"]["2"] = "PBInt";
    $this->values["2"] = "";
    self::$fieldNames["TZGetJobResult"]["2"] = "offset";
    self::$fields["TZGetJobResult"]["3"] = "TZJob";
    $this->values["3"] = array();
    self::$fieldNames["TZGetJobResult"]["3"] = "job";
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
  function set_all_jobs($values)
  {
    return $this->_set_arr_values("3", $values);
  }
  function remove_last_job()
  {
    $this->_remove_last_arr_value("3");
  }
  function jobs_size()
  {
    return $this->_get_arr_size("3");
  }
  function get_jobs()
  {
    return $this->_get_value("3");
  }
}
class TZCreateJobCmd extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZCreateJobCmd"]["1"] = "TZJob";
    $this->values["1"] = "";
    self::$fieldNames["TZCreateJobCmd"]["1"] = "new_job";
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
    self::$fields["TZCreateJobResult"]["1"] = "PBInt";
    $this->values["1"] = "";
    self::$fieldNames["TZCreateJobResult"]["1"] = "id";
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
class TZGetAvailabilityCmd extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZGetAvailabilityCmd"]["1"] = "PBInt";
    $this->values["1"] = "";
    self::$fieldNames["TZGetAvailabilityCmd"]["1"] = "id";
    self::$fields["TZGetAvailabilityCmd"]["2"] = "TZDateRange";
    $this->values["2"] = "";
    self::$fieldNames["TZGetAvailabilityCmd"]["2"] = "date_range";
  }
  function id()
  {
    return $this->_get_value("1");
  }
  function set_id($value)
  {
    return $this->_set_value("1", $value);
  }
  function date_range()
  {
    return $this->_get_value("2");
  }
  function set_date_range($value)
  {
    return $this->_set_value("2", $value);
  }
}
class TZGetAvailabilityResult extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZGetAvailabilityResult"]["1"] = "TZAvailability";
    $this->values["1"] = array();
    self::$fieldNames["TZGetAvailabilityResult"]["1"] = "availability";
  }
  function availability($offset)
  {
    return $this->_get_arr_value("1", $offset);
  }
  function add_availability()
  {
    return $this->_add_arr_value("1");
  }
  function set_availability($index, $value)
  {
    $this->_set_arr_value("1", $index, $value);
  }
  function set_all_availabilitys($values)
  {
    return $this->_set_arr_values("1", $values);
  }
  function remove_last_availability()
  {
    $this->_remove_last_arr_value("1");
  }
  function availabilitys_size()
  {
    return $this->_get_arr_size("1");
  }
  function get_availabilitys()
  {
    return $this->_get_value("1");
  }
}
class TZSaveAvailabilityCmd extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZSaveAvailabilityCmd"]["1"] = "TZAvailability";
    $this->values["1"] = "";
    self::$fieldNames["TZSaveAvailabilityCmd"]["1"] = "availability";
  }
  function availability()
  {
    return $this->_get_value("1");
  }
  function set_availability($value)
  {
    return $this->_set_value("1", $value);
  }
}
class TZSaveAvailabilityResult extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZSaveAvailabilityResult"]["1"] = "PBInt";
    $this->values["1"] = "";
    self::$fieldNames["TZSaveAvailabilityResult"]["1"] = "id";
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
class TZDeleteAvailabilityCmd extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZDeleteAvailabilityCmd"]["1"] = "PBInt";
    $this->values["1"] = "";
    self::$fieldNames["TZDeleteAvailabilityCmd"]["1"] = "id";
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
class TZGetAvailabilityIntervalsCmd extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZGetAvailabilityIntervalsCmd"]["1"] = "PBInt";
    $this->values["1"] = "";
    self::$fieldNames["TZGetAvailabilityIntervalsCmd"]["1"] = "unused";
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
class TZGetAvailabilityIntervalsResult extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZGetAvailabilityIntervalsResult"]["1"] = "PBBool";
    $this->values["1"] = "";
    $this->values["1"] = new PBBool();
    $this->values["1"]->value = false;
    self::$fieldNames["TZGetAvailabilityIntervalsResult"]["1"] = "enabled";
    self::$fields["TZGetAvailabilityIntervalsResult"]["2"] = "TZAvailabilityInterval";
    $this->values["2"] = array();
    self::$fieldNames["TZGetAvailabilityIntervalsResult"]["2"] = "interval";
  }
  function enabled()
  {
    return $this->_get_value("1");
  }
  function set_enabled($value)
  {
    return $this->_set_value("1", $value);
  }
  function interval($offset)
  {
    return $this->_get_arr_value("2", $offset);
  }
  function add_interval()
  {
    return $this->_add_arr_value("2");
  }
  function set_interval($index, $value)
  {
    $this->_set_arr_value("2", $index, $value);
  }
  function set_all_intervals($values)
  {
    return $this->_set_arr_values("2", $values);
  }
  function remove_last_interval()
  {
    $this->_remove_last_arr_value("2");
  }
  function intervals_size()
  {
    return $this->_get_arr_size("2");
  }
  function get_intervals()
  {
    return $this->_get_value("2");
  }
}
class TZGetReportTemplatesCmd extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZGetReportTemplatesCmd"]["1"] = "PBInt";
    $this->values["1"] = "";
    self::$fieldNames["TZGetReportTemplatesCmd"]["1"] = "unused";
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
class TZGetReportTemplatesResult extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZGetReportTemplatesResult"]["1"] = "TZReport";
    $this->values["1"] = array();
    self::$fieldNames["TZGetReportTemplatesResult"]["1"] = "report";
  }
  function report($offset)
  {
    return $this->_get_arr_value("1", $offset);
  }
  function add_report()
  {
    return $this->_add_arr_value("1");
  }
  function set_report($index, $value)
  {
    $this->_set_arr_value("1", $index, $value);
  }
  function set_all_reports($values)
  {
    return $this->_set_arr_values("1", $values);
  }
  function remove_last_report()
  {
    $this->_remove_last_arr_value("1");
  }
  function reports_size()
  {
    return $this->_get_arr_size("1");
  }
  function get_reports()
  {
    return $this->_get_value("1");
  }
}
class TZCommand extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZCommand"]["1"] = "PBInt";
    $this->values["1"] = "";
    $this->values["1"] = new PBInt();
    $this->values["1"]->value = 0;
    self::$fieldNames["TZCommand"]["1"] = "client_handle";
    self::$fields["TZCommand"]["2"] = "TZGetUserCmd";
    $this->values["2"] = "";
    self::$fieldNames["TZCommand"]["2"] = "get_user_cmd";
    self::$fields["TZCommand"]["3"] = "TZSyncCmd";
    $this->values["3"] = "";
    self::$fieldNames["TZCommand"]["3"] = "sync_cmd";
    self::$fields["TZCommand"]["4"] = "TZGetReportCmd";
    $this->values["4"] = "";
    self::$fieldNames["TZCommand"]["4"] = "get_report_cmd";
    self::$fields["TZCommand"]["5"] = "TZCreateReportCmd";
    $this->values["5"] = "";
    self::$fieldNames["TZCommand"]["5"] = "create_report_cmd";
    self::$fields["TZCommand"]["6"] = "TZGetJobCmd";
    $this->values["6"] = "";
    self::$fieldNames["TZCommand"]["6"] = "get_job_cmd";
    self::$fields["TZCommand"]["7"] = "TZCreateJobCmd";
    $this->values["7"] = "";
    self::$fieldNames["TZCommand"]["7"] = "create_job_cmd";
    self::$fields["TZCommand"]["8"] = "TZGetAvailabilityCmd";
    $this->values["8"] = "";
    self::$fieldNames["TZCommand"]["8"] = "get_availability_cmd";
    self::$fields["TZCommand"]["9"] = "TZSaveAvailabilityCmd";
    $this->values["9"] = "";
    self::$fieldNames["TZCommand"]["9"] = "save_availability_cmd";
    self::$fields["TZCommand"]["10"] = "TZDeleteAvailabilityCmd";
    $this->values["10"] = "";
    self::$fieldNames["TZCommand"]["10"] = "delete_availability_cmd";
    self::$fields["TZCommand"]["11"] = "TZGetAvailabilityIntervalsCmd";
    $this->values["11"] = "";
    self::$fieldNames["TZCommand"]["11"] = "get_availability_intervals_cmd";
    self::$fields["TZCommand"]["12"] = "TZGetReportTemplatesCmd";
    $this->values["12"] = "";
    self::$fieldNames["TZCommand"]["12"] = "get_report_templates_cmd";
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
  function get_availability_cmd()
  {
    return $this->_get_value("8");
  }
  function set_get_availability_cmd($value)
  {
    return $this->_set_value("8", $value);
  }
  function save_availability_cmd()
  {
    return $this->_get_value("9");
  }
  function set_save_availability_cmd($value)
  {
    return $this->_set_value("9", $value);
  }
  function delete_availability_cmd()
  {
    return $this->_get_value("10");
  }
  function set_delete_availability_cmd($value)
  {
    return $this->_set_value("10", $value);
  }
  function get_availability_intervals_cmd()
  {
    return $this->_get_value("11");
  }
  function set_get_availability_intervals_cmd($value)
  {
    return $this->_set_value("11", $value);
  }
  function get_report_templates_cmd()
  {
    return $this->_get_value("12");
  }
  function set_get_report_templates_cmd($value)
  {
    return $this->_set_value("12", $value);
  }
}
class TZResult extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZResult"]["1"] = "PBInt";
    $this->values["1"] = "";
    $this->values["1"] = new PBInt();
    $this->values["1"]->value = 0;
    self::$fieldNames["TZResult"]["1"] = "client_handle";
    self::$fields["TZResult"]["2"] = "PBInt";
    $this->values["2"] = "";
    self::$fieldNames["TZResult"]["2"] = "error_code";
    self::$fields["TZResult"]["3"] = "PBString";
    $this->values["3"] = "";
    self::$fieldNames["TZResult"]["3"] = "error_msg";
    self::$fields["TZResult"]["4"] = "TZUser";
    $this->values["4"] = "";
    self::$fieldNames["TZResult"]["4"] = "user";
    self::$fields["TZResult"]["5"] = "TZReport";
    $this->values["5"] = array();
    self::$fieldNames["TZResult"]["5"] = "report";
    self::$fields["TZResult"]["6"] = "TZJob";
    $this->values["6"] = array();
    self::$fieldNames["TZResult"]["6"] = "job";
    self::$fields["TZResult"]["7"] = "TZGetReportResult";
    $this->values["7"] = "";
    self::$fieldNames["TZResult"]["7"] = "get_report_result";
    self::$fields["TZResult"]["8"] = "TZCreateReportResult";
    $this->values["8"] = "";
    self::$fieldNames["TZResult"]["8"] = "create_report_result";
    self::$fields["TZResult"]["9"] = "TZGetJobResult";
    $this->values["9"] = "";
    self::$fieldNames["TZResult"]["9"] = "get_job_result";
    self::$fields["TZResult"]["10"] = "TZCreateJobResult";
    $this->values["10"] = "";
    self::$fieldNames["TZResult"]["10"] = "create_job_result";
    self::$fields["TZResult"]["11"] = "TZGetAvailabilityResult";
    $this->values["11"] = "";
    self::$fieldNames["TZResult"]["11"] = "get_availability_result";
    self::$fields["TZResult"]["12"] = "TZSaveAvailabilityResult";
    $this->values["12"] = "";
    self::$fieldNames["TZResult"]["12"] = "save_availability_result";
    self::$fields["TZResult"]["13"] = "TZGetAvailabilityIntervalsResult";
    $this->values["13"] = "";
    self::$fieldNames["TZResult"]["13"] = "get_availability_intervals_result";
    self::$fields["TZResult"]["14"] = "TZGetReportTemplatesResult";
    $this->values["14"] = "";
    self::$fieldNames["TZResult"]["14"] = "get_report_templates_result";
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
  function set_all_reports($values)
  {
    return $this->_set_arr_values("5", $values);
  }
  function remove_last_report()
  {
    $this->_remove_last_arr_value("5");
  }
  function reports_size()
  {
    return $this->_get_arr_size("5");
  }
  function get_reports()
  {
    return $this->_get_value("5");
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
  function set_all_jobs($values)
  {
    return $this->_set_arr_values("6", $values);
  }
  function remove_last_job()
  {
    $this->_remove_last_arr_value("6");
  }
  function jobs_size()
  {
    return $this->_get_arr_size("6");
  }
  function get_jobs()
  {
    return $this->_get_value("6");
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
  function get_availability_result()
  {
    return $this->_get_value("11");
  }
  function set_get_availability_result($value)
  {
    return $this->_set_value("11", $value);
  }
  function save_availability_result()
  {
    return $this->_get_value("12");
  }
  function set_save_availability_result($value)
  {
    return $this->_set_value("12", $value);
  }
  function get_availability_intervals_result()
  {
    return $this->_get_value("13");
  }
  function set_get_availability_intervals_result($value)
  {
    return $this->_set_value("13", $value);
  }
  function get_report_templates_result()
  {
    return $this->_get_value("14");
  }
  function set_get_report_templates_result($value)
  {
    return $this->_set_value("14", $value);
  }
}
class TZRequest extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZRequest"]["1"] = "PBString";
    $this->values["1"] = "";
    self::$fieldNames["TZRequest"]["1"] = "username";
    self::$fields["TZRequest"]["2"] = "PBString";
    $this->values["2"] = "";
    self::$fieldNames["TZRequest"]["2"] = "password";
    self::$fields["TZRequest"]["3"] = "TZCommand";
    $this->values["3"] = array();
    self::$fieldNames["TZRequest"]["3"] = "command";
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
  function set_all_commands($values)
  {
    return $this->_set_arr_values("3", $values);
  }
  function remove_last_command()
  {
    $this->_remove_last_arr_value("3");
  }
  function commands_size()
  {
    return $this->_get_arr_size("3");
  }
  function get_commands()
  {
    return $this->_get_value("3");
  }
}
class TZResponse extends PBMessage
{
  var $wired_type = PBMessage::WIRED_LENGTH_DELIMITED;
  public function __construct($reader=null)
  {
    parent::__construct($reader);
    self::$fields["TZResponse"]["1"] = "TZResult";
    $this->values["1"] = array();
    self::$fieldNames["TZResponse"]["1"] = "result";
    self::$fields["TZResponse"]["2"] = "PBInt";
    $this->values["2"] = "";
    self::$fieldNames["TZResponse"]["2"] = "timestamp";
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
  function set_all_results($values)
  {
    return $this->_set_arr_values("1", $values);
  }
  function remove_last_result()
  {
    $this->_remove_last_arr_value("1");
  }
  function results_size()
  {
    return $this->_get_arr_size("1");
  }
  function get_results()
  {
    return $this->_get_value("1");
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