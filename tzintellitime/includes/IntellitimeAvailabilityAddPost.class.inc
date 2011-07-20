<?php

class IntellitimeAvailabilityAddPost extends IntellitimePost {
  private $epoch2000_01_01_1200 = '946724400';

  public function __construct($bot, $form, $availability) {
    if (empty($availability)) {
      throw new InvalidArgumentException('Missing availability');
    }
    parent::__construct($bot, $form);
    $this->availability = $availability;
  }

  protected function getPostData() {
    $data = $this->form->getFormValues();
    $data['__EVENTTARGET'] = 'm_availabilityCalendar:ThisMonth';
    $date = $this->availability->getDate();
    $date->setTime(12,0);
    $date->modify('-' . $this->epoch2000_01_01_1200 . ' seconds');
    $data['__EVENTARGUMENT'] =  round(intval($date->format('U'), 10)  / 86400);
    return $data;
  }

  protected function createPage($html_string, $bot) {
    return new IntellitimeAvailabilityPage($html_string, $bot);
  }
}
