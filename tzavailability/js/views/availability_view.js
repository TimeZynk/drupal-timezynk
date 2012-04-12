define([
    'jquery',
    'underscore',
    'backbone',
    'collections/users',
    'models/user',
    'template_views/tzview',
    'views/availability_list_view',
    'text!templates/availability.html'
], function($, _, Backbone, Users, User, TzView, AvailabilityListView, template) {

    /*
     * Users view
     */
    return TzView.extend({
        tmpl: _.template(template),
        
        events:{
        	"click #prev_week" : "prevWeek",
        	"click #next_week" : "nextWeek",
        	"click #curr_week" : "resetWeek",
        	"click #btn_day" : "dayView",
        	"click #btn_week" : "weekView"
        },

        initialize: function() {
            this.collection = new Users();
        },

        render: function() {
            var that = this;

            this.content = this.tmpl(this.params);
            $(this.el).append(this.content);

            var theFrame = $("iframe", parent.document.body);
			theFrame.height(700);

            this.availability = new AvailabilityListView({
                collection: this.collection,
            });

            $(this.el).find("#availability_container").append(this.availability.init().el);

            return this;
        },
        
		nextWeek : function() {
			this.availability.nextInterval();
		},
		prevWeek : function() {
			this.availability.prevInterval();
		},
		resetWeek : function() {
			this.availability.resetInterval();
		},
		dayView : function() {this.availability.dayView();},
		weekView : function() {this.availability.weekView();}
    });
});
