define([
    'jquery',
    'underscore',
    'backbone',
    'list',
    'template_views/tzview',
    'template_views/add_modal',
    'template_views/list_row',
    'text!templates/list.html'
], function($, _, Backbone, List, TzView, AddModal, ListRow, template) {

    /*
     * Generic list
     * Dependencies : TZ_view, Views.AddModal, Views.ListRow
     */
    return TzView.extend({
        tmpl: _.template(template),
        collection : null,
        row : ListRow,
        add_module: AddModal,
        columns : [],

        events : {
            "click #add_btn" : "addItem"
        },

        initialize : function(obj) {
            _.bindAll(this, 'render', 'addOne', 'addAll', 'addItem');
            var that = this;

            $.extend(this, obj);

            this.collection.bind('add', this.addOne, this);
            this.collection.bind('add', this.setupList, this);
            this.collection.bind('reset', this.addAll, this);
            this.collection.bind('reset', this.setupList, this);

            this.collection.fetch({
                success : function() {
                    //that.addAll();
                },
                error: function(){
                    console.log("Error list");
                }
            });
        },
        render : function() {
            $(this.el).html(this.tmpl());

            var that = this;

            //@TODO: We should add field "sortable" to cols to disable sorting for certain columns.
            _.each(this.columns, function(col) {
                var th = that.make("th", {
                    "class" : "sort",
                    "data-sort" : col.title
                }, col.title);
                $(that.el).find("thead tr").append(th);
            });
            return this;
        },
        addOne : function(row) {
            var r = new this.row({
                model : row,
                className : ""
            });
            $(this.el).find("tbody").append(r.render(this.columns).el);
        },
        addAll : function() {
            $(this.el).find("tbody").empty();
            var that = this;
            this.collection.each(function(row) {
                that.addOne(row);
            });

            this.setupList();
        },
        setupList : function() {

            var cols = [];

            _.each(this.columns, function(col) {
                cols.push(col.title);
            });
            var options = {
                valueNames : cols
            };

            this.the_list = new List(this.el, options);
        },
        addItem : function() {

            var a = new this.add_module({
                collection: this.collection
            });

            $(this.el).append(a.render());

            a.bind('save-successful', this.successMessage, this);
        },
        successMessage: function(msg){
            this.displayMessage("List item created!", "alert-success");
            this.trigger("item_added");
        }
    });
});
