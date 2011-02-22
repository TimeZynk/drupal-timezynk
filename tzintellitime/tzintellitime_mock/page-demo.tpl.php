<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0;"/>
    <meta name="apple-mobile-web-app-capable" content="yes"/>
    <meta name="apple-mobile-web-app-status-bar-style" content="default"/>
    <title>Demo</title>
    <link rel="icon" href="favicon.ico" type="image/vnd.microsoft.icon">
    <style type="text/css">
    input {
      font-size: 2em;
      top: 100px;
      position: absolute;
    }
    .messages {
      border: 2px solid green;
      background: #ccff99;
      padding: 5px;
      margin: 5px;
      font-family: sans-serif;
      font-size: 1.2em;
    }
    </style>
  </head>
  <body>
    <?php if ($show_messages && $messages): print $messages; endif; ?>
    <?php print $content; ?>
  </body>
</html>