define([
    'jquery',
    'underscore',
    'backbone',
    'collections/users',
    'models/user',
    'template_views/tzview',
    'template_views/list_view',
    'template_views/list_row',
    'text!templates/user.html'
], function($, _, Backbone, Users, User, TzView, ListView, ListRow, template) {

    /*
     * Single user view
     */
    var UserView = TzView.extend({
        tmpl: _.template(template),
        
        events:{
        	"click #update_user" : "updateUser",
        	"click #send_message" : "sendMessage",
        	"click #send_install" : "sendInstallSMS",
        	"click #delete_btn" : "deletePrompt"
        },
        
        initialize: function(){
            var that = this;
            this.model.fetch({
                success:function(){
                    that.model.bind('change', that.renderContent, that);
                    that.renderContent();
                }
            });
        },
        
        renderContent: function(){
            this.params = {
                data : this.model.attributes
            }
            this.render();
        },
        
        updateUser : function(){
        	var that = this;
            var save_params = $.extend({}, this.save_params);

            //Find desired fields and save values
            var val = $(this.el).find('.model_param');
            val.each(function(i, el) {
                if ($(el).length > 0) { // Make sure the DOM exists before we add it
                	save_params[$(el).attr("name")] = $(el).val();
                }
            });
            
            this.save(save_params);
        },
        
        save : function(save_params){
            var that = this;
            this.model.save(save_params,{
                wait:true,
                success : function(m, msg) {
                	console.log("Saved changes");
                },
                error : function(m, msg) {
                    alert("error");
                    console.log(msg);
                }
            });
        },
        
        sendSMS : function(message){
        	var user = this.model;
        	var that = this;
        	sms = new Backbone.Model();
        	sms.url = '/sms';
            sms.save({
                from: "TimeZynk",
                to: user.get('int-mobile'),
                message: message
            }, {
                success: function(m, msg) {
                	alert(t.message_sent);
                },
                error: function(m, msg){
                	alert(t.message_failed);
                }
            });
        },
        
        sendInstallSMS : function(){
            var msg = "Välkommen till mobil tidrapportering! " +
                    "Installera appen genom att klicka på länken" + "http://" + location.host + this.model.get('download-link') + "och logga " +
                    "sedan in med \"" + this.model.get('login-names')[0] + "\"som användarnamn och med lösenordet som " +
                    "du har fått från din chef.";
            this.sendSMS(msg);
        },
        
        sendMessage : function(){
           this.sendSMS($(this.el).find("#sms_message").val());
        },
        
        deletePrompt :function() {
			if(confirm(t.user_confirm_delete)) {
				var user = this.model.get("name");
				var company_id = this.model.get("company-id");
				this.model.destroy();
				alert(user + " " + t.delete_successful)
				window.location.hash = "company/" + company_id + "/users";
			} else {
			}
		}
		});


    return UserView;
});
