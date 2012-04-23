define([
    'jquery',
    'underscore',
    'backbone',
    'bootstrap-tooltip',
    'bootstrap-popover',
    'bootstrap-button',
    'collections/users',
    'collections/blobs',
    'models/user',
    'functions/users_filters',
    'template_views/list_view',
    'views/av_list_row',
    'views/sms_popup',
    'views/manager_dropdown',
    'text!templates/availability.html',
    'i18n!nls/tzcontrol'
], function($, _, Backbone, Tooltip, Popover, Buttons, Users, Blobs, User, usersFilters, ListView, AvListRow, SmsPopup, ManagerSelect, template, t) {
    /*
     * Availability list view
     */
    var AvailabilityListView = ListView.extend({
        tmpl: _.template(template),
        columns: [{title: t.user_name_column, field:"fullname"}],
        row: AvListRow,

        start_time : new Date(),
        intervals: 7,
        total_interval: 604800, // A week

        events:{
        	"click #prev_week" : "prevInterval",
        	"click #next_week" : "nextInterval",
        	"click #curr_week" : "resetInterval",
        	"click .btn_day" : "dayView",
        	"click #btn_week" : "weekView",
        	'click input[name="select-all"]' : 'selectAll',
        	'click #send_message' : "composeSMS"
        },

        initialize : function(obj) {
            _.bindAll(this, 'render', 'addOne', 'addAll', 'addItem');
            $.extend(this, obj);
            this.collection = new Users();
        },
        
        render : function() {
            this.content = this.tmpl(this.params);
            $(this.el).append(this.content);
            
            var managers = new ManagerSelect();
            $(this.el).find("#filters").append(managers.render().el);
            managers.bind("filter", this.filterCollection, this);
            
            this.collection.bind('add', this.addOne, this);
            this.collection.bind('add', this.setupList, this);
            this.collection.bind('reset', this.addAll, this);
            this.collection.bind('reset', this.setupList, this);

            this.blobs = new Blobs();
			this.resetInterval();

            return this;
        },

        renderHead : function(){        	
        	var cols = this.renderHeadColumns();
        	var intervals = this.renderHeadIntervalColumns();
            
            var blobs = this.make("div",{"class":"slot_container"}, intervals);
            var th = this.make("th", {"class" : "plan_intervals"}, blobs);
        	
        	$(this.el).find("thead tr").html(cols);
        	$(this.el).find("thead tr").append(th);
        },
        
        renderHeadColumns : function(){
        	var cols = [];
        	var that = this;
        	
            var checkbox = this.make("input", {
                "type" : "checkbox",
                "class" : "row_select",
                "name" : "select-all"
            });

            cols.push(this.make("th", {}, checkbox));
        	
        	_.each(this.columns, function(col) {
                var th = that.make("th", {
                    "class" : "sort",
                    "data-sort" : col.title
                }, col.title);
                cols.push(th);
            });
            
        	return cols;
        },
        
        renderHeadIntervalColumns : function(){
        	var that = this;
        	var even = false;
        	var intervalCols = [];
            var date = this.start_time.getEpoch();
            var incr = this.total_interval/this.intervals;
        	
            for(var i=0; i<this.intervals; i++){
        		var rend = that.getIntervalLabel(date);
    			var sp = that.make("div", {
                    "class" : "plan_interval"
                },rend);
        		date += incr;
                $(sp).css({
                	width: 100/this.intervals + "%"
                });
                intervalCols.push(sp);
        	};
        	return intervalCols;
        },

        reload:function(){
        	var that = this;
        	this.renderHead();
        	if (this.collection.length) {
        		this.addAll();
        	} else {
	            this.collection.fetch({
	                success : function() {
	                },
	                error: function(){
	                    console.log("Error list");
	                }
	            });
	        }
        },

		getIntervalLabel : function(date){
			var label_date = new Date(date*1000);
			var label = "";
			if(this.total_interval > 86400){
				label = "<a class='btn_day' data-toggle=" + label_date.getEpoch() + " >" + 
				t["weekday"+label_date.getDay()] + " " +
				label_date.getDate() + "/" + label_date.getMonth() + "</a>";
			} else {
				label = label_date.getHours() + ":00";
			}
			return label;
		},

		nextInterval : function() {
			this.updateInterval(this.total_interval);
		},

		prevInterval : function() {
			this.updateInterval(-this.total_interval);
		},

		resetInterval : function() {
			this.start_time = new Date();
			this.updateInterval(0);
		},

		dayView : function(e) {
			console.log("day");
			var day = $(e.target).attr("data-toggle");
			if(day && day != ""){
				//Switch to specifc day
				this.start_time = new Date(day*1000);
			} else if(this.start_time.getEpoch() < new Date().getEpoch() && (this.start_time.getEpoch() + this.total_interval) > new Date().getEpoch()){
				//Switch to today instead of monday if it is this week
				this.start_time = new Date();
			}
			this.total_interval = 24*3600;
			this.intervals = 24;
			
			this.updateInterval(0);
			$(this.el).find("#curr_week").html("Idag");
		},

		weekView : function() {
			this.total_interval = 7*24*3600;
			this.intervals = 7;
			this.updateInterval(0);
			$(this.el).find("#curr_week").html("Denna vecka");
		},

		updateInterval : function(change) {
			var interval_label = "Default";
			
			if(this.total_interval >= 604800) {
				//If the total interval is at least a week long, start on most recent monday
				this.start_time = this.start_time.addSeconds(change).firstDayOfWeek().toStartOfDay();
				interval_label = t.calendar_week + ": " + this.start_time.getWeek();
			} else if(this.total_interval >= 86400) {
				//If total interval is at least a whole day long, start time at 0:00 at the current date
				this.start_time = this.start_time.addSeconds(change).toStartOfDay();
				interval_label = t["weekday"+ this.start_time.getDay()] + " " + this.start_time.getDate() + "/" + this.start_time.getMonth();
			} else {
				//Shorter than a day, start on current time
				this.start_time = this.start_time.addSeconds(change)
			}
			
			$(this.el).find("#current_interval").html(interval_label);
			
			this.fetchBlobs();
		},
		
		fetchBlobs : function(){	
			var that = this;
			var st = this.start_time.getEpoch();
			var end = st + this.total_interval;

			this.blobs.setInterval(st, end);
			this.blobs.fetch({
                success : function() {
                    that.reload();
                },
                error: function(){
                    console.log("Error reports");
                }
           	});
		},

		addOne : function(model) {
            var r = new this.row({
                model : model,
                blobs : this.blobs,
                intervals: this.intervals,
                total_interval : this.total_interval,
                start_time: this.start_time.getEpoch()
            });
            return r.render(this.columns).el;
        },
        
        addAll : function() {
            $(this.el).find("tbody").empty();
            var that = this;
            var rows = [];
            this.collection.each(function(row) {
                rows.push(that.addOne(row));
            });
            
            $(this.el).find("tbody").append(rows);

            var theFrame = $("iframe", parent.document.body);
			theFrame.height($(this.el).outerHeight()+200);

            this.setupList();
        },
        
        filterCollection : function(manager){
        	this.collection.setFilter(usersFilters.ManagerFilter(manager)).fetch();
        },

        selectAll : function(e){
        	$(this.el).find('input.row_select').attr('checked', $(e.target).is(':checked'));
        },

        composeSMS : function(){
        	var popup = new SmsPopup();
        	$(this.el).find("#composer").append(popup.render().el);
        	$(this.el).find("#send_message").button('loading');

        	popup.on('send', this.sendMessage, this);
        	popup.on('cancel', this.cancelMessage, this);

        	var theFrame = $("iframe", parent.document.body);
			theFrame.height($(this.el).outerHeight()+200);
        },

        sendMessage : function(message){
        	console.log(message);
        	$(this.el).find("#send_message").button('reset');
        	$(this.el).find("tbody").children(".plan_row").children('.select').children(':checked').each(function() {
				$(this).parent('.select').trigger("send", message);
			});
        },

        cancelMessage : function(){
        	$(this.el).find("#send_message").button('reset');
        }
    });

    return AvailabilityListView;
});
