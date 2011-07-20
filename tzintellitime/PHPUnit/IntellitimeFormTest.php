<?php

class IntellitimeFormTest extends PHPUnit_Framework_TestCase {
  public function setUp() {
    $this->bot = $this->getMock('TZIntellitimeBot');
  }

  public function testWhenBuildingFromLoginPage_ItShouldParseCorrectFormAction() {
    $form = $this->build_from_page('intellitime-login-page.html');
    $this->assertEquals('Login.aspx?Gw27UDttLdgps9TM4HqqoQ%3d%3d', $form->getAction());
  }

  public function testWhenBuildingFromLoginPage_ItShouldParseCorrectFormValues() {
    $expectedFormValues = array(
      '__VIEWSTATE' => 'dDwyNDA3MjczMzc7dDw7bDxpPDA+Oz47bDx0PDtsPGk8MT47aTw1PjtpPDE1PjtpPDE5Pjs+O2w8dDw7bDxpPDA+Oz47bDx0PDtsPGk8MT47aTw3Pjs+O2w8dDxwPHA8bDxJbWFnZVVybDs+O2w8fi9DdXN0b21lcnMvaW50ZWxsaXBsYW5fbG9nby5naWY7Pj47Pjs7Pjt0PHA8cDxsPEltYWdlVXJsOz47bDx+L0ltYWdlcy9JbWdfSW50ZWxsaXBsYW5Mb2dvV2hpdGUuZ2lmOz4+Oz47Oz47Pj47Pj47dDxwPHA8bDxWaXNpYmxlOz47bDxvPGY+Oz4+Oz47Oz47dDxwPHA8bDxUZXh0Oz47bDxMb2dnYSBpbjs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDs+O2w8R2zDtm10IGzDtnNlbm9yZGV0Pzs+Pjs+Ozs+Oz4+Oz4+Oz5ngNWIe5WIW3O3prUuG7wbptC3jg==',
      'TextBoxUserName' => '',
      'TextBoxPassword' => '',
      'ButtonLogin' => 'Logga in',
    );
    $form = $this->build_from_page('intellitime-login-page.html');
    $this->assertEquals($expectedFormValues, $form->getFormValues());
  }

  public function testWhenBuildingFromEmptyWeekPage_ItShouldParseCorrectFormValues() {
    $expectedFormValues = array(
      '__EVENTTARGET' => '',
      '__EVENTARGUMENT' => '',
      '__VIEWSTATE' => 'dDwtMzc5NDU5ODQyO3Q8O2w8aTwwPjs+O2w8dDw7bDxpPDA+O2k8Mz47aTw0PjtpPDY+O2k8Nz47aTw4PjtpPDk+O2k8MTA+O2k8MTE+O2k8MTI+O2k8MTM+Oz47bDx0PDtsPGk8MD47PjtsPHQ8O2w8aTwxPjtpPDM+O2k8Nz47PjtsPHQ8cDxwPGw8SW1hZ2VVcmw7PjtsPGh0dHA6Ly9pcHdlYi5pbnRlbGxpcGxhbi5zZS9rdW5kbG9nby80MDk0LmpwZzs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDs+O2w8Sm9oYW4gSGVhbmRlcjs+Pjs+Ozs+O3Q8cDxwPGw8SW1hZ2VVcmw7PjtsPH4vSW1hZ2VzL0ltZ19JbnRlbGxpcGxhbkxvZ29XaGl0ZS5naWY7Pj47Pjs7Pjs+Pjs+Pjt0PHA8cDxsPFRleHQ7PjtsPDU7Pj47Pjs7Pjt0PHQ8O3A8bDxpPDA+O2k8MT47PjtsPHA8IFtWaXNhIGFsbGEgdXBwZHJhZ10gOzA+O3A8VGVzdGbDtnJldGFnZXQgRWZmZWt0LCBUcnVja2bDtnJhcmU7NjIwMD47Pj47bDxpPDA+Oz4+Ozs+O3Q8cDxsPF8hSXRlbUNvdW50Oz47bDxpPDQ+Oz4+Ozs+O3Q8cDxsPFZpc2libGU7PjtsPG88Zj47Pj47Oz47dDxwPGw8VmlzaWJsZTs+O2w8bzxmPjs+Pjs7Pjt0PHA8bDxfIUl0ZW1Db3VudDs+O2w8aTwwPjs+Pjs7Pjt0PDtsPGk8MTM+O2k8MTU+O2k8MTc+O2k8MTk+O2k8MjE+O2k8MjM+O2k8Mjc+O2k8Mjk+O2k8MzM+Oz47bDx0PHQ8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47PjtwPGw8aTwxPjtpPDI+O2k8Mz47aTw0PjtpPDU+O2k8Nj47aTw3Pjs+O2w8cDxtw6UsIDMxLzAxIDsyMDExLTAxLTMxPjtwPHRpLCAwMS8wMiA7MjAxMS0wMi0wMT47cDxvbiwgMDIvMDIgOzIwMTEtMDItMDI+O3A8dG8sIDAzLzAyIDsyMDExLTAyLTAzPjtwPGZyLCAwNC8wMiA7MjAxMS0wMi0wND47cDxsw7YsIDA1LzAyIDsyMDExLTAyLTA1PjtwPHPDtiwgMDYvMDIgOzIwMTEtMDItMDY+Oz4+Oz47Oz47dDx0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47cDxsPGk8MT47aTwyPjtpPDM+Oz47bDxwPFRlc3Rmw7ZyZXRhZ2V0IEVmZmVrdCwgVHJ1Y2tmw7ZyYXJlOzYyMDA+O3A8LS0tOy0xPjtwPFxlO19BQ18+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPHA8bDxCYWNrQ29sb3I7XyFTQjs+O2w8MjxcZT47aTw4Pjs+Pjs+Ozs+O3Q8cDxwPGw8QmFja0NvbG9yO18hU0I7PjtsPDI8XGU+O2k8OD47Pj47Pjs7Pjt0PHA8cDxsPEJhY2tDb2xvcjtfIVNCOz47bDwyPFxlPjtpPDg+Oz4+Oz47Oz47dDxwPGw8VmlzaWJsZTs+O2w8bzxmPjs+PjtsPGk8MT47PjtsPHQ8O2w8aTwxPjs+O2w8dDxwPHA8bDxUZXh0Oz47bDxcZTs+Pjs+Ozs+Oz4+Oz4+Oz4+O3Q8cDxwPGw8VGV4dDs+O2w8VXBwZGF0ZXJhOz4+Oz47Oz47dDxwPHA8bDxUZXh0O1Zpc2libGU7PjtsPFZlY2thIEtsYXI7bzxmPjs+Pjs+Ozs+O3Q8cDxwPGw8VGV4dDtWaXNpYmxlOz47bDzDhG5kcmEgdmVja2E7bzxmPjs+Pjs+Ozs+Oz4+Oz4+O2w8RnVsbERheUNoZWNrQm94Oz4+',
      'DoPost' => 'true',
      'AddTimeFromTextBox' => '',
      'AddTimeToTextBox' => '',
      'AddBreakTextBox' => '',
      'AddExplicitOvertimeTextBox' => '',
      'AddNoteTextBox' => '',
      'UpdateButton' => 'Uppdatera',
      'CustOrdersDropDown' => '0',
      'AddDateDropDown' => '',
      'AddRowDropDown' => '',
    );
    $form = $this->build_from_page('WeekData_v9SyncEmptyWeek.txt');
    $this->assertEquals($expectedFormValues, $form->getFormValues());
  }

  private function build_from_page($filename) {
    $full_name = dirname(__FILE__) . "/../tests/$filename";
    $handle = fopen($full_name, "r");
    $contents = fread($handle, filesize($full_name));
    fclose($handle);
    $page = new IntellitimePage($contents, $this->bot);
    return $page->getForm();
  }
}