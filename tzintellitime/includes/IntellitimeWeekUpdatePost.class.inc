<?php

class IntellitimeWeekUpdatePost extends IntellitimeWeekPost {
  private $allReportsDone = FALSE;

  public function setAllReportsDone($allReportsDone) {
    $this->allReportsDone = $allReportsDone;
    return $this;
  }

  public function getAllReportsDone() {
    return $this->allReportsDone;
  }

  protected function createPage($html_string, $server) {
    return new IntellitimeWeekPageUpdatedFinal($html_string, $server);
  }

  protected function getPostData() {
    $post = $this->form->getFormValues();

    foreach($this->itreports as $itreport) {
      $this->set_report_fields($post, $itreport);
      $this->set_report_state($post, $itreport);
    }

    if($this->allReportsDone) {
      if(empty($post['DoneButton'])) {
        throw new TZIntellitimeInconsistentPost('Done button not found: ' . print_r($post, TRUE), strval($this->parser));
      }
      unset($post['UpdateButton']);
      unset($post['ChangeButton']);
    } else {
      if(empty($post['UpdateButton'])) {
        throw new TZIntellitimeInconsistentPost('Update button not found', strval($this->parser));
      }
      unset($post['DoneButton']);
      unset($post['ChangeButton']);
    }

    return $post;
  }

  /**
   * @param TZIntellitimeReport $report
   */
  private function set_report_fields(&$post, $report) {
    $control_prefix = $this->parser->find_report_control_prefix($report);

    $fields_map = array(
      "TextboxTimeFrom" => $report->begin,
      "TextboxTimeTo" => $report->end,
      "TextboxBreak" => $report->break_duration_minutes,
      "TextboxExplicitOvertime" => $report->overtime_hours,
      "TextboxNote" => $report->comment,
    );

    foreach($fields_map as $key => $value) {
      $this->set_key_if_exists($post, $control_prefix . $key, $value);
    }
  }
}

class IntellitimeWeekPageUpdatedFinal extends IntellitimeWeekPage {
  public function getUpdatePost(array $itreports, $allReportsDone) {
    return NULL;
  }

  public function getUnlockPost(array $itreports, $unlockImmutable) {
    return NULL;
  }
}
