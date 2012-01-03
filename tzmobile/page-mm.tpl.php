<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">

<head>
<link rel="apple-touch-icon" href="<?php print $base_path . $directory ?>/apple-touch-icon.png"/>
<link rel="apple-touch-startup-image" href="<?php print $base_path . $directory ?>/apple-touch-startup-image.png" />
<meta content="yes" name="apple-mobile-web-app-capable" />
<title>TZ Team</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<?php print $head ?>
<!-- ?php print $styles ? -->

<link rel="stylesheet" href="<?php print drupal_get_path('module', 'tzmobile') . '/tzmobile.css' ?>"/>
<link rel="stylesheet" href="http://code.jquery.com/mobile/1.0/jquery.mobile-1.0.min.css" />
<script src="http://code.jquery.com/jquery-1.6.4.min.js"></script>
<script src="http://code.jquery.com/mobile/1.0/jquery.mobile-1.0.min.js"></script>
<?php print $scripts ?>
</head>

<body>
<div data-role="page">
  <div id="topbar" data-role="header" class="ui-header ui-bar-a">
    <h1>
      <?php
      if($title) {
        print $title;
      } else {
        print $site_name;
      }
      ?>
    </h1>
  </div>

  <div data-role="content">
    <?php if($messages) { ?>
    <ul class="pageitem">
      <?php print $messages ?>
    </ul>
    <?php } ?>

    <?php print $content ?>
  </div>

  <?php if ($footer_message): ?>
  <div id="footer">
    <?php print $footer_message; ?>
  </div>
  <?php endif; ?>
</div>
<?php print $closure ?>
</body>
</html>
