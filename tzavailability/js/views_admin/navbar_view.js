define([
    'jquery',
    'underscore',
    'backbone',
    'text!templates/navbar.html'
], function($, _, Backbone, template) {
    return Backbone.View.extend({

        tmpl : _.template(template),

        render: function() {
            $(this.el).html(this.tmpl());
            return this;
        },

        routeEvent: function(event) {
            brand = event;
            this.render();
        },
    });
});
