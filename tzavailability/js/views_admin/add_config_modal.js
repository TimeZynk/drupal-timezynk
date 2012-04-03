define([
    'jquery',
    'underscore',
    'backbone',
    'template_views/add_modal'
], function($, _, Backbone, AddModal) {
    return AddModal.extend({
        events:{
            "change #backend_dropdown" : "updateOptions",
            "click #save_btn" : "save_item"
        },

        rendered : function(){
            var that = this;

            // Get list of available backends
            this.backends = new Backbone.Collection();
            this.backends.url = "/backends";
            this.backends.fetch({
                success: function(){
                    that.renderBackends();
                },
                error: function(m, msg){
                    that.failedSave(msg);
                }
            });
            $(this.el).modal();
        },

        renderBackends: function(){
            var that = this;
            var select = this.make("select", {"id":"backend_dropdown"});
            $(select).append(that.make("option", {}, "..."));

            this.backends.each(function(b){
                var option = that.make("option", {"value":b.get("url")}, b.get("id"));
                $(select).append(option);
            });

            $(that.el).find("#msg_field").after(select);
        },

        updateOptions : function(e){
            this.save_params = {};
            this.clearMessages();
            var path = $(e.currentTarget).attr("value");
            if(path != "..." && path !=""){
                var that = this;
                this.model = new Backbone.Model({});
                this.model.url = path;
                this.model.fetch({
                    success: function(){
                        that.renderControls();
                    },
                    error: function(m, msg){
                        that.failedSave(msg);
                    }
                });
            } else {
                $(this.el).find("#input_fields").empty();
            }
        },

        renderControls: function(){
            var that = this;
            this.collection = new Backbone.Collection();
            this.collection.url = this.model.get("config-url");
            this.save_params = {"company-id": this.company_id};
            $(this.el).find("#input_fields").html(this.renderFields(this.model.get("config-fields")));
        }
    });
});
