define([
    'jquery',
    'underscore',
    'backbone',
    'lib/bootstrap-modal',
    'collections/managers',
    'i18n!nls/tzcontrol'
], function($, _, Backbone, Modal, Managers, t) {

    /*
     * Managers dropdown
     */
    return Backbone.View.extend({
        tagName: "select",
        className : "manager_filter",
        events:{
        	"change" : "selected"
        },

        initialize: function() {
            this.collection = new Managers();
            this.collection.setUrl("");
        },

        render: function() {
        	var that = this;
        	this.collection.fetch({
        		success: function(){
        			that.renderOptions();
        		},
        		error : function(){
        			console.log("Could not load managers. Sorry.")
        		}
        	})
            return this;
        },
        
        renderOptions : function(){
        	var that = this;
        	var options = [];
        	
        	var option = that.make(
    			"option",
    			{
    				"class":"filter_option", 
    				"value": null
    			},
    			t.managers_all
    		)
        	options.push(option);
        	
        	this.collection.each(function(o){
        		var option = that.make(
        			"option",
        			{
        				"class":"filter_option", 
        				"value":o.get("user_id")
        			},
        			o.get("fullname")
        		)
        		options.push(option);
        	});
        	$(this.el).html(options);
        },
        
        selected : function(e){
        	var manager = $(e.target).val();
        	this.trigger("filter", manager);
        }

    });
});
