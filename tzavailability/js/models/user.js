define([
  'underscore',
  'backbone',
  'collections/availabilities'
], function(_, Backbone, Availabilities) {
    return Backbone.Model.extend({
        urlRoot: location.protocol + '//' + location.host + '/api/users',

        availabilities: function() {
            var avs = new Availabilities();
            avs.url = avs.url + '?user_id=' + this.id;
            return avs;
        }
    });
});
