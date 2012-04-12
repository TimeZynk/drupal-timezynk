define([
    'jquery',
    'underscore',
    'backbone',
    'lib/bootstrap-tooltip',
    'lib/bootstrap-popover',
    'template_views/list_row'
], function($, _, Backbone, Tooltip, Popover, TZListRow) {
     /*
     * Generic list row
     */
    return TZListRow.extend({
        tagName : "tr",
        className: "plan_row",
        
        events : {
        	"click .name_field a" : "displayInfo"
        },
        
        initialize : function(obj) {
        	$.extend(this, obj);
        },

        render : function(cols) {
            this.columns = cols;
            var that = this;
            var container = that.make("div", {"class" : "name_field_container"}, "<a>" + this.model.get('fullname') + "</a>");
            var td = that.make("td", {"class" : "name_field"}, container);
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
                
                $(blobs).append(sp);
        	};
        	$(td).append(blobs);
        	$(this.el).append(td);
        	
        	this.renderBlobs(blobs);
        },
        
        renderBlobs : function(container){
        	var that =this;
        	var percent = this.total_interval/100;
        	        	
        	this.slots.each(function(blob){
        		
        		var td = that.make("a", {
	            	"class" : "plan_slot slot_available" + blob.get("availability_type"),
	            	"rel" : "popover",
	            	"data-content" : new Date(blob.get("start_time")*1000),
	            	"title" : that.model.get("fullname")
	            },"<span></span>");
	            
	            var left = (blob.get("start_time") - that.start_time)/percent;
	            var width = (blob.get("end_time") - blob.get("start_time"))/percent;
	            
	            if(width + left > 100){
	            	width = 100 - left;
	            };
	            
	            if(left < 0){
	            	width += left;
	            	left = 0;
	            }
	            
	            $(td).css({
	            	top:5,
	            	left: left + "%",
	            	width: width + "%"
	            });
	            $(container).append(td);
	            
	            if(left+(width/2) > 70){
	            	$(td).popover({placement:"left"});
	            } else if(left+(width/2) < 20){
	            	$(td).popover({placement:"right"});
	            }{
	            	$(td).popover({placement:"top"});
	            }
        	});
        },
        
        displayInfo : function(){
        	
        }
    });
});
