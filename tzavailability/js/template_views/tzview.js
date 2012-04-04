define([
    'jquery',
    'underscore',
    'backbone',
    'text!templates/availability.html',
], function($, _, Backbone, template) {
    return Backbone.View.extend({
        tmpl: _.template(template),
        params : {
            data : {}
        },

        events: {},

        initialize : function(obj){
            $.extend(this, obj);
        },
        render: function() {
            $(this.el).empty();
            this.content = this.tmpl(this.params);
            $(this.el).append(this.content);
            this.rendered();
            return this;
        },
        rendered: function (){

        },
        checkFields : function() {

        },
        displayMessage: function(msg, type, target){
            $(this.el).find(".alert").remove();
            /*var alert = new MessageView({
                className: "alert " + type,
                message: msg
            });
            if(target){
                $(this.el).find(target).html(alert.el);
            } else if($(this.el).find("#msg_field")){
                $(this.el).find("#msg_field").prepend(alert.el);
            } else {
                $(this.el).find(".main-area").prepend(alert.el);
            }*/
        },

        clearMessages : function(){
            $(this.el).find(".alert").remove();
        },

        fetchData: function(path, method){
            // Useful generic function to retrieve data from backend
            var that = this;
            var tempModel = new Backbone.Model({});
            tempModel.url = path;
            tempModel.fetch({
                success: function(){
                    tempModel.off("change"); //Disable further events
                },
                error: function(m, msg){
                    that.displayMessage(msg.statusText, "alert-error");
                }
            });

            return tempModel;
        },
    });
});
