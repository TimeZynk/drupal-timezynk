define([
  'backbone',
  'models/report'
], function(Backbone, Report) {
    return Backbone.Collection.extend({
        url: "",
        model: Report,
        
        setUrl : function(newUrl){
        	this.baseUrl = location.origin + '/api/reports';
        	this.url = this.baseUrl + newUrl;
        	return this.url;
        }
    });
});
