<?php
function __tzintellitime_autoload($class_name) {
  switch ($class_name) {
    case 'IntellitimeAvailability':
      require dirname(__FILE__) . '/includes/IntellitimeAvailability.class.inc';
      return TRUE;
    case 'IntellitimeAvailabilityAddPost':
      require dirname(__FILE__) . '/includes/IntellitimeAvailabilityAddPost.class.inc';
      return TRUE;
    case 'IntellitimeAvailabilityFactory':
      require dirname(__FILE__) . '/includes/IntellitimeAvailabilityFactory.class.inc';
      return TRUE;
    case 'IntellitimeAvailabilityMergeStrategy':
      require dirname(__FILE__) . '/includes/IntellitimeAvailabilityMergeStrategy.class.inc';
      return TRUE;
    case 'IntellitimeAvailabilityPage':
    case 'IntellitimeAvailabilityFinalPage':
      require dirname(__FILE__) . '/includes/IntellitimeAvailabilityPage.class.inc';
      return TRUE;
    case 'IntellitimeAvailabilitySyncController':
      require dirname(__FILE__) . '/includes/IntellitimeAvailabilitySyncController.class.inc';
      return TRUE;
    case 'IntellitimeAvailabilityUpdatePost':
      require dirname(__FILE__) . '/includes/IntellitimeAvailabilityUpdatePost.class.inc';
      return TRUE;
    case 'IntellitimeForm':
      require dirname(__FILE__) . '/includes/IntellitimeForm.class.inc';
      return TRUE;
    case 'TZIntellitimeErrorPageException':
    case 'IntellitimePage':
      require dirname(__FILE__) . '/includes/IntellitimePage.class.inc';
      return TRUE;
    case 'IntellitimePost':
      require dirname(__FILE__) . '/includes/IntellitimePost.class.inc';
      return TRUE;
    case 'IntellitimeSynchronizationDriver':
      require dirname(__FILE__) . '/includes/IntellitimeSynchronizationDriver.class.inc';
      return TRUE;
    case 'TZCompositeLogger':
      require dirname(__FILE__) . '/includes/TZCompositeLogger.class.inc';
      return TRUE;
    case 'TZIntellitimeServerException':
    case 'TZNetworkFailureException':
    case 'TZCurl':
      require dirname(__FILE__) . '/includes/TZCurl.class.inc';
      return TRUE;
    case 'TZIntellitimeAssignment':
      require dirname(__FILE__) . '/includes/TZIntellitimeAssignment.class.inc';
      return TRUE;
    case 'TZIntellitimeBot':
      require dirname(__FILE__) . '/includes/TZIntellitimeBot.class.inc';
      return TRUE;
    case 'TZIntellitimeDBReportStorage':
      require dirname(__FILE__) . '/includes/TZIntellitimeDBReportStorage.class.inc';
      return TRUE;
    case 'TZIntellitimeDefaultWeekFactory':
      require dirname(__FILE__) . '/includes/TZIntellitimeDefaultWeekFactory.class.inc';
      return TRUE;
    case 'TZIntellitimeLogger':
      require dirname(__FILE__) . '/includes/TZIntellitimeLogger.class.inc';
      return TRUE;
    case 'TZIntellitimeReportRowNotFound':
    case 'TZIntellitimeInconsistentPost':
    case 'TZIntellitimeParser':
      require dirname(__FILE__) . '/includes/TZIntellitimeParser.class.inc';
      return TRUE;
    case 'TZIntellitimePostData':
      require dirname(__FILE__) . '/includes/TZIntellitimePostData.class.inc';
      return TRUE;
    case 'TZIntellitimeReport':
      require dirname(__FILE__) . '/includes/TZIntellitimeReport.class.inc';
      return TRUE;
    case 'TZIntellitimeReportStorage':
      require dirname(__FILE__) . '/includes/TZIntellitimeReportStorage.class.inc';
      return TRUE;
    case 'TZIntellitimeServerInterface':
      require dirname(__FILE__) . '/includes/TZIntellitimeServerInterface.class.inc';
      return TRUE;
    case 'TZIntellitimeSimpleSyncPolicy':
      require dirname(__FILE__) . '/includes/TZIntellitimeSimpleSyncPolicy.class.inc';
      return TRUE;
    case 'TZIntellitimeSyncController':
      require dirname(__FILE__) . '/includes/TZIntellitimeSyncController.class.inc';
      return TRUE;
    case 'TZIntellitimeSyncPolicy':
      require dirname(__FILE__) . '/includes/TZIntellitimeSyncPolicy.class.inc';
      return TRUE;
    case 'TZIntellitimeUserJobsMappingPolicy':
      require dirname(__FILE__) . '/includes/TZIntellitimeUserJobsMappingPolicy.class.inc';
      return TRUE;
    case 'TZIntellitimeUserStatus':
      require dirname(__FILE__) . '/includes/TZIntellitimeUserStatus.class.inc';
      return TRUE;
    case 'TZAuthenticationFailureException':
    case 'TZIntellitimeWeek':
      require dirname(__FILE__) . '/includes/TZIntellitimeWeek.class.inc';
      return TRUE;
    case 'TZIntellitimeWeekData':
      require dirname(__FILE__) . '/includes/TZIntellitimeWeekData.class.inc';
      return TRUE;
    case 'TZIntellitimeWeekFactory':
      require dirname(__FILE__) . '/includes/TZIntellitimeWeekFactory.class.inc';
      return TRUE;
    case 'TZIntellitimeWeekSyncResult':
      require dirname(__FILE__) . '/includes/TZIntellitimeWeekSyncResult.class.inc';
      return TRUE;
    case 'TZSmsLogger':
      require dirname(__FILE__) . '/includes/TZSmsLogger.class.inc';
      return TRUE;
    case 'TZWatchdogLogger':
      require dirname(__FILE__) . '/includes/TZWatchdogLogger.class.inc';
      return TRUE;
    default:
      return FALSE;
  }
}
spl_autoload_register('__tzintellitime_autoload', TRUE, TRUE);
