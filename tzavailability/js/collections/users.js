define([
  'backbone',
  'models/user'
], function(Backbone, User, Filters) {
	
    return Backbone.Collection.extend({
        baseUrl : location.protocol + '//' + location.host + '/api/users?status[10]=10&status[20]=20',
        url: location.protocol + '//' + location.host + '/api/users?status[10]=10&status[20]=20',
        model: User,
        comparator: function(u) {
            var fullname = u.get('fullname');
            if (fullname) {
                return fullname.toLowerCase();
            }
            return u.id;
        },
        
        setFilter : function(filter){
        	return filter(this);
        }
    });
});
