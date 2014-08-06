<?php

class IntellitimeAvailabilityAddPost extends IntellitimePost {
  private $epoch2000_01_01_1200 = '946724400';

  public function __construct($server, $form, $availability) {
    if (empty($availability)) {
      throw new InvalidArgumentException('Missing availability');
    }
    parent::__construct($server, $form);
    $this->availability = $availability;
  }

  protected function getPostData() {
    $data = $this->form->getFormValues();
    // Unset Save-button since we are "clicking" on one of the date links instead
    unset($data['m_availabilityItems$SaveButton']);
    foreach($data as $key => $value) {
      if (FALSE !== strpos($key, 'ImageButton')) {
        unset($data[$key]);
      }
    }

    $data['__EVENTTARGET'] = 'm_availabilityCalendar$ThisMonth';
    $date = $this->availability->getDate();
    $date->setTime(12,0);
    $date->modify('-' . $this->epoch2000_01_01_1200 . ' seconds');
    $data['__EVENTARGUMENT'] =  round(intval($date->format('U'), 10)  / 86400);
    return $data;
  }

  protected function getAction() {
    return 'Availability/' . parent::getAction();
  }

  protected function createPage($html_string, $server) {
    return new IntellitimeAvailabilityPage($html_string, $server);
  }
}
