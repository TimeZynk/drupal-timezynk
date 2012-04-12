define([
  'jquery',
  'underscore',
  'backbone',
  'models/session',
  'models/user',
  'views/availability_list_view',
], function (
	$,
	_,
	Backbone,
	Session,
	User,
	AvailabilityView
	) {
    var session = new Session({}),
        Routes = Backbone.Router.extend({
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
