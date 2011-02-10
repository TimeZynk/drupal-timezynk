<?php

class TZUserFilter {
  private $policy;
  private $users;

  /**
   * Make a new User filter
   * @param string $policy 'include' or 'exclude'
   * @param array $users array of user IDs
   */
  function __construct($policy, $users) {
    $this->policy = $policy;
    $this->users = $users;
  }

  function isIncluded($user) {
    if ($this->policy === 'include') {
      return FALSE !== array_search($user, $this->users);
    }
    return FALSE === array_search($user, $this->users);
  }
}