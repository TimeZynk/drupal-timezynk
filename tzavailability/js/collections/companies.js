define([
  'backbone',
  'models/company'
], function(Backbone, Company) {
    return Backbone.Collection.extend({
        url: "/companies",
        model: Company
    });
});
