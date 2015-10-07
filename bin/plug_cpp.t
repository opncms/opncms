#include <cppcms/url_dispatcher.h>
#include <cppcms/url_mapper.h>
#include <boost/assign/list_of.hpp>

#include <booster/log.h>

#include <opncms/tools.h>
#include <opncms/ioc.h>

#include <opncms/modules/plug.h>
#include <opncms/modules/auth.h>
#include <opncms/modules/cont.h>

#include "${plug}.h"

namespace ${plug}_sign {
	const std::string name = "${plug_name}";
	const std::string shortname = "${plug}";
	const std::string slug = "${plug_slug}";
	const std::string version = "${plug_version}";
}

class ${plug}_impl
{
	friend class ${plug};
	friend class ${plug}_rpc;

public:
	${plug}_impl()
	:auth( &ioc::get<Auth>() ),
	data( &ioc::get<Data>() ),
	view( &ioc::get<View>() )
	{}

private:
	Auth* auth;// = &ioc::get<Auth>();
	Data* data;// = &ioc::get<Data>();
	View* view;// = &ioc::get<View>();
};

namespace content {

${plug}_edit_form::${plug}_edit_form()
{
}

bool ${plug}_edit_form::validate()
{
	if(!form::validate())
		return false;

	return true;
}

}//namespace content

class ${plug}: public Plugin
{
public:

	${plug}(cppcms::service &srv)
	: Plugin::Plugin(srv), impl()
	{
		BOOSTER_LOG(debug, __FUNCTION__) << "name=" << ${plug}_sign::name << ", shortname=" << ${plug}_sign::shortname << ", slug=" << ${plug}_sign::slug << ", version=" << ${plug}_sign::version;
		
		map_.push_back(${plug}_sign::shortname);
		map_.push_back(std::string("/")+${plug}_sign::slug+"/{1}");
		map_.push_back(std::string("/")+${plug}_sign::slug+"(/(.*))?");
		map_.push_back("2");

		dispatcher().assign("", &${plug}::display,this,0);
		mapper().assign("");
	}
	~${plug}(){
		BOOSTER_LOG(debug,__FUNCTION__);
	}

	virtual void prepare(){
		html_menu_.name = name();
		html_left_.name = name();
		html_right_.name = name();
		html_top_.name = name();
		html_bottom_.name = name();
		html_content_.name = name();
	}

	virtual void display(std::string page)
	{
		content::${plug} c;
		BOOSTER_LOG(debug, __FUNCTION__);

		ioc::get<Cont>().post(c);

		//init base content first
		ioc::get<Cont>().init(c);

		c.name = name();

		if(ioc::get<Auth>().auth()) {
			//some functions for authed users
		}

		render(shortname()+"_view", shortname(), c);
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
	bool is_content(){ return false; }
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
	tools::vec_str map_;
	${plug}_impl impl;
};

class ${plug}_rpc : public PluginRpc
{
public:
        ${plug}_rpc(cppcms::service &srv)
        : PluginRpc::PluginRpc(srv), impl()
        {
                BOOSTER_LOG(debug, __FUNCTION__);

                map_.push_back(std::string(${plug}_sign::shortname+"_rpc"));
                map_.push_back(std::string("/")+${plug}_sign::slug+"_rpc/{1}");
                map_.push_back(std::string("/")+${plug}_sign::slug+"_rpc(/(.*))?");
                map_.push_back("2");

                bind("system.listMethods",cppcms::rpc::json_method(&${plug}_rpc::methods,this),method_role);

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
	cppcms::json::array methods_;
	tools::vec_str map_;
	${plug}_impl impl;
};

extern "C" void plugin(Plug &pb)
{
	BOOSTER_LOG(debug, __FUNCTION__) << "Init plugin";
	cppcms::service &s = ioc::get<Plug>().get_srv();
	BOOSTER_LOG(debug, __FUNCTION__) << "Get service successfully";
	ioc::get<Plug>().add(user_sign::shortname,new user(s),new user_rpc(s));
	BOOSTER_LOG(debug, __FUNCTION__) << "Init plugin successfully";

	//add plugin's menu items
	ioc::get<View>().link_add(user_sign::name,std::string("/")+user_sign::slug);
	ioc::get<View>().menu_add("sidebar",user_sign::name);
}
