define([
    'jquery',
    'underscore',
    'backbone',
], function($, _, Backbone) {

    return Backbone.View.extend({
        className: "alert",
        events : {
            "click .close" : "close"
        },
        initialize:function(obj){
            this.message = obj.message;
            this.render();
        },
        render: function(){
            console.log(this.message);
            $(this.el).html(this.message + '<a class="close">×</a>');

            $(this.el).hide();
            $(this.el).fadeIn();
            return this;
        },
        close:function(){
            this.remove();
        }
    });
});
