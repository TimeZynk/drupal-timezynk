define([
    'jquery',
    'underscore',
    'backbone',
    'collections/users',
    'models/user',
    'template_views/tzview',
    'template_views/list_view',
    'template_views/list_row',
    'text!templates/availability_list.html'
], function($, _, Backbone, Users, User, TzView, ListView, ListRow, template) {

    /*
     * Availability list view
     */
    var AvailabilityListView = ListView.extend({
        tmpl: _.template(template),
    });

    return AvailabilityListView;
});
