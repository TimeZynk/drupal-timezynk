define([
    'jquery',
    'underscore',
    'backbone',
    'template_views/tzview',
    'template_views/list_view',
    'template_views/list_row',
    'template_views/edit_view',
    'views_admin/add_config_modal',
    'text!templates/company.html'
], function($, _, Backbone, TzView, ListView, ListRow, EditView, AddConfigModal, template) {

    /*
     * Configuration row
     */
    var ConfigRow = ListRow.extend({
        events: {
            "click td" : "editConfig"
        },
        render : function(cols) {
            this.columns = cols;
            var that = this;

            _.each(this.columns, function(col) {
                var link = that.make("a",{
                    "name" : that.model.get("id")
                }, that.model.get(col.field))
                var td = that.make("td", {
                    "class" : col.title
                }, link);
                $(that.el).append(td);
            });

            return this;
        },

        editConfig: function(){
            this.configModel = this.fetchData(this.model.get("config-url"));
            this.configModel.bind('change', this.getFields, this);
        },

        getFields : function(){
            this.backendModel = this.fetchData(this.model.get("backend-url"));
            this.backendModel.bind('change', this.renderControls, this);
        },

        renderControls:function(){
            var that = this;
            var input_fields = this.backendModel.get("config-fields");

            //Add current models values
            _.each(input_fields, function(f){
                f.value = that.configModel.get(f.key);
            });

            var edit = new EditView({
                model : this.configModel,
                fields : input_fields
            });
            $(this.el).append(edit.render().el);
        }
    });

    /*
     * Single company view
     */
    var CompanyView = TzView.extend({

        tmpl : _.template(template),
        collection : new Backbone.Collection(),
        events : {
            "click #edit_btn" : "edit"
        },

        initialize: function(){
            var that = this;
            this.model.fetch({
                success:function(){
                    that.collection.url = that.model.get("config-url");
                    that.collection.bind('add', that.renderContent, that);
                    that.model.bind('change', that.renderContent, that);
                    that.renderContent();
                }
            });
        },

        renderContent: function(){
            this.params = {
                data : this.model.attributes
            }
            this.render();
            this.renderConfigs();
        },

        renderConfigs: function() {
            var AddConfig = AddConfigModal.extend({
                company_id : this.model.get("id")
            });

            this.configs = new ListView({
                collection: this.collection,
                columns: [{title: t.backend_label, field:"backend-id"}, {title: t.backend_config_label, field:"config-id"}],
                row: ConfigRow,
                add_module: AddConfig
            });

            $(this.el).find("#config_list").append(this.configs.render().el);

            //Added configs has to be manually re-fetched
            this.configs.bind('item_added', this.updateList, this);

            return this;
        },

        updateList : function(){
            this.collection.fetch();
        },

        edit : function(){
            var that = this;
            var edit = new EditView({
                //title:"Edit company",
                model:this.model,
                fields: [
                    {
                        key : "name",
                        type : "input",
                        description: "Name",
                        value: that.model.get("name")
                    },
                    {
                        key : "vat-id",
                        type : "input",
                        description : "VAT",
                        value: that.model.get("vat-id")
                    }
                ]
            });
            $(this.el).append(edit.render().el);
        }
    });

    return CompanyView;
});
