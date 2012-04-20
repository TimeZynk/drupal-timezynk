define([
  'underscore',
  'backbone',
], function(_, Backbone) {
    return Backbone.Model.extend({
    	model_type : "report",
        urlRoot: location.protocol + '//' + location.host + '/api/reports',
    });
});
