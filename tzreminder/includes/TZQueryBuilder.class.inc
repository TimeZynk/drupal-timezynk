<?php

/**
 * Builds TZQuery objects.
 */
interface TZQueryBuilder {
  /**
   * @return TZQuery
   */
  public function build();
}

class TZReportQueryBuilder implements TZQueryBuilder {
  /**
   * @var TZQuery
   */
  private $query;
  /**
   *
   * @var Array
   */
  private $queryArgumentMap = array();
  /**
   * Creates a new query builder that returns the latest revision of all
   * reports with tzreport->flags < $flags.
   * @param TZFlags $flags 
   */
  public function __construct($flags) {
    $this->query = new TZQuery();
    $this->queryArgumentMap['flags'] = $flags;
  }

  /**
   * Only return reports with endtime < the supplied $date.
   * @param DateTime $date filter out reports with endtime >= $date
   */
  public function setEndtimeBefore($date) {
    if (empty($date)) {
      throw new InvalidArgumentException("No datetime suppplied");
    }
    $this->queryArgumentMap['endtime'] = $date->format('U');
  }
  
  public function build() {
    $queryString = 'SELECT * FROM {node} n INNER JOIN {tzreport} t ON n.vid = t.vid WHERE t.flags < %d';
    $queryArgs = array ($this->queryArgumentMap['flags']);
    if (isset($this->queryArgumentMap['endtime'])) {
      $queryString .= ' AND t.endtime < %d';
      $queryArgs[] = $this->queryArgumentMap['endtime'];
    }
    $this->query->setQueryString($queryString);
    $this->query->setQueryArgs($queryArgs);
    return $this->query;
  }

}