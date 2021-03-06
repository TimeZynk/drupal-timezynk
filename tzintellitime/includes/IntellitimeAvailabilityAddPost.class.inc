<?php

class IntellitimeAvailabilityAddPost extends IntellitimePost {
  private $epoch2000_01_01_1200 = '946724400';

  public function __construct($server, $form, $availability) {
    if (empty($availability)) {
      throw new InvalidArgumentException('Missing availability');
    }
    parent::__construct($server, $form);
    $this->availability = $availability;
    // watchdog('tzdebug', 'Creating Add Post');
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

    $date = $this->availability->getDate();
    $date->setTime(12,0);
    $date->modify('-' . $this->epoch2000_01_01_1200 . ' seconds');
    $currentDate = new DateTime();
    $currentMonth = $currentDate->format('m');
    if ($date->format('m') == $currentMonth) {
      $data['__EVENTTARGET'] = 'm_availabilityCalendar$ThisMonth';
    } else {
      $data['__EVENTTARGET'] = 'm_availabilityCalendar$NextMonth';
    }
    $data['__EVENTARGUMENT'] =  round(intval($date->format('U'), 10)  / 86400);
    // watchdog('tzdebug_data', 'Add Post Data: ' . print_r($data, TRUE));
    return $data;
  }

  protected function getAction() {
    return 'Availability/' . parent::getAction();
  }

  protected function createPage($html_string, $server) {
    return new IntellitimeAvailabilityPage($html_string, $server);
  }
}
