define([
  'jquery',
  'underscore',
  'backbone',
  'models/session',
  'models/company',
  'models/user',
  'views_admin/navbar_view',
  'views_admin/login_view',
  'views_admin/company_view',
  'views_admin/companies_view',
  'views_admin/company_users_view',
  'views_admin/users_all_view',
  'views_admin/user_single_view',
  'views_admin/user_downloads_view',
], function (
	$, 
	_, 
	Backbone, 
	Session, 
	Company,
	User,
	NavBarView, 
	LoginView, 
	CompanyView, 
	CompaniesView, 
	UsersView, 
	AllUsersView, 
	SingleUserView,
	UserDownloadsView
	) {
    var session = new Session({}),
        Routes = Backbone.Router.extend({
        routes: {
            "*actions" : "login"
        },

        initialize: function() {

            var that = this;

            //Create navigation bar and bind to router changes
            var navbar = new NavBarView({el: $('#navbar')});
            this.bind('all', function(event) {
                navbar.routeEvent(event);
            });

            this.validateLogin();
        },

        validateLogin:function(){

            var that = this;

            //Check if user is logged in
            console.log("Attempt login");

            session.fetch({
                success:function(s){
                    console.log("User logged in");
                    that.updateRoutes();
                    //that.navigate(that.defaultStart, {trigger: true, replace:true});
                    Backbone.history.start();
                },
                error:function(){
                    //User not logged in, reroute to login-page.
                    console.log("User not logged in");
                    Backbone.history.start();
                    window.location.hash = "login";
                }
            });
        },

        updateRoutes:function(){
            if(session.get("admin")) {
                this.route("*actions", "companies");

                this.route("companies", "companies");
                this.route("company/:company_id", "company");
                this.route("company/:company_id/user/:user_id/downloads", "single_user_downloads");
                this.route("company/:company_id/user/:user_id", "single_user");
                this.route("company/:company_id/users", "company_users");
                
                this.route("users", "all_users");

                this.route("logout", "logout");
            } else if(session.get("user")) {
                this.route("logout", "logout");
            } else {
                // Anonymous routes?
            }
        },

        login: function() {
            this.updateRoutes();
            this.navigate("login");
            var v = new LoginView();
            $('#main').html(v.render().el);
        },

        logout: function() {
            this.route("login", "login");
            session.destroy();
            this.navigate("login", {trigger: true, replace:true});
            window.location.reload();
        },

        companies: function(){
            this.navigate("companies", {replace:true});
            this.current_view = new CompaniesView();
            $('#main').html(this.current_view.render().el);
        },

        company_users: function(company_id){
            this.navigate("company/" + company_id + "/users", {replace:true});
            this.current_view = new UsersView({
                company_id: company_id
            });
            $('#main').html(this.current_view.render().el);
        },

        company: function(company_id){
            this.navigate("company/" + company_id, {replace:true});

            this.current_view = new CompanyView({
                model: new Company({
                    id : company_id
                })
            });
            $('#main').html(this.current_view.render().el);
        },
        
        all_users: function(){
            this.navigate("users", {replace:true});
            this.current_view = new AllUsersView();
            $('#main').html(this.current_view.render().el);
        },
        
        single_user: function(company_id, user_id){
            this.navigate("company/" + company_id + "/user/" + user_id, {replace:true});
            this.current_view = new SingleUserView({
            	model: new User({
                    id : user_id
                })
            });
            $('#main').html(this.current_view.render().el);
        },
        
        single_user_downloads: function(company_id, user_id){
            this.navigate("company/" + company_id + "/user/" + user_id + "/downloads", {replace:true});
            this.current_view = new UserDownloadsView({
            	model: new User({
                    id : user_id
                })
            });
            $('#main').html(this.current_view.render().el);
        }
    });

    return {
        initialize: function() {
            return new Routes();
        }
    };
});
