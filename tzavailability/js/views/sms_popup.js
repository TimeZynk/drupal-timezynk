define([
    'jquery',
    'underscore',
    'backbone',
    'lib/bootstrap-modal',
    'collections/users',
    'models/user',
    'template_views/tzview',
    'views/availability_list_view',
    'text!templates/sms_popup.html',
    'i18n!nls/tzcontrol'
], function($, _, Backbone, Modal, Users, User, TzView, AvailabilityListView, template, t) {

    /*
     * Users view
     */
    return TzView.extend({
    	
        tmpl: _.template(template),
        className : "well",
        
        events:{
            "click #save_btn" : "send",
            "click #cancel_btn" : "cancel"
        },

        initialize: function() {
            this.collection = new Users();
        },

        render: function() {
            var that = this;

            this.content = this.tmpl();
            $(this.el).append(this.content);
            
            this.bind('save-successful', this.close, this);
            return this;
        },
        
        close : function(){
        	this.remove();
        },
        
        send : function(){
        	var message = $(this.el).find("#message_field").val();
        	console.log(message);
        	this.trigger("send", message);
        	this.close();
        },
        
        cancel : function(){
        	this.trigger("cancel");
        	this.close();
        },

    });
});
