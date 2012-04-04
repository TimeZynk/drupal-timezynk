define([
    'jquery',
    'underscore',
    'backbone',
    'collections/users',
    'models/user',
    'template_views/list_view',
    'views/av_list_row',
    'text!templates/availability_list.html',
    'i18n!nls/tzcontrol'
], function($, _, Backbone, Users, User, ListView, AvListRow, template, t) {

    /*
     * Availability list view
     */
    var AvailabilityListView = ListView.extend({
        tmpl: _.template(template),
        columns: [{title: t.user_name_column, field:"fullname"}],
        row: AvListRow,
    });

    return AvailabilityListView;
});
