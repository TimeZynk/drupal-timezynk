<?php

/**
 * Description of TZPostData
 *
 */
class TZIntellitimePostData {
  private $postAction;
  private $postData;
  private $intermediate = FALSE;

  function setPostAction($postAction) {
    $this->postAction = $postAction;
  }
  function getPostAction() {
    return $this->postAction;
  }
  function setPostData($postData) {
    $this->postData = $postData;
  }
  function getPostData() {
    return $this->postData;
  }
  function isIntermediate() {
    return $this->intermediate;
  }
  function setIntermediate($intermediate) {
    $this->intermediate = $intermediate ? TRUE : FALSE;
  }
}
