<?php

abstract class IntellitimeWeekPost extends IntellitimePost {
  protected $itreports = array();
  protected $parser;

  public function setParser($parser) {
    $this->parser = $parser;
    return $this;
  }

  public function setReports($itreports) {
    $this->itreports = $itreports;
    return $this;
  }

  protected function createPage($html_string, $server) {
    return new IntellitimeWeekPage($html_string, $server);
  }

  protected function getActionPrefix() {
    return 'TimeReport/';
  }

  protected function set_key_if_exists(&$post, $key, $value) {
    if (isset($post[$key])) {
      $post[$key] = $value;
    }
  }

  protected function set_report_state(&$post, $report) {
    $control_prefix = $this->parser->find_report_control_prefix($report);

    switch ($report->state) {
      case TZIntellitimeReport::STATE_OPEN:
        unset($post[$control_prefix . "CheckboxDayDone"]);
        unset($post[$control_prefix . "CheckBoxDelete"]);
        break;

      case TZIntellitimeReport::STATE_REPORTED:
        $control_name = $control_prefix . "CheckboxDayDone";
        if ($this->parser->name_exists($control_name)) {
          $post[$control_name] = "on";
        }
        unset($post[$control_prefix . "CheckBoxDelete"]);
        break;

      case TZIntellitimeReport::STATE_DELETED:
        $post[$control_prefix . "CheckBoxDelete"] = "on";
        break;

      default:
        break;
    }
  }
}