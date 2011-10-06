<?php

abstract class CommandHandler {
  protected $protocol_version;

  public function __construct($protocol_version) {
    $this->protocol_version = $protocol_version;
  }

  /**
   * Handle the designated command
   * @param TZCommand $command command to be handled
   * @param TZResult $result results after handling should be written here.
   * @throws TZProtoException if handling fails
   */
  public abstract function handle($command, &$result);
}
