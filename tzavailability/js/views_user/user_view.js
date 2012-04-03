define([
    'jquery',
    'underscore',
    'backbone',
    'text!templates/user.html'
], function($, _, Backbone) {

    return Backbone.View.extend({

        tmpl : _.template(template),

        params: {
            data : {}
        },

        events: {},

        render: function() {
            this.content = this.tmpl(this.params);
            $(this.el).append(this.content);

            return this;
        }
    });
});
