<?php
/**
 * @file
 * Handler for printing unix timestamps with the help of the date_api module
 */

/**
 * Views handler for unix timestamps
 */
class tzbase_handler_field_date extends views_handler_field_date {
  function option_definition() {
    $options = parent::option_definition();

    $options['date_format'] = array('default' => 'small');
    $options['custom_date_format'] = array('default' => '');

    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $date = tzbase_make_date();

    $form['date_format'] = array(
      '#type' => 'select',
      '#title' => t('Date format'),
      '#options' => array(
        'short' => date_format_date($date, 'short'),
        'medium' => date_format_date($date, 'medium'),
    	'long' => date_format_date($date, 'long'),
    	'custom' => t('Custom'),
      ),
      '#default_value' => isset($this->options['date_format']) ? $this->options['date_format'] : 'small',
    );

    $form['custom_date_format'] = array(
      '#type' => 'textfield',
      '#title' => t('Custom date format'),
      '#description' => t('See <a href="http://us.php.net/manual/en/function.date.php" target="_blank">the PHP docs</a> for date formats.'),
      '#default_value' => isset($this->options['custom_date_format']) ? $this->options['custom_date_format'] : '',
      '#process' => array('views_process_dependency'),
      '#dependency' => array('edit-options-date-format' => array('custom')),
    );
  }

  function render($values) {
    $value = $values->{$this->field_alias};

    if (!$value) {
      return theme('views_nodate');
    }

    $format = $this->options['date_format'];
    $date = tzbase_make_date($value);

    if($format == 'custom') {
      $custom_format = $this->options['custom_date_format'];
      return date_format_date($date, 'custom', $custom_format);
    }

    return date_format_date($date, $format);
  }
}
