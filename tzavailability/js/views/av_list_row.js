define([
    'jquery',
    'underscore',
    'backbone',
    'template_views/list_row'
], function($, _, Backbone, TZListRow) {
     /*
     * Generic list row
     */
    return TZListRow.extend({
        tagName : "tr",

        initialize : function() {
            console.log(this.model);
            this.availabilities = this.model.availabilities();
        },

        render : function(cols) {
            this.columns = cols;
            var that = this;
            var c = $('<td class="fullname">' + this.model.get('fullname') + '</td>');
            $(that.el).append(c);
            this.availabilities.fetch();
            return this;
        },
    });
});
