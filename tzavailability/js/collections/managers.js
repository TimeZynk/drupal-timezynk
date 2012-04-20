define([
  'backbone',
  'models/availability'
], function(Backbone, Availability) {
    return Backbone.Collection.extend({
        url: "",

        setUrl : function(newUrl){
        	this.baseUrl = location.protocol + '//' + location.host + '/api/managers';
        	this.url = this.baseUrl + newUrl;
        	return this.url;
        }
    });
});
