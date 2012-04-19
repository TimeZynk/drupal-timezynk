define([
    'jquery',
    'underscore',
    'backbone',
    'text!templates/tz_alert.html',
], function($, _, Backbone, template) {
    return Backbone.View.extend({
        tmpl: _.template(template),

        tagName : "div",
        className : "alert alert-block",
        params : {
            data : {}
        },

        events: {
        	"click .dismiss" : "close",
        	"click .confirm" : "confirm"
        },

        initialize : function(obj){
            $.extend(this, obj);
        },

        render: function() {
            $(this.el).empty();
            this.content = this.tmpl(this.params);
            $(this.el).append(this.content);
            return this;
        },

        alert: function (type, header, msg){

        	this.className = "alert-" + type;

        	this.params.data = {
        		header: header,
        		body : msg
        	}

        	$(this.el).addClass(this.className);

			return this.render();
        },

        prompt: function (type, header, msg){
        	this.alert(type, header, msg);

        	var ok = this.make("button", {"class":"btn btn-" + type + " confirm", "name":"confirm"}, t.button_confirm);
        	var cancel = this.make("button", {"class":"btn dismiss", "name":"dismiss"}, t.button_cancel);

        	$(this.el).find(".button_area").append(ok);
        	$(this.el).find(".button_area").append(" ");
        	$(this.el).find(".button_area").append(cancel);

        	return this;
        },

        close:function(){
        	this.remove();
        },

        confirm:function(e){
        	this.trigger(e.currentTarget.name);
        	this.remove();
        }
    });
});
