<?php

class IntellitimeWeekUnlockPost extends IntellitimeWeekPost {
  private $unlockImmutable = FALSE;

  public function setUnlockImmutable($unlockImmutable) {
    $this->unlockImmutable = $unlockImmutable;
    return $this;
  }

  public function getUnlockImmutable() {
    return $this->unlockImmutable;
  }

  protected function getPostData() {
    $post = $this->form->getFormValues();

    if (isset($post['ChangeButton'])) {
      /* If we find the ChangeButton then all reports are marked as
       * Reported or higher. Unmark all that can be unmarked so that the
       * next POST can change the appropriate values */
      unset($post['DoneButton']);
      unset($post['UpdateButton']);
    } else if ($this->unlockImmutable) {
      /* If we have immutable reports on a partially locked week we have to
       * first lock the week to then be able to unlock it.
       */
      unset($post['ChangeButton']);
      unset($post['UpdateButton']);
    } else {
      // Report states are possible to manipulate, set up the state checkboxes.
      foreach($this->itreports as $itreport) {
        $itreport_clone = clone($itreport);
        $itreport_clone->state = TZIntellitimeReport::STATE_OPEN;
        $this->set_report_state($post, $itreport_clone);
      }
      unset($post['DoneButton']);
    }

    return $post;
  }
}