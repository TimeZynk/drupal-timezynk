define([
  'underscore',
  'backbone',
], function(_, Backbone) {
    return Backbone.Model.extend({
    	model_type : "availability",
        urlRoot: location.protocol + '//' + location.host + '/api/availabilities',
    });
});
