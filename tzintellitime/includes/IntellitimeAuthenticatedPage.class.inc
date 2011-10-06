<?php

class TZAuthenticationFailureException extends TZIntellitimeServerException {}

class IntellitimeAuthenticatedPage extends IntellitimePage {
  function validate() {
    parent::validate();
    $this->validateLogin();
  }

  function validateLogin() {
    /* Look for the LogOut link as proof that the login succeeded */
    $list = $this->doc->xpath('//img[@src="../Images/Img_LogOut.gif"]');
    if (count($list) === 0) {
      throw new TZAuthenticationFailureException('Invalid username or password', (string)$this);
    }
  }
}