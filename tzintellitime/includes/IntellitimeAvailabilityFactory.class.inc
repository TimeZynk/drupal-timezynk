<?php
class IntellitimeAvailabilityFactory {
  /**
   * @var Array
   */
  private $day_range;
  /**
   * @var Array
   */
  private $evening_range;
    /**
   * @var Array
   */
  private $night_range;

  public function __construct($day_range, $evening_range, $night_range) {
    $this->day_range = $day_range;
    $this->evening_range = $evening_range;
    $this->night_range = $night_range;
  }

  /**
   * Creates an IntellitimeAvailability from a regular Availability.
   * @param Availability $availability
   * @return IntellitimeAvailability the created availability.
   */
  public function create($availability) {
    $ia = new IntellitimeAvailability($availability->getStartTime());
    if ($availability->isAvailable()) {
      $ia->setDay($availability->isOverlappingRange($this->day_range['start'], $this->day_range['end']));
      $ia->setEvening($availability->isOverlappingRange($this->evening_range['start'], $this->evening_range['end']));
      $ia->setNight($availability->isOverlappingRange($this->night_range['start'], $this->night_range['end']));
    }
    return $ia;
  }
}