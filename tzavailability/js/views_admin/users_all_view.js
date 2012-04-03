define([
    'jquery',
    'underscore',
    'backbone',
    'collections/users',
    'models/user',
    'template_views/tzview',
    'template_views/list_view',
    'template_views/list_row',
    'text!templates/all_users.html'
], function($, _, Backbone, Users, User, TzView, ListView, ListRow, template) {

    /*
     * Users view
     */
    var UsersView = TzView.extend({
        tmpl: _.template(template),

        render: function() {
            var that = this,
            collection = new Users();

            collection.url = '/users';
            
            this.content = this.tmpl(this.params);
            $(this.el).append(this.content);

            var table = new ListView({
                collection: collection,
                columns: [
                    {title: t.user_name_column, field:"name"},
                    {title: t.user_mobile_column, field:"mobile"},
                    {title: t.company_heading, field:"company-id"},
                    {title: t.user_last_login_column, field:"last-login"}
                ]
            });

            $(this.el).find("#companies_list").append(table.render().el);

            return this;
        }
    });

    return UsersView;
});
