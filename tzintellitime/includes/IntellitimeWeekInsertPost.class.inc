<?php

class IntellitimeWeekInsertPost extends IntellitimeWeekPost {
  /**
   * @var TZIntellitimeReport
   */
  private $itreport;

  public function setReport($nid, $itreport) {
    $this->nid = $nid;
    $this->itreport = $itreport;
    return $this;
  }

  public function getNid() {
    return $this->nid;
  }

  protected function getPostData() {
    if (empty($this->itreport->jobid)) {
      throw new InvalidArgumentException("Missing jobid in TZIntellitimeReport");
    }

    $post = $this->form->getFormValues();
    $this->set_key_if_exists($post, 'AddDateDropDown', $this->itreport->get_date_string());
    $this->set_key_if_exists($post, 'AddRowDropDown', (string)$this->itreport->jobid);
    $this->set_key_if_exists($post, 'AddTimeFromTextBox', $this->itreport->begin);
    $this->set_key_if_exists($post, 'AddTimeToTextBox', $this->itreport->end);
    $this->set_key_if_exists($post, 'AddBreakTextBox', (string)$this->itreport->break_duration_minutes);
    $this->set_key_if_exists($post, 'AddNoteTextBox', (string)$this->itreport->comment);

    // Always use the UpdateButton when adding rows
    unset($post['ChangeButton']);
    unset($post['DoneButton']);
    return $post;
  }
}