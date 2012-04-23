define(function () {
	/*
	 * Filters for users collection
	 */
    var filters = {
    	ManagerFilter : function (manager) {
			return function(collection) {
				collection.url += '&manager=' + encodeURI(manager);
				return collection;
			};
		}
    };

    return filters;
});


