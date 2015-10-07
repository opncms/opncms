////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2013-2015 Vladimir Yakunin (kpeo) <opncms@gmail.com>
//
//  The redistribution terms are provided in the COPYRIGHT.txt file
//  that must be distributed with this source code.
//
////////////////////////////////////////////////////////////////////////////

#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <boost/assign/list_of.hpp>

#include <booster/log.h>

#include <opncms/tools.h>
#include <opncms/ioc.h>

#include <opncms/module/plugin.h>
#include <opncms/module/auth.h>
#include <opncms/module/view.h>

#include "admin.h"

namespace admin_sign {
	const std::string name = "Administrator Panel";
	const std::string shortname = "admin";
	const std::string slug = "admin";
	const std::string version = "0.0.1";
	const std::string api_version = "0.0.1";
}

/*
namespace mod
{
	Auth* auth = &ioc::get<Auth>();
	Data* data = &ioc::get<Data>();
	View* view = &ioc::get<View>();
}
*/
namespace content {

}//namespace content

class admin_impl
{
	friend class admin;
	friend class admin_rpc;

public:
	admin_impl()
	:auth( &ioc::get<Auth>() ),
	data( &ioc::get<Data>() ),
	view( &ioc::get<View>() )
	{}

private:
	Auth* auth;// = &ioc::get<Auth>();
	Data* data;// = &ioc::get<Data>();
	View* view;// = &ioc::get<View>();
};

class admin: public Plugin
{
public:
	admin(cppcms::service &srv)
	: Plugin::Plugin(srv), impl()
	{
		BOOSTER_LOG(debug, __FUNCTION__) << "name=" << admin_sign::name << ", shortname=" << admin_sign::shortname << ", slug=" << admin_sign::slug << ", version=" << admin_sign::version;

		map_.push_back(admin_sign::shortname);
		map_.push_back(std::string("/")+admin_sign::slug+"/{1}");
		map_.push_back(std::string("/")+admin_sign::slug+"(/(.*))?");
		map_.push_back("2");

		dispatcher().assign("/?", &admin::display,this,0);
		mapper().assign("");
/*
		dispatcher().assign("/(\\w+)/?$", &admin::display,this,1);
		mapper().assign("/{1}");
*/
		dispatcher().assign("/config/?",&admin::config,this);
		mapper().assign("config","/config");

		dispatcher().assign("/pages/?",&admin::pages,this);
		mapper().assign("pages","/pages");
	}
	~admin(){
		BOOSTER_LOG(debug,__FUNCTION__);
	}

	virtual void prepare(){
		content_.name = name();
	}
	
	virtual void display(std::string page)
	{
		BOOSTER_LOG(debug, __FUNCTION__) << "display " << page;
		
		if(page == "config")
		{
			content::admin_config c;
			impl.view->post(c);
			impl.view->init(c);
			c.name = name();
			render(shortname()+"_view", "admin_config", c);
		}
		else
		{
			content::admin c;
			impl.view->post(c);
			//init base content first
			impl.view->init(c);
			c.name = name();

			if(impl.auth->auth()) {
				//some functions for authed users
			}
			render(shortname()+"_view", shortname(), c);
		}
	}

	virtual void config()
	{
		content::admin_config c;
		BOOSTER_LOG(debug, __FUNCTION__);

		impl.view->init(c);
		impl.view->post(c);

		c.name = name();
		cppcms::json::value v;
		impl.data->driver("file").get(v,"plugins.js");
		BOOSTER_LOG(debug, __FUNCTION__) << tools::json_to_string(v);
		tools::json_to_map(c.config, v);
		
		if(impl.auth->auth())
		{
			//some functions for authed users
		}
		render(shortname()+"_view", "admin_config", c);
	}

	virtual void pages()
	{
		content::admin_pages c;
		BOOSTER_LOG(debug, __FUNCTION__);

		impl.view->init(c);
		impl.view->post(c);

		c.name = name();
		std::string p = impl.data->driver().get("pages");
		cppcms::json::value v = tools::string_to_json(p);
		if(!v.is_null() && !v.is_undefined())
		{
			for(cppcms::json::object::const_iterator it = v.object().begin(); it != v.object().end(); ++it)
			{
				c.pages.push_back(it->first.str());
			}
		}
		if(impl.auth->auth())
		{
			//some functions for authed users
		}
		render(shortname()+"_view", "admin_pages", c);
	}

	//Extend base view
	virtual bool is_css(){ return false; }
	virtual bool is_js_head(){ return false; }
	virtual bool is_js_foot(){ return false; }
	virtual bool is_menu(){ return true; }
	virtual bool is_left(){ return false; }
	virtual bool is_right(){ return false; }
	virtual bool is_top(){ return false; }
	virtual bool is_bottom(){ return false; }
	virtual bool is_content(){ return false; }
	virtual cppcms::base_content& html_css() { return content_; }
	virtual cppcms::base_content& html_js_head() { return content_; }
	virtual cppcms::base_content& html_js_foot() { return content_; }
	virtual cppcms::base_content& html_menu() { return content_; }
	virtual cppcms::base_content& html_left() { return content_; }
	virtual cppcms::base_content& html_right() { return content_; }
	virtual cppcms::base_content& html_top() { return content_; }
	virtual cppcms::base_content& html_bottom() { return content_; }
	virtual cppcms::base_content& html_content() { return content_; }
	
	virtual cppcms::application& get(){
		return *this;
	}
	
	virtual std::string skin(){
		return admin_sign::slug + "_view";
	}

	virtual std::string view(const std::string& s){
		return admin_sign::slug + "_" + s;
	}

	virtual const std::string& name(){
		return admin_sign::name;
	}
	virtual const std::string& shortname(){
		return admin_sign::shortname;
	}
	virtual const std::string& slug(){
		return admin_sign::slug;
	}
	virtual const std::string& version(){
		return admin_sign::version;
	}
	virtual tools::vec_str& map(){
		return map_;
	}
private:
	admin_impl impl;
	tools::vec_str map_;
	content::admin content_;
};

class admin_rpc : public PluginRpc
{
public:
	admin_rpc(cppcms::service &srv)
	: PluginRpc::PluginRpc(srv), impl()
	{
		BOOSTER_LOG(debug, __FUNCTION__);

		map_.push_back(std::string(admin_sign::shortname+"_rpc"));
		map_.push_back(std::string("/")+admin_sign::slug+"_rpc/{1}");
		map_.push_back(std::string("/")+admin_sign::slug+"_rpc(/(.*))?");
		map_.push_back("2");

		bind("system.listMethods",cppcms::rpc::json_method(&admin_rpc::methods,this),method_role);

		methods_ = boost::assign::list_of ("system.listMethods");
		BOOST_ASSERT( methods_.size() == 1 );
	}

	virtual void methods()
	{
		BOOSTER_LOG(debug, __FUNCTION__);
		return_result(methods_);
	}

	virtual tools::vec_str& map(){
		return map_;
	}

private:
	admin_impl impl;
	tools::vec_str map_;
	cppcms::json::array methods_;
};

extern "C" void plugin()
{
	cppcms::service &s = ioc::get<View>().service();
	if(ioc::get<Plug>().get(admin_sign::shortname) == NULL && ioc::get<Plug>().get_rpc(admin_sign::shortname) == NULL)
	{
		BOOSTER_LOG(debug, __FUNCTION__) << "Try to add the plugin " << admin_sign::name;
		ioc::get<Plug>().add(admin_sign::shortname,new admin(s),new admin_rpc(s),admin_sign::api_version);
		BOOSTER_LOG(debug, __FUNCTION__) << "Add plugin's menu items";
		ioc::get<View>().link_add(admin_sign::name,std::string("/")+admin_sign::slug);
		ioc::get<View>().menu_add("sidebar",admin_sign::name);
	}
}
