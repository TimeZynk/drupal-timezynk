/*!
 * TimeZynk Web
 * http://timezynk.com/
 *
 * Copyright 2012, TimeZynk AB
 * All rights reserved.
 */

// Handle missing console.log in IE
if (typeof(console) === 'undefined') {
    console = {};
}
if (typeof(console.log) === 'undefined') {
    console.log = function() {};
}


require.config({
    paths: {
        // Major libraries
        jquery: '../lib/jquery-1.7.1',
        underscore: '../lib/underscore', // https://github.com/amdjs
        backbone: '../lib/backbone', // https://github.com/amdjs
        tooltip: 'lib/bootstrap-tooltip',
        popover: 'lib/bootstrap-popover',
        list: '../lib/list',
        json2: '../lib/json2',

        // Require.js plugins
        text: '../lib/require/text',
        i18n: '../lib/require/i18n',
        
        date_utils: '../lib/date_utils',

        // Just a short cut so we can put our html outside the js dir
        templates: '../templates',
        lib: '../lib'
    }
});

// Let's kick off the application

require([
    'json2',
    'routes',
    'i18n!nls/tzcontrol',
    'date_utils'
], function(json2, routes, t, date_utils) {
    window.t = t;
    routes.initialize();
});
