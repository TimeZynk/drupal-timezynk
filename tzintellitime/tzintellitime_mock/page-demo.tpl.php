<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0;"/>
    <meta name="apple-mobile-web-app-capable" content="yes"/>
    <meta name="apple-mobile-web-app-status-bar-style" content="default"/>
    <title>Demo</title>
    <link rel="icon" href="favicon.ico" type="image/vnd.microsoft.icon">
    <style type="text/css">
    body {
      font-family: sans-serif;
      font-size: 22px;
    }
    input {
      font-size: 40px;
      margin-top: 1em;
    }
    select {
      display: block;
      font-size: 22px;
    }
    .messages {
      padding: 5px;
      margin: 5px;
      border: 2px solid green;
      background: #ccff99;
    }
    .messages.error {
      border: 2px solid red;
      background: #ffcc99;
    }
    </style>
  </head>
  <body>
    <?php if ($show_messages && $messages): print $messages; endif; ?>
    <?php print $content; ?>
  </body>
</html>