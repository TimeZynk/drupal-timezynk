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
    'template_views/list_view',
    'views/av_list_row',
    'views/sms_popup',
    'text!templates/availability.html',
    'i18n!nls/tzcontrol'
], function($, _, Backbone, Tooltip, Popover, Buttons, Users, Blobs, User, ListView, AvListRow, SmsPopup, template, t) {
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
        	"click #btn_day" : "dayView",
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

            var theFrame = $("iframe", parent.document.body);
			theFrame.height(400);

            this.collection.bind('add', this.addOne, this);
            this.collection.bind('add', this.setupList, this);
            this.collection.bind('reset', this.addAll, this);
            this.collection.bind('reset', this.setupList, this);

            this.blobs = new Blobs();
			this.resetInterval();

            return this;
        },

        renderHead : function(){
        	var that = this;
        	var even = false;
        	$(that.el).find("thead tr").empty();

        	_.each(this.columns, function(col) {
                var th = that.make("th", {
                    "class" : "sort",
                    "data-sort" : col.title
                }, col.title);
                $(that.el).find("thead tr").append(th);
            });

            var checkbox = this.make("input", {
                "type" : "checkbox",
                "class" : "row_select",
                "name" : "select-all"
            });

            var th_box = this.make("th", {}, checkbox);

            var th = this.make("th", {
                "class" : "plan_intervals"
            });
            var blobs = this.make("div",{"class":"slot_container"});
            var date = this.start_time.getEpoch();
            var incr = this.total_interval/this.intervals;

        	for(var i=0; i<this.intervals; i++){

        		var rend = that.getIntervalLabel(date);

        		if(even){
        			var sp = that.make("div", {
	                    "class" : "plan_interval"
	                },rend);
        		} else{
        			var sp = that.make("div", {
	                    "class" : "plan_interval_odd"
	                },rend);
        		}

        		even = !even;
        		date += incr;

                $(sp).css({
                	width: 100/this.intervals + "%"
                });

                $(blobs).append(sp);
        	};

        	$(th).append(blobs);
        	$(that.el).find("thead tr").prepend(th_box);
        	$(that.el).find("thead tr").append(th);
        },

        reload:function(){
        	var that = this;
        	this.renderHead();
            this.collection.fetch({
                success : function() {
                    that.addAll();
                },
                error: function(){
                    console.log("Error list");
                }
            });
        },

		getIntervalLabel : function(date){
			var label = new Date(date*1000);
			if(this.total_interval > 86400){
				label = label.getDate() + "/" + label.getMonth();
			} else {
				label = label.getHours() + ":00";
			}
			return label;
		},

		nextInterval : function() {
			this.renderInterval(this.total_interval);
		},

		prevInterval : function() {
			this.renderInterval(-this.total_interval);
		},

		resetInterval : function() {
			this.start_time = new Date();
			this.renderInterval(0);
		},

		dayView : function() {
			this.total_interval = 24*3600;
			this.intervals = 24;
			this.renderInterval(0);
			$(this.el).find("#curr_week").html("Idag");
		},

		weekView : function() {
			this.total_interval = 7*24*3600;
			this.intervals = 7;
			this.renderInterval(0);
			$(this.el).find("#curr_week").html("Denna vecka");
		},

		renderInterval : function(change) {
			var that = this;

			var interval_label = "Default";
			if(this.total_interval >= 604800) {
				//Longer than a week, start on monday
				this.start_time = this.start_time.addSeconds(change).firstDayOfWeek().toStartOfDay();
				interval_label = t.calendar_week + ": " + this.start_time.getWeek();
			} else if(this.total_interval >= 86400) {
				//Longer than one day, start at 00:00
				this.start_time = this.start_time.addSeconds(change).toStartOfDay();
				interval_label = this.start_time.getDate() + "/" + this.start_time.getMonth();
			} else {
				//Shorter than a day, don´t adjust starttime...
				this.start_time = this.start_time.addSeconds(change)
			}
			$(this.el).find("#current_interval").html(interval_label);

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
            $(this.el).find("tbody").append(r.render(this.columns).el);

            var theFrame = $("iframe", parent.document.body);
			theFrame.height($(this.el).outerHeight()+200);
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
