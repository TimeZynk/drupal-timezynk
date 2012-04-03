define([
    'jquery',
    'underscore',
    'backbone',
    'models/session',
    'template_views/tzview',
    'text!templates/login.html'
], function($, _, Backbone, Session, TzView, template) {

    return TzView.extend({
        tmpl : _.template(template),

        events: {
            'click button': 'login',
        },

        render: function() {
            this.content = this.tmpl(this.params);
            $(this.el).append(this.content);

            return this;
        },

        login: function(e) {
            var that = this;

            e.preventDefault();
            var session = new Session({
                username: $('#loginUserName').val(),
                password: $('#loginPassword').val(),
                admin: true
            });

            session.save({}, {
                error: function(model, response) {
                    that.displayMessage(response.statusText, "alert-error");
                },
                success: function(model) {
                    window.location.reload();
                }
            });
            return false;
        }
    });
});
