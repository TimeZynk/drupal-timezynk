<?php

interface CommandHandler {
  /**
   * Handle the designated command
   * @param TZCommand $command command to be handled
   * @param TZResult $result results after handling should be written here.
   * @throws TZProtoException if handling fails
   */
  public function handle($command, &$result);
}