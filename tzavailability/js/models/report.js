define([
  'underscore',
  'backbone',
], function(_, Backbone) {
    return Backbone.Model.extend({
    	model_type : "report",
        urlRoot: location.origin + '/api/reports',
    });
});
