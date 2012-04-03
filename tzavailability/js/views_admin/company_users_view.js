define([
    'jquery',
    'underscore',
    'backbone',
    'collections/users',
    'models/user',
    'template_views/tzview',
    'template_views/add_modal',
    'template_views/list_view',
    'template_views/list_row',
    'text!templates/users.html'
], function($, _, Backbone, Users, User, TzView, AddModal, ListView, ListRow, template) {

	/*
     * Add user row
     */
    var AddUserRow = ListRow.extend({

    	className: "new_user_row",

        render : function(fields) {
            this.columns = fields;
            var that = this;

            _.each(this.columns, function(field) {
                that.renderField(field);
            });

            return this;
        },

        renderField : function(field){

	        var defaults = {
	            key : "", //Field identifier
	            type : "input", //HTML element
	            description : "", //Label
	            placeholder : "", //Backround placeholder text
	            value : "", //Field content
	        };

	        $.extend(defaults, field);

	        var form = this.make("form",{"class":"form form-inline"});
	        var l = this.make("label", {}, defaults.description);
	        var field_class = "model_param";

	        if(defaults.mandatory == true){
	            field_class += " required";
	        };

	        // The field
	        var f = this.make(defaults.type,{
	            "value":defaults.value,
	            "name": defaults.key,
	            "placeholder":defaults.placeholder,
	            "class" : field_class
	        });

	        $(form).append(l);
	        $(form).append(f);

	        var td = this.make("td",{},form);
	        $(this.el).append(td);
        },

        save_user :function(){
        	var that = this;
        	var save_params = $.extend({}, this.save_params);
        	var ignore = true; //If all fields are blank we just ignore the row.

        	var val = $(this.el).find('.model_param');
            val.each(function(i, el) {
                if ($(el).val() != "") { // Make sure the DOM exists before we add it
                   save_params[$(el).attr("name")] = $(el).val();
                   ignore = false;
                }
            });

            user = {
                name: save_params['name'],
                mobile: save_params['mobile'],
                backend: {
                    id: $("select[name=backend]").val().split(':')[0],
                    "config-id": $("select[name=backend]").val().split(':')[1],
                    username: save_params['username']
                }
            };

            if(!ignore){
	            $(this.el).addClass("pending");
	            this.model.save(user, {
	                wait : true,
	                success : function(m, msg) {
	                	console.log("one saved");
	                	that.send_install_sms([m,msg]);
	                },
	                error : function(m, msg) {
	                	console.log("one save failed saved")
	                    $(that.el).removeClass("pending");
	                    $(that.el).addClass("error");
	                }
             	});
            } else {
            	this.remove();
            	this.model.destroy();
            }
        },

        send_install_sms: function(params) {
        	var user = params[0];

        	var that = this;
        	sms = new Backbone.Model();
        	message = _.template("Välkommen till mobil tidrapportering! " +
                    "Installera appen genom att klicka på länken <%= url %> och logga " +
                    "sedan in med \"<%= username %>\" som användarnamn och med lösenordet som " +
                    "du har fått från din chef.");

            sms.url = '/sms';

            sms.save({
                from: "TimeZynk",
                to: user.get('int-mobile'),
                message: message({
                    url: "http://" + location.host + user.get('download-link'),
                    username: user.get('login-names')[0]
                })
            }, {
                success: function(m, msg) {
                	console.log("sms sent");
                    that.trigger("save-successful",[m,msg]);
                    that.remove();
                }
            });
        }
    });

    /*
     * Add new user
     */
    function buildAddUser(company_id) {
        return AddModal.extend({

        	events:{
        		"click .add_user_row td" : "addUserFieldsRow",
        		"click #save_btn" : "save_item"
        	},

            rendered: function() {

            	$(this.el).css({
            		width:"70%",
            		marginLeft:"-35%",
            		maxHeight:"80%",
            		top:"40%"
            	});

                var fields = [
                    {
                        key : "backend",
                        type : "select",
                        description : t.user_backend_column
                    }
                ];

                $(this.el).find("#input_fields").html(this.renderFields(fields));

                // Add new user action-row.
                var addRow = this.make("tr",{"class":"add_user_row"},this.make("td",{"colspan":"4"},"+ Add row"));

                // New users table
                var table = this.make("table",{"id":"users_table", "class":"table"},this.make("tbody",{},addRow));
                $(this.el).find("#input_fields").append(table);

                // Backend-select
                var configs = new Backbone.Collection();
                configs.url = '/companies/' + company_id + '/configs';
                configs.fetch({
                    success: function(configs) {
                        configs.each(function(config) {
                            $('select').append($('<option value="' + config.get('backend-id') + ':' +
                                config.get('config-id') + '">' + config.get('backend-id') +
                                " - " + config.get('config-id') + "</option>"));
                        });
                    }
                });

                this.addUserFieldsRow();
                $(this.el).modal({backdrop:"static"});
            },

            addUserFieldsRow : function(){
	            var that = this;

	            var fields = [
                    {
                        key : "name",
                        type : "input",
                        description: t.user_name_column
                    },
                    {
                        key : "mobile",
                        type : "input",
                        description : t.user_mobile_column
                    },
                    {
                        key : "email",
                        type : "input",
                        description : t.user_email_column
                    },
                    {
                        key : "username",
                        type : "input",
                        description : t.user_username_label
                    }
                ];

                this.collection.add(row);

				var row = new User({
					name : "undefined",
					mobile : "undefined",
					"company-id" : company_id,
					backend : {
						id : "undefined",
						"config-id" : "undefined",
						username : "undefined"
					}
				});

	            var tr = new AddUserRow({
	                model : row,
	                className : ""
	            });

	            $(this.el).find("#users_table .add_user_row").before(tr.render(fields).el);

	            tr.bind('save-successful', this.rowSaved, this);
	            this.bind('save-users', tr.save_user, tr);
	        },

            save_item : function() {
            	this.trigger("save-users");
	        },

	        rowSaved : function(row){
	        	var rows = $(this.el).find('.new_user_row');
	        	if(rows.length <= 1){
	        		this.trigger('save-successful');
	        		this.collection.fetch();
	        		this.close();
	        	}
	        }
        });
    }

    /*
     * Users row
     */
    var UserRow = ListRow.extend({
        render : function(cols) {
            this.columns = cols;
            var that = this;

            _.each(this.columns, function(col) {
                var link = that.make("a",{
                    "href" : "#company/" + that.model.get("company-id") + "/user/" + that.model.get("id")
                }, that.format_value(col))
                var td = that.make("td", {
                    "class" : col.title
                }, link);
                $(that.el).append(td);
            });
            return this;
        }
    });

    /*
     * Users view
     */
    var UsersView = TzView.extend({
        tmpl: _.template(template),

        render: function() {
            var that = this,
            collection = new Users();

            collection.url = '/companies/' + that.company_id + '/users';
            currentBasePath = '#company/' + that.company_id;

            this.content = this.tmpl(this.params);
            $(this.el).append(this.content);

            var table = new ListView({
                collection: collection,
                columns: [
                    {title: t.user_name_column, field:"name"},
                    {title: t.user_mobile_column, field:"mobile"},
                    {title: t.user_last_login_column, field:"last-login", format: function(v) {
                        if (v) {
                            return (new Date(v * 1000)).toString();
                        } else {
                            return "";
                        }
                    }}
                ],
                row: UserRow,
                add_module : buildAddUser(that.company_id)
            });

            $(this.el).find("#companies_list").append(table.render().el);

            return this;
        }
    });

    return UsersView;
});
