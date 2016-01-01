<?php

/**
 * Status codes for synchronization
 */
class TZIntellitimeSyncStatus {
  const SYNC_OK = 0;
  const PENDING = 1;
  const NETWORK_FAILURE = 2;
  const AUTH_FAILURE = 3;
  const LOCKED = 4;
  const POSTPONED = 5;
}
