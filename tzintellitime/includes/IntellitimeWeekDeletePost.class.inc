<?php

class IntellitimeWeekDeletePost extends IntellitimeWeekPost {
  protected function createPage($html_string, $server) {
    return new IntellitimeWeekPageDeletedFinal($html_string, $server);
  }

  protected function getPostData() {
    $post = $this->form->getFormValues();

    foreach($this->itreports as $itreport) {
      if ($itreport->state == TZIntellitimeReport::STATE_DELETED) {
        $this->set_report_state($post, $itreport);
      }
    }

    if(empty($post['UpdateButton'])) {
      throw new TZIntellitimeInconsistentPost('Update button not found', $this->doc->saveXML());
    }
    unset($post['DoneButton']);
    unset($post['ChangeButton']);

    return $post;
  }
}

class IntellitimeWeekPageDeletedFinal extends IntellitimeWeekPage {
  public function getDeletePost(array $itreports) {
    return NULL;
  }
  public function getUnlockPost(array $itreports, $unlockImmutable) {
    return NULL;
  }
}
