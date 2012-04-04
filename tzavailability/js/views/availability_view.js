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

        initialize: function() {
            this.collection = new Users();
        },

        render: function() {
            var that = this;

            this.content = this.tmpl(this.params);
            $(this.el).append(this.content);

            var theFrame = $("iframe", parent.document.body);
			theFrame.height(700);

            var table = new AvailabilityListView({
                collection: this.collection,
            });

            $(this.el).find("#availability_container").append(table.render().el);

            return this;
        }
    });
});
