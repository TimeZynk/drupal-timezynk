define([
    'jquery',
    'underscore',
    'backbone',
    'template_views/add_modal',
], function($, _, Backbone, AddModal) {
    /*
     * Edit module
     */
    return AddModal.extend({
        save : function(save_params){
            var that = this;
            this.model.save(save_params,{
                wait:true,
                success : function(m, msg) {
                    $(that.el).modal('hide');
                    $(".modal-backdrop").remove();
                    that.remove();
                    that.trigger("save-successful");
                },
                error : function(m, msg) {
                    console.log("error");
                    that.displayMessage(msg.statusText, "alert-error");
                }
            });
        }
    });
});
