define([
    'jquery',
    'underscore',
    'backbone',
    'template_views/tzview'
], function($, _, Backbone, TzView) {
     /*
     * Generic list row
     */
    return TzView.extend({

        tagName : "tr",
        columns : [],

        format_value: function(col) {
            if (typeof(col.format) !== 'undefined') {
                return col.format(this.model.get(col.field));
            } else {
                return this.model.get(col.field);
            }
        },

        render : function(cols) {
            this.columns = cols;
            var that = this;

            _.each(this.columns, function(col) {
                var td = that.make("td", {
                    "class" : col.title
                }, that.format_value(col));
                $(that.el).append(td);
            })
            return this;
        },
    });
});
