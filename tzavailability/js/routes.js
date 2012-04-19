define([
  'jquery',
  'underscore',
  'backbone',
  'models/user',
  'views/availability_list_view',
], function ($, _, Backbone, User, AvailabilityView) {
    var Routes = Backbone.Router.extend({
        routes: {
            "*actions" : "availability"
        },

        initialize: function() {
        	this.availability();
        },

        availability: function(){
            this.current_view = new AvailabilityView();
            $('#main').html(this.current_view.render().el);
        }
    });

    return {
        initialize: function() {
            return new Routes();
        }
    };
});
