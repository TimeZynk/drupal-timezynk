define([
    'jquery',
    'underscore',
    'backbone',
    'bootstrap-tooltip',
    'bootstrap-popover',
    'template_views/list_row',
    'views/blob',
    'template_views/tz_alert'
], function($, _, Backbone, Tooltip, Popover, TZListRow, Blob, Alert) {
     /*
     * Generic list row
     */
    return TZListRow.extend({
        tagName : "tr",
        className: "plan_row",

        events : {
        	"send .select" : "sendSMS",
        	"click .slot_availability_true span" : "selectRow"
        },

        initialize : function(obj) {
        	$.extend(this, obj);
        },

        render : function(cols) {
            this.columns = cols;
            var elements = this.renderColumns();
            var blobs = this.make("div", {"class":"slot_container"}, this.renderIntervals());
            elements.push(this.make("td", {"class" : "plan_intervals"}, blobs));
            
            $(blobs).append(this.renderBlobs());
            
            $(this.el).html(elements);
            
            return this;
        },
        
        renderColumns : function(){
        	var cols = [];
        	var checkbox = this.make("input", {"type":"checkbox", "class":"row_select"});
            var last_login = new Date(this.model.get("last_login") * 1000);
            var pop_over_content = 	t.user_mobile_column + ": " + this.model.get("mobile") +
            				"<br />" + t.user_last_login + ": " + dateFormat(last_login, "yyyy-mm-dd HH:MM:ss");

            var container = this.make(
            	"div",
            	{
            		"class" : "name_field_container",
	            	"rel" : "popover",
	            	"data-content" : pop_over_content,
	            	"title" : this.model.get("fullname")
            	},
            	this.model.get('fullname')
            );
            cols.push(this.make("td", {"class" : "select"}, checkbox));
            cols.push(this.make("td", {"class" : "name_field"}, container));
            $(container).popover({placement:"right", delay:{show:200, hide:10}});
            
            return cols;
        },

        renderIntervals : function(){
        	var that = this;
        	var even = false;
        	var cols = [];
        	
        	for(var i=0; i<this.intervals; i++){
        		var className = "plan_interval";
        		if(even){
        			className = "plan_interval_odd";
        		}
        		var sp = that.make("div", {
                    "class" : className
                });
                $(sp).css({
                	width: 100/this.intervals + "%"
                });{}
                cols.push(sp);
                even = !even;
        	};
        	
        	return cols;
        },

        renderBlobs : function(container){
        	var that =this;
        	var percent = this.total_interval/100;
			var all_blobs = [];
			
        	this.blobs.each(function(blob_model) {
        		if (blob_model.get('user_id') != that.model.get('id')) {
        			// Only show this users blobs...
        			return;
        		}
        		var blob = new Blob({
        			model: blob_model,
        			user_model : that.model,
        			interval_start : that.start_time,
        			percent : that.total_interval/100
        		});
        		
        		all_blobs.push(blob.render().el);
        	});
        	return all_blobs;
        },

        sendSMS : function(e, message){
        	var user = this.model;
        	var that = this;
        	sms = new Backbone.Model();
        	sms.url = location.protocol + '//' + location.host + '/api/sms';
        	var a = new Alert();

            sms.save({
                recipients: [user.get('id')],
                text: message
            }, {
                success: function(m, msg) {
                	$("#msg_area").html(a.alert("info", t.success, t.message_sent).el);
                },
                error: function(m, msg){
                	$("#msg_area").html(a.alert("danger", t.failure, t.message_failed).el);
                }
            });
        },
        
        selectRow : function(){
        	var checkbox = $(this.el).find('input.row_select');
        	checkbox.attr('checked', !checkbox.attr('checked'));
        }
    });
});
