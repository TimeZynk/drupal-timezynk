define([
  'underscore',
  'backbone',
  'models/blob',
  'models/availability',
  'collections/availabilities',
  'models/report',
  'collections/reports'
], function(_, Backbone, Blob, Availability, Availabilities, Report, Reports) {
	
	function blobFromReport(r) {
		var reported = (parseInt(r.flags,10) > 1);
		return new Blob({
			id: 'report-' + r.nid,
			type: 'report',
			user_id: r.assignedto,
			start_time: r.begintime,
			end_time: r.endtime,
			title: (reported ? t.user_reported : t.user_booked) + ': ' + r.title,
			reported: reported
		});
	}
	
	function blobFromAvailability(a) {
		var available = (a.availability_type === "0");
		return new Blob({
			id: 'availability-' + a.id,
			type: 'availability',
			user_id: a.uid,
			start_time: a.start_time,
			end_time: a.end_time,
			title: available ? t.user_available : t.user_unavailable,
			available: available
		});
	}
	
    return Backbone.Collection.extend({
        url: "",
        model: Blob,
        
        initialize: function() {
        	this.availabilities = new Availabilities();
        	this.reports = new Reports();
        },
        
        setInterval: function(from, to) {
        	this.availabilities.setUrl("?from=" + from +"&to=" + to);
			this.reports.setUrl("?from=" + from +"&to=" + to);
        },
        
        fetch: function(options) {
        	var that = this;
        	this.reset([], {silent: true});
        	this.availabilities.fetch({
        		success: function(av_data) {
        			that.reports.fetch({
        				success: function(r_data) {
        					r_data.each(function(r) {
        						that.add(blobFromReport(r.toJSON()), {silent: true});
        					});
        					that.trigger('reset');
        					options.success(that);
        				},
        				error: options.error
        			});
        			av_data.each(function(av) {
        				that.add(blobFromAvailability(av.toJSON()), {silent: true});
        			});
        		},
        		error: options.error
        	})
        },
        
        setUrl : function(newUrl){
        	this.baseUrl = location.origin + '/api/availabilities';
        	this.url = this.baseUrl + newUrl;
        	return this.url;
        }
    });
});
