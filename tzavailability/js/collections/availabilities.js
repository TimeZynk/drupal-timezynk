define([
  'backbone',
  'models/availability'
], function(Backbone, Availability) {
    return Backbone.Collection.extend({
        url: "",
        model: Availability,
        
        setUrl : function(newUrl){
        	this.baseUrl = location.origin + '/api/availabilities';
        	this.url = this.baseUrl + newUrl;
        	return this.url;
        }
    });
});
