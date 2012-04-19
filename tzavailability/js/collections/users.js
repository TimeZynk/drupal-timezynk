define([
  'backbone',
  'models/user'
], function(Backbone, User) {
    return Backbone.Collection.extend({
        url: location.origin + '/api/users?status[10]=10&status[20]=20',
        model: User,
        comparator: function(u) {
            var fullname = u.get('fullname');
            if (fullname) {
                return fullname.toLowerCase();
            }
            return u.id;
        }
    });
});
