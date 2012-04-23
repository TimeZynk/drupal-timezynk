<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>TimeZynk</title>
    <meta name="description" content="TimeZynk Availability Panel">
    <meta name="author" content="TimeZynk AB">
    <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">

    <!--[if lt IE 9]>
      <script src="http://html5shim.googlecode.com/svn/trunk/html5.js"></script>
    <![endif]-->

    <base href="/<?php print drupal_get_path('module', 'tzavailability') ?>/"/>
    <script type="application/javascript">
    	var current_drupal_user = <?php 
    		global $user; 
    		$account = clone($user); 
    		unset($account->pass); 
    		print json_encode($account); ?>;
    </script>
    <script
      data-main="/<?php print drupal_get_path('module', 'tzavailability') ?>/js/main"
      src="/<?php print drupal_get_path('module', 'tzavailability') ?>/lib/require/require.js"></script>
    <link rel="stylesheet" href="/<?php print drupal_get_path('module', 'tzavailability') ?>/css/tzcontrol.css">
  </head>

  <body>
    <div  class="container-fluid" id="main">

    </div>
  </body>
</html>
