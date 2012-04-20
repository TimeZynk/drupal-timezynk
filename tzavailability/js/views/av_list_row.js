define([
    'jquery',
    'underscore',
    'backbone',
    'bootstrap-tooltip',
    'bootstrap-popover',
    'template_views/list_row',
    'template_views/tz_alert'
], function($, _, Backbone, Tooltip, Popover, TZListRow, Alert) {
     /*
     * Generic list row
     */
    return TZListRow.extend({
        tagName : "tr",
        className: "plan_row",

        events : {
        	"send .select" : "sendSMS"
        },

        initialize : function(obj) {
        	$.extend(this, obj);
        },

        render : function(cols) {
            this.columns = cols;
            var that = this;

            var checkbox = this.make("input", {"type":"checkbox", "class":"row_select"});

            var last_login = new Date(this.model.get("last_login") * 1000);
            var cont = t.user_mobile_column + ": " + this.model.get("mobile") +
                "<br />" + t.user_last_login + ": " + dateFormat(last_login, "yyyy-mm-dd HH:MM:ss");

            var container = that.make(
            	"div",
            	{
            		"class" : "name_field_container",
	            	"rel" : "popover",
	            	"data-content" : cont,
	            	"title" : this.model.get("fullname")
            	},
            	this.model.get('fullname')
            );

            var box_td = that.make("td", {"class" : "select"}, checkbox);
            var td = that.make("td", {"class" : "name_field"}, container);

            $(container).popover({placement:"right", delay:{show:200, hide:10}});
            $(that.el).append(box_td);
            $(that.el).append(td);

            that.renderIntervals();
            return this;
        },

        renderIntervals : function(){
        	var that = this;

        	var td = that.make("td", {
                "class" : "plan_intervals"
            });
        	var blobs = this.make("div",{"class":"slot_container"});
        	var even = false;
        	var cols = [];
        	for(var i=0; i<this.intervals; i++){
        		if(even){
        			var sp = that.make("div", {
	                    "class" : "plan_interval"
	                });
        		} else{
        			var sp = that.make("div", {
	                    "class" : "plan_interval_odd"
	                });
        		}

        		even = !even;

                $(sp).css({
                	width: 100/this.intervals + "%"
                });{}

                cols.push(sp);
        	};
        	
        	this.renderBlobs(blobs);
        	$(blobs).append(cols);
        	$(td).append(blobs);
        	$(this.el).append(td);
        },

        renderBlobs : function(container){
        	var that =this;
        	var percent = this.total_interval/100;
        	var info_tmpl =  _.template('<%= title %>:<br/><%= date_str %><br/><%= from_str %> - <%= to_str %>');
			var all_blobs = [];
			
        	this.blobs.each(function(blob) {
        		if (blob.get('user_id') != that.model.get('id')) {
        			// Only show this users blobs...
        			return;
        		}

        		var className = "";
        		var popoverClassName = "";

        		if(blob.get('type') == "availability"){
        			className = "plan_slot slot_availability_" + blob.get("available");
        			popoverClassName = "popover_" + blob.get('type') + "_" + blob.get("available");
        		} else {
        			className = "plan_slot report_slot";
        			popoverClassName = "popover_" + blob.get('type');
        		}

        		var start_date = new Date(blob.get('start_time')*1000);
        		var end_date = new Date(blob.get('end_time')*1000);

        		var td = that.make("a", {
	            	"class" : className,
	            	"rel" : "popover",
	            	"data-content" : info_tmpl({
	            		title: blob.get('title'),
	            		date_str: dateFormat(start_date, 'yyyy-mm-dd'),
	            		from_str: dateFormat(start_date, 'HH:MM'),
	            		to_str: dateFormat(end_date, 'HH:MM')
	            	}),
	            	"title" : that.model.get("fullname")
	            },"<span></span>");

	            var left = (blob.get('start_time') - that.start_time)/percent;
	            var width = (blob.get('end_time') - blob.get('start_time'))/percent;

	            if(width + left > 100){
	            	width = 100 - left;
	            };

	            if(left < 0){
	            	width += left;
	            	left = 0;
	            }

	            $(td).css({
	            	left: left + "%",
	            	width: width + "%"
	            });
	            all_blobs.push(td);

	            if(left+(width/2) >= 70){
	            	$(td).popover({placement:"left", className : popoverClassName, delay:{show:200, hide:10}});
	            } else if (left+(width) <= 30){
	            	$(td).popover({placement:"right", className : popoverClassName, delay:{show:200, hide:10}});
	            } else {
	            	$(td).popover({placement:"bottom", className : popoverClassName, delay:{show:200, hide:10}});
	            }
        	});
        	$(container).append(all_blobs);
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
        }
    });
});
