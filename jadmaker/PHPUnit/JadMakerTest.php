<?php

require_once dirname(__FILE__) . '/../jadmaker.inc';

class JadMakerTest extends PHPUnit_Framework_TestCase {
  function testSingleAcceptLanguage() {
    global $_SERVER;
    $_SERVER['HTTP_ACCEPT_LANGUAGE'] = 'sv';
    $lang = jadmaker_detect_language();
    $this->assertEquals('sv', $lang);
  }

  function testThreeAcceptLanguage() {
    global $_SERVER;
    $_SERVER['HTTP_ACCEPT_LANGUAGE'] = 'dk, sv-se;q=0.9, en;q=0.6';
    $lang = jadmaker_detect_language();
    $this->assertEquals('sv', $lang);
  }

  function testFiveAcceptLanguageEnglish() {
    global $_SERVER;
    $_SERVER['HTTP_ACCEPT_LANGUAGE'] = 'dk, de;q=0.9, en-us;q=0.8, sv-se;q=0.7, sv;q=0.6';
    $lang = jadmaker_detect_language();
    $this->assertEquals('en', $lang);
  }

  function testNoAcceptLanguageEnglishUserAgent() {
    global $_SERVER;
    $_SERVER['HTTP_ACCEPT_LANGUAGE'] = '';
    $_SERVER['HTTP_USER_AGENT'] = 'Mozilla/5.0 (iPhone Simulator; U; CPU iPhone OS 4_1 like Mac OS X; en-us) AppleWebKit/532.9 (KHTML, like Gecko) Version/4.0.5 Mobile/8B117 Safari/6531.22.7';
    $lang = jadmaker_detect_language();
    $this->assertEquals('en', $lang);
  }

  function testUnknownAcceptLanguageEnglishUserAgent() {
    global $_SERVER;
    $_SERVER['HTTP_ACCEPT_LANGUAGE'] = 'dk, de';
    $_SERVER['HTTP_USER_AGENT'] = 'Mozilla/5.0 (iPhone Simulator; U; CPU iPhone OS 4_1 like Mac OS X; en-us) AppleWebKit/532.9 (KHTML, like Gecko) Version/4.0.5 Mobile/8B117 Safari/6531.22.7';
    $lang = jadmaker_detect_language();
    $this->assertEquals('en', $lang);
  }

  function testUnknownAcceptLanguageSwedishUserAgent() {
    global $_SERVER;
    $_SERVER['HTTP_ACCEPT_LANGUAGE'] = 'dk, de';
    $_SERVER['HTTP_USER_AGENT'] = 'Mozilla/5.0 (iPhone Simulator; U; CPU iPhone OS 4_1 like Mac OS X; sv-SE) AppleWebKit/532.9 (KHTML, like Gecko) Version/4.0.5 Mobile/8B117 Safari/6531.22.7';
    $lang = jadmaker_detect_language();
    $this->assertEquals('sv', $lang);
  }

  function testUnknownAcceptLanguageUnknownUserAgentEnglishGlobal() {
    global $_SERVER, $language;
    $_SERVER['HTTP_ACCEPT_LANGUAGE'] = 'dk, de';
    $_SERVER['HTTP_USER_AGENT'] = 'Mozilla/5.0 (iPhone Simulator; U; CPU iPhone OS 4_1 like Mac OS X; dk) AppleWebKit/532.9 (KHTML, like Gecko) Version/4.0.5 Mobile/8B117 Safari/6531.22.7';
    $language->language = 'en';
    $lang = jadmaker_detect_language();
    $this->assertEquals('en', $lang);
  }

  function testUnknownAcceptLanguageUnknownUserAgentSwedishGlobal() {
    global $_SERVER, $language;
    $_SERVER['HTTP_ACCEPT_LANGUAGE'] = 'dk, de';
    $_SERVER['HTTP_USER_AGENT'] = 'Mozilla/5.0 (iPhone Simulator; U; CPU iPhone OS 4_1 like Mac OS X; dk) AppleWebKit/532.9 (KHTML, like Gecko) Version/4.0.5 Mobile/8B117 Safari/6531.22.7';
    $language->language = 'sv';
    $lang = jadmaker_detect_language();
    $this->assertEquals('sv', $lang);
  }

  function testUnknownAcceptLanguageUnknownUserAgentUnknownGlobal() {
    global $_SERVER, $language;
    $_SERVER['HTTP_ACCEPT_LANGUAGE'] = 'dk, de';
    $_SERVER['HTTP_USER_AGENT'] = 'Mozilla/5.0 (iPhone Simulator; U; CPU iPhone OS 4_1 like Mac OS X; dk) AppleWebKit/532.9 (KHTML, like Gecko) Version/4.0.5 Mobile/8B117 Safari/6531.22.7';
    $language->language = 'de';
    $lang = jadmaker_detect_language();
    $this->assertEquals('sv', $lang);
  }

  function testBasicSwedishAndroidFile() {
    global $_SERVER;
    $_SERVER['HTTP_USER_AGENT'] = 'Mozilla/5.0 (Linux; U; Android 2.2; sv-se; HTC Desire Build/FRF91) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1';
    $file = jadmaker_find_file('sv');
    $this->assertEquals("tz-Generic-android-sv.apk", $file->name);
  }

  function testBasicEnglishAndroidFile() {
    global $_SERVER;
    $_SERVER['HTTP_USER_AGENT'] = 'Mozilla/5.0 (Linux; U; Android 2.2; sv-se; HTC Desire Build/FRF91) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1';
    $file = jadmaker_find_file('en');
    $this->assertEquals("tz-Generic-android-en.apk", $file->name);
  }

  function testBasicEnglishSEMCFile() {
    global $_SERVER;
    $_SERVER['HTTP_USER_AGENT'] = 'SonyEricssonC902/R3CA Browser/NetFront/3.4 Profile/MIDP-2.1 Configuration/CLDC-1.1 JavaPlatform/JP-8.3.1';
    $file = jadmaker_find_file('en');
    $this->assertEquals("tz-Sony-Ericsson-JavaPlatform8-en.jar", $file->name);
  }

  function testBasicSwedishSEMCFile() {
    global $_SERVER;
    $_SERVER['HTTP_USER_AGENT'] = 'SonyEricssonC902/R3CA Browser/NetFront/3.4 Profile/MIDP-2.1 Configuration/CLDC-1.1 JavaPlatform/JP-8.3.1';
    $file = jadmaker_find_file('sv');
    $this->assertEquals("tz-Sony-Ericsson-JavaPlatform8-sv.jar", $file->name);
  }

  function testBasicEnglishNokiaFile() {
    global $_SERVER;
    $_SERVER['HTTP_USER_AGENT'] = 'Nokia5310XpressMusic/2.0 (08.32) Profile/MIDP-2.1 Configuration/CLDC-1.1';
    $file = jadmaker_find_file('en');
    $this->assertEquals("tz-Generic-AnyPhone-en.jar", $file->name);
  }

  function testBasicSwedishNokiaFile() {
    global $_SERVER;
    $_SERVER['HTTP_USER_AGENT'] = 'Nokia5310XpressMusic/2.0 (08.32) Profile/MIDP-2.1 Configuration/CLDC-1.1';
    $file = jadmaker_find_file('sv');
    $this->assertEquals("tz-Generic-AnyPhone-sv.jar", $file->name);
  }

  function testDynamicLanguageAndroidFile() {
    global $_SERVER;
    $_SERVER['HTTP_USER_AGENT'] = 'Mozilla/5.0 (Linux; U; Android 2.2; sv-se; HTC Desire Build/FRF91) AppleWebKit/533.1 (KHTML, like Gecko) Version/4.0 Mobile Safari/533.1';
    $file = jadmaker_find_file();
    $this->assertEquals("tz-Generic-android.apk", $file->name);
  }

  function testDynamicLanguageNokiaFile() {
    global $_SERVER;
    $_SERVER['HTTP_USER_AGENT'] = 'Nokia5310XpressMusic/2.0 (08.32) Profile/MIDP-2.1 Configuration/CLDC-1.1';
    $file = jadmaker_find_file();
    $this->assertEquals("tz-Generic-AnyPhone.jar", $file->name);
  }

  function testDynamicLanguageSEMCFile() {
    global $_SERVER;
    $_SERVER['HTTP_USER_AGENT'] = 'SonyEricssonC902/R3CA Browser/NetFront/3.4 Profile/MIDP-2.1 Configuration/CLDC-1.1 JavaPlatform/JP-8.3.1';
    $file = jadmaker_find_file();
    $this->assertEquals("tz-Sony-Ericsson-JavaPlatform8.jar", $file->name);
  }

  function testM1000AndroidPackage() {
    $_SERVER['SERVER_NAME'] = 'm1000.tzapp.com';
    $package = jadmaker_android_package_name();
    $this->assertEquals('com.tzapp.m1000', $package);
  }

  function testStripWWWFromAndroidPackage() {
    $_SERVER['SERVER_NAME'] = 'www.m1000.tzapp.com';
    $package = jadmaker_android_package_name();
    $this->assertEquals('com.tzapp.m1000', $package);
  }

  function testConvertIllegalCharactersInAndroidPackage() {
    $_SERVER['SERVER_NAME'] = 'www.demo-en.tzapp.com';
    $package = jadmaker_android_package_name();
    $this->assertEquals('com.tzapp.demo_en', $package);
  }
}
