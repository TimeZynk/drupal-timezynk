define([
    'jquery',
    'underscore',
    'backbone',
    'collections/companies',
    'template_views/tzview',
    'template_views/add_modal',
    'template_views/list_view',
    'template_views/list_row',
    'text!templates/companies.html'
], function($, _, Backbone, Companies, TzView, AddModal, ListView, ListRow, template) {
    /*
     * Add new company
     */
    var AddCompany = AddModal.extend({
        rendered: function(){
            var fields = [
                {
                    key : "name",
                    type : "input",
                    description: t.companies_name_column
                },
                {
                    key : "vat-id",
                    type : "input",
                    description : t.companies_vat_column
                }
            ]
            $(this.el).find("#input_fields").html(this.renderFields(fields));
            $(this.el).modal();
        }
    });

    /*
     * Company row
     */
    var CompanyRow = ListRow.extend({
        render : function(cols) {
            this.columns = cols;
            var that = this;

            _.each(this.columns, function(col) {
                var link = that.make("a",{
                    "href" : "#company/" + that.model.get("id") + "/users"
                }, that.model.get(col.field))
                var td = that.make("td", {
                    "class" : col.title
                }, link);
                $(that.el).append(td);
            });
            return this;
        }
    });

    /*
     * Companies view
     */
    var CompaniesView = TzView.extend({
        tmpl: _.template(template),

        render: function() {
            this.content = this.tmpl(this.params);
            $(this.el).append(this.content);

            var table = new ListView({
                collection: new Companies(),
                columns: [{title: t.companies_name_column, field:"name"}, {title: t.companies_vat_column, field:"vat-id"}],
                row: CompanyRow,
                add_module : AddCompany
            });

            $(this.el).find("#companies_list").append(table.render().el);

            return this;
        }
    });

    return CompaniesView;
});
