<?php

/**
 * Script meant for fast testing of intellitime login code. It should be run with "drush scr login.php".
 * It boots drupal, inserts our intellitime module, and tries to login to intellitime using that module.
 */

require_once(dirname(__FILE__) . '/../tzintellitime.module');
require_once(dirname(__FILE__) . '/../tzintellitime_sync/tzintellitime_sync.module');

tzintellitime_include_classes();

$jobmap = array(
  'Axis Commun, Lagerarbeta, Q6032, spec' => 1,
  'Axis Communic, Lagerarbeta, "Heating"' => 2,
  'Axis Communicatio, Lagerarbeta, P5532' => 3,
  'Lagerarbetare, TimeZynk' => 4,
);

$titles = array(
  'Axis Communication AB, Lagerarbetare, Q6032, spec. prod.',
  'Axis Communication AB, Lagerarbetare, "Heating"',
  'Axis Communicatio, Lagerarbeta, P5534',
  'Lagerarbetare, TimeZynk',
);

foreach($titles as $title) {
  print $title . ": " . TZIntellitimeSyncController::match_job($jobmap, $title) . "\n";
}