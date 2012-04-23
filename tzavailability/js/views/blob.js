define([
    'jquery',
    'underscore',
    'backbone',
    'bootstrap-tooltip',
    'bootstrap-popover'
], function($, _, Backbone, Tooltip, Popover) {
     /*
     * Blob
     */
    return Backbone.View.extend({
        tagName : "div",
        tmpl :  _.template('<%= title %>:<br/><%= date_str %><br/><%= from_str %> - <%= to_str %>'),
        className : "plan_slot",
        popoverClassName : "",

        initialize : function(obj) {
        	$.extend(this, obj);
    		this.start_date = new Date(this.model.get('start_time')*1000);
    		this.end_date = new Date(this.model.get('end_time')*1000);
    		this.left = (this.model.get('start_time') - this.interval_start)/this.percent;
            this.width = (this.model.get('end_time') - this.model.get('start_time'))/this.percent;
        },

        render : function() {
    		this.setClass();
            this.setAttributes();
            this.positionBlob();
            this.renderPopover();
            $(this.el).html(this.make("span"));
            return this;
        },
        
        setClass : function(){
        	if(this.model.get('type') == "availability"){
    			$(this.el).addClass("slot_availability_" + this.model.get("available"));
    			popoverClassName = "popover_" + this.model.get('type') + "_" + this.model.get("available");
    		} else {
    			$(this.el).addClass("report_slot");
    			popoverClassName = "popover_" + this.model.get('type');
    		}
        },
        
        setAttributes : function(){
        	$(this.el).attr({
        		"rel" : "popover",
	        	"data-content" : this.tmpl({
	        		title: this.model.get('title'),
	        		date_str: dateFormat(this.start_date, 'yyyy-mm-dd'),
	        		from_str: dateFormat(this.start_date, 'HH:MM'),
	        		to_str: dateFormat(this.end_date, 'HH:MM')
	        	}),
	        	"title" : this.user_model.get("fullname")
        	});
        },
        
        positionBlob : function(){
            if(this.width + this.left > 100){
            	width = 100 - this.left;
            };
            if(this.left < 0){
            	width += this.left;
            	left = 0;
            }
            $(this.el).css({
            	left: this.left + "%",
            	width: this.width + "%"
            });
        },
        
        renderPopover : function(){
        	if(this.left+(this.width/2) >= 70){
            	$(this.el).popover({placement:"left", className : this.popoverClassName, delay:{show:200, hide:10}});
            } else if (this.left+(this.width) <= 30){
            	$(this.el).popover({placement:"right", className : this.popoverClassName, delay:{show:200, hide:10}});
            } else {
            	$(this.el).popover({placement:"bottom", className : this.popoverClassName, delay:{show:200, hide:10}});
            }
        }
    });
});
