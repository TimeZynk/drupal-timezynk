define([
  'backbone',
  'models/user'
], function(Backbone, User) {
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
        
        getManager : function(manager){
        	var that = this;
        	if(manager != 'reset'){
        		console.log("chager");
        		this.url = this.baseUrl + "&manager=" + manager;
        	} else{
        		//Reset list
        		console.log("reset");
        		this.url = this.baseUrl;
        	}
        	console.log(this.url);
        	this.fetch({
        		success: function(){
        			that.trigger("reset");
        		},
        		error : function(){
        			console.log("Error fetching users.")
        		}
        	});
        }
    });
});
