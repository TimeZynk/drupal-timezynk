<?php
abstract class TZAbstractReminderPolicy implements TZReminderPolicy {
  public function getMessage($numberOfReports) {
    if (empty($numberOfReports)) {
      throw new InvalidArgumentException("Missing arguments to getMessage");
    }
    return
      t('Hi! We are waiting for @count of your time reports. Please fill !third-person-personal-pronoun in!',
        array(
          '@count' => format_plural($numberOfReports, 'one', '@count'),
          '!third-person-personal-pronoun' => format_plural($numberOfReports, 'it', 'them'),
        )
      );
  }
}