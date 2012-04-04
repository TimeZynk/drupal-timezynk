define([
  'backbone',
  'models/availability'
], function(Backbone, Availability) {
    return Backbone.Collection.extend({
        url: location.origin + '/api/availabilities',
        model: Availability
    });
});
