define([
    'jquery',
    'underscore',
    'backbone',
    'lib/bootstrap-modal',
    'template_views/tzview',
    'text!templates/add_modal.html'
], function($, _, Backbone, Modal, TzView, template) {
    /*
     * Generic add-list-item view
     */
    return TzView.extend({
        tmpl: _.template(template),
        className: "modal",
        collection : "",

        events : {
            "click #save_btn" : "save_item"
        },

        rendered: function (){
            //$(this.el).find("#module_header").html(this.title);
            $(this.el).find("#input_fields").html(this.renderFields(this.fields));
            $(this.el).modal({backdrop:"static"});
            this.bind('save-successful', this.close, this);
        },

        renderFields : function(fields){
            var that = this;
            var el = this.make("form",{"class":"form form-inline"});

            _.each(fields, function(field){
                var defaults = {
                    key : "", //Field identifier
                    type : "input", //HTML element
                    description : "", //Label
                    placeholder : "", //Backround placeholder text
                    value : "", //Field content
                };

                $.extend(defaults, field);

                var l = that.make("label", {}, defaults.description);
                var field_class = "model_param";

                if(defaults.mandatory == true){
                    field_class += " required";
                };
                // The field
                var f = that.make(defaults.type,{
                    "value":defaults.value,
                    "name": defaults.key,
                    "placeholder":defaults.placeholder,
                    "class" : field_class
                });

                $(el).append(l);
                $(el).append(f);
                $(el).append("<br />");
            });
            return el;
        },

        save_item : function() {
            var that = this;
            var save_params = $.extend({}, this.save_params);

            //Find desired fields and save values
            var val = $(this.el).find('.model_param');
            val.each(function(i, el) {
                if ($(el).length > 0) { // Make sure the DOM exists before we add it
                    save_params[$(el).attr("name")] = $(el).val();
                }
            });
            
            this.save(save_params);
        },

        save : function(save_params){
            var that = this;
            this.collection.create(save_params, {
                wait : true,
                success : function(m, msg) {
                    that.trigger("save-successful",[m,msg]);
                    //$(that.el).modal('hide');
                    //that.remove();
                },
                error : function(m, msg) {
                    console.log("error: " + msg);
                    that.displayMessage(msg.statusText, "alert-error");
                }
            });
        },
        
        close : function(){
        	$(this.el).modal('hide');
            this.remove();
        }
    });
});
