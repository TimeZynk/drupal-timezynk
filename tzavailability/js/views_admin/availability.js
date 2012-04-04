define([
    'jquery',
    'underscore',
    'backbone',
    'collections/users',
    'models/user',
    'template_views/tzview',
    'views_admin/availability_list',
    'text!templates/availability.html'
], function($, _, Backbone, Users, User, TzView,  Availability, template) {

    /*
     * Users view
     */
    var UsersView = TzView.extend({
        tmpl: _.template(template),

        render: function() {
            var that = this,
            collection = new Users();
            console.log("Working");

            collection.url = location.origin + '/api/users?status[10]=10&status[20]=20';
            
            this.content = this.tmpl(this.params);
            $(this.el).append(this.content);
            
            var theFrame = $("iframe", parent.document.body);
			theFrame.height(700);

            var table = new Availability({
                collection: collection,
                columns: [
                    {title: t.user_name_column, field:"fullname"}
                ]
            });

            $(this.el).find("#availability_container").append(table.render().el);

            return this;
        }
    });

    return UsersView;
});
