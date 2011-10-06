<?php

class IntellitimeLoginPage extends IntellitimePage {
  /**
   * Get login post for given username and password
   * @param string $username
   * @param string $password
   */
  public function getPost($username, $password) {
    $post = new IntellitimeLoginPost($this->server, $this->getForm());
    return $post->setCredentials($username, $password);
  }

  /**
   * Logs in to the intellitime web service at $login_url
   * @param IntellitimeServer $server
   * @param string $login_url
   * @param string $username the user to login as
   * @param string $password the password to login with
   * @return string Users full name
   * @throws TZAuthenticationFailureException if login fails
   */
  public static function login($server, $login_url, $username, $password) {
    $page = new IntellitimeLoginPage($server->get($login_url), $server);
    $post = $page->getPost($username, $password);
    $main_page = $post->post();
    $fullname = $main_page->getFullName();
    if(empty($fullname)) {
      return $username;
    } else {
      return $fullname;
    }
  }
}

class IntellitimeMainPage extends IntellitimeAuthenticatedPage {
  public function getFullName() {
    $username = NULL;
    $spans = $this->doc->xpath('//td/font/span');
    foreach($spans as $span) {
      if(preg_match('/LabeUserName$/', $span['id'])) {
        $username = trim(strval($span));
        break;
      }
    }
    return $username;
  }
}

class IntellitimeLoginPost extends IntellitimePost {
  private $username = '';
  private $password = '';

  public function setCredentials($username, $password) {
    $this->username = $username;
    $this->password = $password;
    return $this;
  }

  protected function getPostData() {
    $data = $this->form->getFormValues();
    $data['TextBoxUserName'] = $this->username;
    $data['TextBoxPassword'] = $this->password;
    return $data;
  }

  protected function getAction() {
    return 'Portal/' . parent::getAction();
  }

  protected function createPage($html_string, $server) {
    return new IntellitimeMainPage($html_string, $server);
  }
}