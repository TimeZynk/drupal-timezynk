define([
  'jquery',
  'underscore',
  'backbone',
  'models/session',
  'models/user',
  'views_admin/availability',
], function (
	$, 
	_, 
	Backbone,
	Session,
	User,
	Availability
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
            this.current_view = new Availability();
            $('#main').html(this.current_view.render().el);
        }
    });

    return {
        initialize: function() {
            return new Routes();
        }
    };
});
