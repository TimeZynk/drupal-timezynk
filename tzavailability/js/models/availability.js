define([
  'underscore',
  'backbone',
], function(_, Backbone) {
    return Backbone.Model.extend({
        urlRoot: location.origin + '/api/availabilities',
    });
});
