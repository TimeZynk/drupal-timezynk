<?php

/**
 * Wraps a query string and its associated arguments.
 */
class TZQuery {

  private $queryString;
  private $queryArgs;


  public function setQueryString($queryString) {
    $this->queryString = $queryString;
  }

  public function setQueryArgs($queryArgs) {
    $this->queryArgs = $queryArgs;
  }

  public function getQueryString() {
    return $this->queryString;
  }

  public function getQueryArgs() {
    return $this->queryArgs;
  }
}