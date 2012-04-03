define([
    'jquery',
    'underscore',
    'backbone',
    'collections/users',
    'models/user',
    'template_views/tzview',
    'template_views/list_view',
    'text!templates/user_downloads.html'
], function($, _, Backbone, Users, User, TzView, ListView, template) {

    /*
     * Single user view
     */
    var UserDownloadsView = TzView.extend({
        tmpl: _.template(template),
        
        events:{
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
            this.getDownloads();
        },
        
        getDownloads : function(){
        	var that = this;
        	var downloads = new Backbone.Model();
        	downloads.url = "/downloads/" + this.model.get("download-id");
        	
        	downloads.fetch({
                success:function(d){
                	that.renderDownloads(d);
                }
            });
        },
        
        renderDownloads : function(d) {
        	var downloads = new Backbone.Collection(d.get("access"));
        	var that = this;
        	var tmpl = _.template("<div class='well'><b>Timestamp:</b> <%= timestamp %><br />" + 
        						"<b>User agent:</b> <%= user_agent %><br />" + 
        						"<b>Accept language:</b> <%= accept_language %><br />" + 
        						"<b>Response body:</b> <pre><%= response_body %></pre>" + 
        						"<b>Response headers Content-Type:</b> <%= rh_content_type %></div>");
        	
        	downloads.each(function(d){
        		var div = tmpl({
                    timestamp: new Date(d.attributes.timestamp*1000),
                    user_agent: d.attributes.headers["user-agent"],
                    accept_language: d.attributes.headers["accept-language"],
                    response_body: d.attributes.response["body"],
                    rh_content_type: d.attributes.response.headers["Content-Type"]
                })
        		$("#downloads").append(div);
        	});
        }
	});


    return UserDownloadsView;
});
