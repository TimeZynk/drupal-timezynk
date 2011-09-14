<?php

class IntellitimeAvailabilityMergeStrategyTest extends PHPUnit_Framework_TestCase {
  private $ipAvailabilities;
  private $localAvailabilities;
  private $merge;

  function setUp() {
    $this->ipAvailabilities = array(
      '2011-06-23' => new IntellitimeAvailability(date_make_date('2011-06-23'), 'abc_form_id'),
    );
    $this->ipAvailabilities['2011-06-23']->setDay(TRUE);

    $this->localAvailabilities = array(
      new Availability(array(
        'id' => 10000,
        'uid' => 123,
        'availability_type' => Availability::AVAILABLE,
        'start_time' => 1308780000, // Wed, 23 Jun 2011 00:00:00 GMT+2
        'end_time'   => 1308866399, //  Wed, 23 Jun 2011 23:59:59 GMT+2
      )),
    );

    $this->store = new AvailabilityStore(NULL);
    $factory = new IntellitimeAvailabilityFactory($this->store, "08:00-12:00", "12:00-18:00", "18:00-24:00");
    $this->merge = new IntellitimeAvailabilityMergeStrategy($factory);
  }

  public function testToIntellitimeOverwritesLocalIfNoLocalChanges() {
    $merged = $this->merge->mergeToIntellitimeAvailabilities($this->localAvailabilities, $this->ipAvailabilities);
    $this->assertEquals(1, count($merged));
    $this->assertTrue($merged['2011-06-23']->isAvailableDuringDay());
    $this->assertFalse($merged['2011-06-23']->isAvailableDuringEvening());
    $this->assertFalse($merged['2011-06-23']->isAvailableDuringNight());
  }

  public function testToIntellitimeOverwritesIPIfLocalChanges() {
    $this->localAvailabilities[0]->setLocalChanges();
    $merged = $this->merge->mergeToIntellitimeAvailabilities($this->localAvailabilities, $this->ipAvailabilities);
    $this->assertEquals(1, count($merged));
    $this->assertTrue($merged['2011-06-23']->isAvailableDuringDay());
    $this->assertTrue($merged['2011-06-23']->isAvailableDuringEvening());
    $this->assertTrue($merged['2011-06-23']->isAvailableDuringNight());
    $this->assertTrue($merged['2011-06-23']->haveLocalChanges());
  }

  public function testToIntellitimeAddsIfNewLocal() {
    $this->localAvailabilities[] = new Availability(array(
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308895200, // Wed, 24 Jun 2011 08:00:00 GMT+2
      'end_time'   => 1308909600, // Wed, 24 Jun 2011 12:00:00 GMT+2
      'local_changes' => 1,
    ));
    $merged = $this->merge->mergeToIntellitimeAvailabilities($this->localAvailabilities, $this->ipAvailabilities);
    $this->assertEquals(array('2011-06-23', '2011-06-24'), array_keys($merged));
    // original unmodified
    $this->assertTrue($merged['2011-06-23']->isAvailableDuringDay());
    $this->assertFalse($merged['2011-06-23']->isAvailableDuringEvening());
    $this->assertFalse($merged['2011-06-23']->isAvailableDuringNight());
    $this->assertFalse($merged['2011-06-23']->haveLocalChanges());
    // new row added
    $this->assertTrue($merged['2011-06-24']->isAvailableDuringDay());
    $this->assertFalse($merged['2011-06-24']->isAvailableDuringEvening());
    $this->assertFalse($merged['2011-06-24']->isAvailableDuringNight());
    $this->assertTrue($merged['2011-06-24']->haveLocalChanges());
  }

  public function testToIntellitimeRemovesLocalIfRemovedOnServer() {
    $this->localAvailabilities[] = new Availability(array(
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308895200, // Wed, 24 Jun 2011 08:00:00 GMT+2
      'end_time'   => 1308909600, // Wed, 24 Jun 2011 12:00:00 GMT+2
      'local_changes' => 0,
    ));
    $merged = $this->merge->mergeToIntellitimeAvailabilities($this->localAvailabilities, $this->ipAvailabilities);
    $this->assertEquals(array('2011-06-23'), array_keys($merged));
  }

  public function testToIntellitimeRemovesRemoteIfRemovedLocally() {
    $this->localAvailabilities[] = new Availability(array(
      'uid' => 123,
      'availability_type' => Availability::DELETED,
      'start_time' => 1308808800, // Wed, 23 Jun 2011 08:00:00 GMT+2
      'end_time'   => 1308823200, // Wed, 23 Jun 2011 12:00:00 GMT+2
      'local_changes' => 1,
    ));
    $merged = $this->merge->mergeToIntellitimeAvailabilities($this->localAvailabilities, $this->ipAvailabilities);
    // original unmodified
    $this->assertFalse($merged['2011-06-23']->isAvailableDuringDay());
    $this->assertFalse($merged['2011-06-23']->isAvailableDuringEvening());
    $this->assertFalse($merged['2011-06-23']->isAvailableDuringNight());
    $this->assertTrue($merged['2011-06-23']->haveLocalChanges());
  }

  public function testToIntellitimeMarksCheckboxIfSelectionIncreased() {
    $this->ipAvailabilities['2011-06-23']->setNight(TRUE);
    $this->localAvailabilities[] = new Availability(array(
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308808800, // Wed, 23 Jun 2011 08:00:00 GMT+2
      'end_time'   => 1308823200, // Wed, 23 Jun 2011 12:00:00 GMT+2
      'local_changes' => 0,
    ));
    $merged = $this->merge->mergeToIntellitimeAvailabilities($this->localAvailabilities, $this->ipAvailabilities);
    $this->assertTrue($merged['2011-06-23']->isAvailableDuringDay());
    $this->assertFalse($merged['2011-06-23']->isAvailableDuringEvening());
    $this->assertTrue($merged['2011-06-23']->isAvailableDuringNight());
    $this->assertFalse($merged['2011-06-23']->haveLocalChanges());
  }

  public function testToIntellitimeMergesTwoLocalOnSameDay() {
    $this->localAvailabilities[] = new Availability(array(
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308895200, // Wed, 24 Jun 2011 08:00:00 GMT+2
      'end_time'   => 1308909600, // Wed, 24 Jun 2011 12:00:00 GMT+2
      'local_changes' => 1,
    ));
    $this->localAvailabilities[] = new Availability(array(
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308909600, // Wed, 24 Jun 2011 12:00:00 GMT+2
      'end_time'   => 1308931200, // Wed, 24 Jun 2011 18:00:00 GMT+2
      'local_changes' => 1,
    ));
    $merged = $this->merge->mergeToIntellitimeAvailabilities($this->localAvailabilities, $this->ipAvailabilities);
    $this->assertEquals(array('2011-06-23', '2011-06-24'), array_keys($merged));
    $this->assertTrue($merged['2011-06-24']->isAvailableDuringDay());
    $this->assertTrue($merged['2011-06-24']->isAvailableDuringEvening());
    $this->assertFalse($merged['2011-06-24']->isAvailableDuringNight());
    $this->assertTrue($merged['2011-06-24']->haveLocalChanges());
  }

  public function testToIntellitimeMergesDeletedAndAddedOnSameDay() {
    $this->localAvailabilities[] = new Availability(array(
      'uid' => 123,
      'availability_type' => Availability::DELETED,
      'start_time' => 1308808800, // Wed, 23 Jun 2011 08:00:00 GMT+2
      'end_time'   => 1308823200, // Wed, 23 Jun 2011 12:00:00 GMT+2
      'local_changes' => 1,
    ));
    $this->localAvailabilities[] = new Availability(array(
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308823200, // Wed, 23 Jun 2011 12:00:00 GMT+2
      'end_time'   => 1308844800, // Wed, 23 Jun 2011 18:00:00 GMT+2
      'local_changes' => 1,
    ));
    $merged = $this->merge->mergeToIntellitimeAvailabilities($this->localAvailabilities, $this->ipAvailabilities);
    $this->assertEquals(array('2011-06-23'), array_keys($merged));
    $this->assertFalse($merged['2011-06-23']->isAvailableDuringDay());
    $this->assertTrue($merged['2011-06-23']->isAvailableDuringEvening());
    $this->assertFalse($merged['2011-06-23']->isAvailableDuringNight());
    $this->assertTrue($merged['2011-06-23']->haveLocalChanges());
  }

  public function testToAvailabilityAddsNewRowsToLocal() {
    $this->localAvailabilities = array();
    $merged = $this->merge->mergeToAvailabilities($this->localAvailabilities, $this->ipAvailabilities);
    $this->assertEquals(1, count($merged));
    $this->assertNull($merged[0]->getId());
    $this->assertEquals('2011-06-23 08:00', $merged[0]->getStartTime()->format('Y-m-d H:i'));
    $this->assertEquals('2011-06-23 12:00', $merged[0]->getEndTime()->format('Y-m-d H:i'));
  }

  public function testToAvailabilityRemovesDeletedRowsFromLocal() {
    $this->ipAvailabilities = array();
    $this->localAvailabilities = array(new Availability(array(
      'id' => 10001,
      'uid' => 123,
      'availability_type' => Availability::AVAILABLE,
      'start_time' => 1308895200, // Wed, 24 Jun 2011 08:00:00 GMT+2
      'end_time'   => 1308909600, // Wed, 24 Jun 2011 12:00:00 GMT+2
      'local_changes' => 0,
    )));
    $merged = $this->merge->mergeToAvailabilities($this->localAvailabilities, $this->ipAvailabilities);
    $this->assertEquals(1, count($merged));
    $this->assertEquals(10001, $merged[0]->getId());
    $this->assertEquals(Availability::DELETED, $merged[0]->getType());
  }

  public function testToAvailabilityKeepsMatchingAvailabilities() {
    $merged = $this->merge->mergeToAvailabilities($this->localAvailabilities, $this->ipAvailabilities);
    $this->assertEquals(1, count($merged));
    $this->assertEquals(10000, $merged[0]->getId());
    $this->assertEquals('2011-06-23 08:00', $merged[0]->getStartTime()->format('Y-m-d H:i'));
    $this->assertEquals('2011-06-23 12:00', $merged[0]->getEndTime()->format('Y-m-d H:i'));
  }

  public function testToAvailabilityRemovesExtraAvailabilitiesFromDay() {
     $this->localAvailabilities = array(
       new Availability(array(
        'id' => 10001,
        'uid' => 123,
        'availability_type' => Availability::AVAILABLE,
        'start_time' => 1308808801, // Wed, 23 Jun 2011 08:00:01 GMT+2
        'end_time'   => 1308823201, // Wed, 23 Jun 2011 12:00:01 GMT+2
        'local_changes' => 0,
      )),
      new Availability(array(
        'id' => 10002,
        'uid' => 123,
        'availability_type' => Availability::AVAILABLE,
        'start_time' => 1308808801, // Wed, 23 Jun 2011 08:00:01 GMT+2
        'end_time'   => 1308823201, // Wed, 23 Jun 2011 12:00:01 GMT+2
        'local_changes' => 1,
      ))
    );
    $merged = $this->merge->mergeToAvailabilities($this->localAvailabilities, $this->ipAvailabilities);
    $this->assertEquals(2, count($merged));
    $this->assertEquals(10001, $merged[0]->getId());
    $this->assertEquals('2011-06-23 08:00', $merged[0]->getStartTime()->format('Y-m-d H:i'));
    $this->assertEquals('2011-06-23 12:00', $merged[0]->getEndTime()->format('Y-m-d H:i'));
    $this->assertEquals(10002, $merged[1]->getId());
    $this->assertEquals(Availability::DELETED, $merged[1]->getType());
  }

  public function testToAvailabilityNewCheckboxOnExistingDayAddsNewRow() {
    $this->ipAvailabilities['2011-06-23']->setNight(TRUE);
    $merged = $this->merge->mergeToAvailabilities($this->localAvailabilities, $this->ipAvailabilities);
    $this->assertEquals(2, count($merged));
    $this->assertEquals(10000, $merged[0]->getId());
    $this->assertEquals('2011-06-23 08:00', $merged[0]->getStartTime()->format('Y-m-d H:i'));
    $this->assertEquals('2011-06-23 12:00', $merged[0]->getEndTime()->format('Y-m-d H:i'));
    $this->assertEquals(NULL, $merged[1]->getId());
    $this->assertEquals('2011-06-23 18:00', $merged[1]->getStartTime()->format('Y-m-d H:i'));
    $this->assertEquals('2011-06-24 00:00', $merged[1]->getEndTime()->format('Y-m-d H:i'));
  }

  public function testToAvailabilityConsecutiveCheckboxKeepsMatchingAvailabilities() {
    $this->ipAvailabilities['2011-06-23']->setEvening(TRUE)->setNight(TRUE);
    $merged = $this->merge->mergeToAvailabilities($this->localAvailabilities, $this->ipAvailabilities);
    $this->assertEquals(1, count($merged));
    $this->assertEquals(10000, $merged[0]->getId());
    $this->assertEquals('2011-06-23 08:00', $merged[0]->getStartTime()->format('Y-m-d H:i'));
    $this->assertEquals('2011-06-24 00:00', $merged[0]->getEndTime()->format('Y-m-d H:i'));
  }

}