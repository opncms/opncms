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

#include "wiki.h"

#define WIKI_STRUCT_SIZE 8

namespace wiki_sign {
	const std::string name = "Wiki";
	const std::string shortname = "wiki";
	const std::string slug = "w";
	const std::string version = "0.0.1";
	const std::string api_version = "0.0.1";
}

class wiki_impl
{
	friend class wiki;
	friend class wiki_rpc;

public:
	wiki_impl()
	:auth( &ioc::get<Auth>() ),
	data( &ioc::get<Data>() ),
	view( &ioc::get<View>() )
	{}

private:
	Auth* auth;
	Data* data;
	View* view;
};

namespace {
	cppcms::xss::rules const& xss_filter()
	{
		static cppcms::xss::rules r;
		static bool initialized = false;
		if(initialized)
			return r;
		using namespace cppcms::xss;

		r.html(rules::xhtml_input);
		r.add_tag("ol",rules::opening_and_closing);
		r.add_tag("ul",rules::opening_and_closing);
		r.add_tag("li",rules::opening_and_closing);
		r.add_tag("p",rules::opening_and_closing);
		r.add_property("p","style",booster::regex("\\s*text-align\\s*:\\s*(center|left|right)\\s*;?"));
		r.add_tag("b",rules::opening_and_closing);
		r.add_tag("i",rules::opening_and_closing);
		r.add_tag("tt",rules::opening_and_closing);
		r.add_tag("sub",rules::opening_and_closing);
		r.add_tag("sup",rules::opening_and_closing);
		r.add_tag("blockquote",rules::opening_and_closing);
		r.add_tag("strong",rules::opening_and_closing);
		r.add_tag("em",rules::opening_and_closing);
		r.add_tag("h1",rules::opening_and_closing);
		r.add_tag("h2",rules::opening_and_closing);
		r.add_tag("h3",rules::opening_and_closing);
		r.add_tag("h4",rules::opening_and_closing);
		r.add_tag("h5",rules::opening_and_closing);
		r.add_tag("h6",rules::opening_and_closing);
		booster::regex cl_id(".*");
		r.add_property("h1","id",cl_id);
		r.add_property("h2","id",cl_id);
		r.add_property("h3","id",cl_id);
		r.add_property("h4","id",cl_id);
		r.add_property("h5","id",cl_id);
		r.add_property("h6","id",cl_id);
		r.add_tag("span",rules::opening_and_closing);
		r.add_property("span","id",cl_id);
		r.add_tag("code",rules::opening_and_closing);
		r.add_tag("pre",rules::opening_and_closing);
		r.add_property("pre","name",booster::regex("\\w+"));
		r.add_property("pre","class",booster::regex("\\w+"));
		r.add_tag("a",rules::opening_and_closing);
		r.add_uri_property("a","href");
		r.add_tag("hr",rules::stand_alone);
		r.add_tag("br",rules::stand_alone);
		r.add_tag("img",rules::stand_alone);
		r.add_uri_property("img","src");
		r.add_integer_property("img","width");
		r.add_integer_property("img","height");
		r.add_integer_property("img","border");
		r.add_property("img","alt",booster::regex(".*"));
		r.add_tag("table",rules::opening_and_closing);
		r.add_tag("tr",rules::opening_and_closing);
		r.add_tag("th",rules::opening_and_closing);
		r.add_tag("td",rules::opening_and_closing);
		r.add_integer_property("table","cellpadding");
		r.add_integer_property("table","cellspacing");
		r.add_integer_property("table","border");
		r.add_tag("center",rules::opening_and_closing);
		r.add_entity("nbsp");
		r.encoding("UTF-8");
		r.comments_allowed(true);

		initialized = true;
		return r;
	}
	//struct init_it { init_it() { xss_filter(); } } instance;

	std::string mymarkdown(const std::string& s)
	{
		int flags = mkd::no_pants;
		if(s.compare(0,10,"<!--toc-->")==0) {
			flags |= mkd::toc;
		}
		std::string html = markdown_format_for_highlighting(markdown_to_html(s.c_str(),s.size(),flags),"cpp");
		return cppcms::xss::filter(html,xss_filter(),cppcms::xss::escape_invalid);
	}

} //local namespace

namespace cppcms {
namespace json {
	template<>
	struct traits <content::wiki>{
		static content::wiki get(value const &v)
		{
			content::wiki c;// = static_cast<content::project>(v.number());//(&apps::site);

			if( v.object().size() != WIKI_STRUCT_SIZE )
				throw bad_value_cast();

			c.wiki_title=v.get<std::string>("title",std::string());
			c.wiki_content=v.get<std::string>("content",std::string());
			c.wiki_sidebar=v.get<std::string>("sidebar",std::string());
			c.wiki_tags=v.get<std::string>("tags",std::string());
		}
		static void set(value &v,content::wiki const &c)
		{
			v.set<std::string>("title",c.wiki_title);
			v.set<std::string>("content",c.wiki_content);
			v.set<std::string>("sidebar",c.wiki_sidebar);
			v.set<std::string>("tags",c.wiki_tags);
		}
	};
} // json
} // cppcms

namespace content {

wiki_edit_form::wiki_edit_form()
{
	save.value(_("save"));
	preview.value(_("preview"));
	post.value(_("post"));

	title.name("title");
	title.help("Title of Wiki");

	content.name("content");
	content.help("Viewent of Wiki");

	sidebar.name("sidebar");
	sidebar.help("Sidebar of Wiki");

	tags.name("tags");
	tags.help("Tags");

	fields.add(title);
	fields.add(content);
	fields.add(sidebar);
	fields.add(tags);

	buttons.add(save);
	buttons.add(preview);
	buttons.add(post);

	add(fields);
	add(buttons);
}

bool wiki_edit_form::validate()
{
	BOOSTER_LOG(debug, __FUNCTION__);
	bool res = form::validate();

	if(!res){
		BOOSTER_LOG(debug, __FUNCTION__) << "form not validated";
		return false;
	}

	if(!tags.value().empty())
	{
		booster::smatch res;
		static const booster::regex tags_regex("[^0-9.\\x00-\\x1F\\x7F\\x22!#$%&'()*+/:;<=>?@\\^_`{|}~-]");
		if(booster::regex_match(tags.value(),res,tags_regex)) {
			BOOSTER_LOG(debug, __FUNCTION__) << "tags not validated";
			return false;
		}
	}
	return res;
}

} //content

class wiki: public Plugin
{
public:
	wiki(cppcms::service &srv)
	: Plugin::Plugin(srv), new_page(false), impl()
	{
		BOOSTER_LOG(debug, __FUNCTION__) << "name=" << wiki_sign::name << ", shortname=" << wiki_sign::shortname << ", slug=" << wiki_sign::slug << ", version=" << wiki_sign::version;

		map_.push_back(wiki_sign::shortname);
		map_.push_back(std::string("/")+wiki_sign::slug+"/{1}");
		map_.push_back(std::string("/")+wiki_sign::slug+"(/(.*))?");
		map_.push_back("2");

		dispatcher().assign("", &wiki::display,this,0);
		mapper().assign("");

		dispatcher().assign("/(\\w+)/?$", &wiki::display,this,1);
		mapper().assign("/{1}");

		dispatcher().assign("/(\\w+)/edit/?$", &wiki::edit,this,1);
		mapper().assign("/edit/{1}");

		dispatcher().assign("/(\\w+)/edit/v/(\\d+)?$", &wiki::editver,this,1,2);
		mapper().assign("/editver/{1}/{2}");

/*		dispatcher().assign("/(\\w+)/?$", &wiki::edit,this,1);
		mapper().assign("/edit{1}");

		dispatcher().assign("/(\\w+)/edit(/ver/(\\d+))?$", &wiki::editver,this,1,3);
		mapper().assign("/editver{1}/{3}");

		dispatcher().assign("wiki", &wiki::display,this,0);
		mapper().assign("wiki","wiki");

*/
	}

	virtual void init(content::wiki& c)
	{
		BOOSTER_LOG(debug, __FUNCTION__);
/*
		if(page_.empty())
			page_ = "home";

		c.wiki_page = page_;
*/
		c.name = name();

	}
/*
------------------------ URL --------------------------
*/
	virtual std::string url_root()
	{
		BOOSTER_LOG(debug, __FUNCTION__) << "root: " << this->root();
		return std::string("/w/home");
	}

	virtual std::string url_edit()
	{
		BOOSTER_LOG(debug, __FUNCTION__) << "root: " << root();
		return std::string("/w/")+page_+"/edit";
	}
	virtual std::string url_page(const std::string& page)
	{
		BOOSTER_LOG(debug, __FUNCTION__) << "root: " << root();
		return std::string("/w/")+page;
	}
/*
------------------------ SAVE/LOAD ----------------------------
*/
	virtual void save_form(content::wiki &c)
	{
		BOOSTER_LOG(debug,__FUNCTION__);
		c.wiki_title = c.wiki_form.title.value();
		c.wiki_content = c.wiki_form.content.value();
		c.wiki_sidebar = c.wiki_form.sidebar.value();
		c.wiki_tags = c.wiki_form.tags.value();
	}

	virtual void load_form(content::wiki &c)
	{
		BOOSTER_LOG(debug,__FUNCTION__);
		c.wiki_form.title.value(c.wiki_title);
		c.wiki_form.content.value(c.wiki_content);
		c.wiki_form.sidebar.value(c.wiki_sidebar);
		c.wiki_form.tags.value(c.wiki_tags);
	}

	virtual void load(const std::string& page, content::wiki& c)
	{
		cppcms::json::value v;
		BOOSTER_LOG(debug, __FUNCTION__) << "page=" << page;

		v = tools::string_to_json(impl.data->driver().get(page, "default"));
		if(v.is_undefined() || v.is_null()) {
			BOOSTER_LOG(debug,__FUNCTION__) << "Can`t load page " << page << " - redirect to edit it";
			response().set_redirect_header(url_edit());
			return;
		}

		//TODO: using get treats
		//c = v.get<content::wiki>(page,content::wiki());

		c.wiki_page = page;
		c.wiki_title = v.get<std::string>("title",std::string());
		c.wiki_content = v.get<std::string>("content",std::string());
		c.wiki_sidebar = v.get<std::string>("sidebar",std::string());
		c.wiki_tags = v.get<std::string>("tags",std::string());

		BOOSTER_LOG(debug, __FUNCTION__) << "title: " << c.wiki_title << ", content: " << c.wiki_content << ", sidebar: " << c.wiki_sidebar << ", tags=" << c.wiki_tags;
	}

	virtual void save(std::string const &page, content::wiki &c)
	{
		std::stringstream ss;
		BOOSTER_LOG(debug,__FUNCTION__) << "page=" << page;
		//std::tm t = booster::ptime::local_time(booster::ptime::now());

		//if(!MDATA.driver().exists(MAUTH.username(),MVIEW.locale_name(),page,"w"))
		if(!impl.data->driver().exists(impl.auth->id(),impl.view->locale_name()+page))
		{
			BOOSTER_LOG(debug,__FUNCTION__) << "Page " << page << " is not exist";
			//return;
		}

		//ioc::get<Data>().store().page_save(page,);
		//booster::nowide::ofstream fout((ioc::get<Data>().store_path() + "/" + page + "." + MVIEW.locale_name()).c_str());

		cppcms::json::value v;
		save_form(c);
		//save json data to file
		v.set<content::wiki>(page,c);

		//MDATA.ref().save(v, page, "w");
		impl.data->driver().set(page, page, v.str());
		//v.save(fout,true);
	}

	//TODO: cache readed pages for faster savings
	virtual void edit(std::string page)
	{
		BOOSTER_LOG(debug, __FUNCTION__) << "path: " << (page.empty() ? "empty": page) << ", new: " << new_page;

		content::wiki c;
		c.wiki_preview = false;
		c.wiki_edit = true;
//		c.wiki_page = upath;

		//if(!MDATA.driver().data_exist(MAUTH.username(),MVIEW.locale_name(),page,"w"))
		if(!impl.data->driver().exists(impl.auth->id(),impl.view->locale_name()+page))
		{
			BOOSTER_LOG(debug, __FUNCTION__) << "page " << page << " is not exists";
			c.wiki_new = new_page = true;
		}
		else {
			c.wiki_new = new_page = false;
		}

		impl.view->post(c);

		if(request().request_method()=="POST")
		{
			c.wiki_form.load(context());
			if(c.wiki_form.validate())
			{
				if(c.wiki_form.save.value())
				{
					BOOSTER_LOG(debug, __FUNCTION__) << "submit save";
					save(page,c);
					cache().rise(std::string("wiki_page_")+impl.view->locale_name()+":"+page);
				}
				if(c.wiki_form.preview.value())
				{
					BOOSTER_LOG(debug, __FUNCTION__) << "submit preview";
					save_form(c);
					c.wiki_preview = true;
					BOOSTER_LOG(debug, __FUNCTION__) << "deb1: c.wiki_content=" << c.wiki_content;
					cache().rise(std::string("wiki_page_")+impl.view->locale_name()+":"+page);
				}
				if(c.wiki_form.post.value())
				{
					BOOSTER_LOG(debug, __FUNCTION__) << "submit post";
					save(page,c);
					cache().rise(std::string("wiki_page_")+impl.view->locale_name()+":"+page);
					response().set_redirect_header(url_page(page));
				}
			}
		}
		//init base content first
		impl.view->init(c);
		init(c);

		if(!c.wiki_preview && !c.wiki_new)
		{
			BOOSTER_LOG(debug, __FUNCTION__) << "loading page " << page;
			load(page,c);
		}

		//c.wiki_title = upath;
		BOOSTER_LOG(debug, __FUNCTION__) << "deb2: c.wiki_content=" << c.wiki_content;
		load_form(c);
		BOOSTER_LOG(debug, __FUNCTION__) << "deb3: c.wiki_content=" << c.wiki_content;
		render("wiki_view","wiki", c);
	}

	virtual void editver(std::string page, std::string ver)
	{
		BOOSTER_LOG(debug, __FUNCTION__) << "path: " << (page.empty() ? "empty" : page);

		if(ver.empty())
		{
			edit(page);
			return;
		}

		content::wiki c;
		//c.wiki_page = upath;

		impl.view->post(c);
		//init base content first
		impl.view->init(c);

		init(c);

		//c.wiki_page = upath;

		render("wiki_view","wiki", c);
	}

	virtual void display(std::string page)
	{
		if(page.empty())
		{
			response().set_redirect_header(url_root());
			return;
		}

		BOOSTER_LOG(debug, __FUNCTION__) << "path: " << page;

		this->page_ = page;

		std::string key="wiki_page_"+impl.view->locale_name()+":"+page;
		if(cache().fetch_page(key))
			return;

		//if(!MDATA.driver().data_exist(MAUTH.username(),MVIEW.locale_name(),page,"w"))
		if(!impl.data->driver().exists(impl.auth->id(),impl.view->locale_name()+page))
		{
			response().set_redirect_header(url_edit());
			return;
		}

		content::wiki c;
//		c.wiki_page = page;
//		c.name = name();

		impl.view->post(c);
		//init base content first
		impl.view->init(c);

		init(c);

		load(page,c);

		c.wiki_edit = false;
		c.wiki_info["version"] = "0.0.1";
		c.wiki_info["file hash"] = impl.auth->sha512(page);
		c.wiki_info["content hash"] = tools::string_hash(c.wiki_content,"sha1");
		c.wiki_link = tools::magnet(c.wiki_content,c.wiki_tags);

		render("wiki_view","wiki", c);
	}

	//Extend base view
	virtual cppcms::base_content& html_css() { return content_; }
	virtual cppcms::base_content& html_js_head() { return content_; }
	virtual cppcms::base_content& html_js_foot() { return content_; }
	virtual cppcms::base_content& html_menu() { return content_; }
	virtual cppcms::base_content& html_left() { return content_; }
	virtual cppcms::base_content& html_right() { return content_; }
	virtual cppcms::base_content& html_top() { return content_; }
	virtual cppcms::base_content& html_bottom() { return content_; }
	virtual cppcms::base_content& html_content() { return content_; }

	virtual bool is_css(){ return false; }
	virtual bool is_js_head(){ return false; }
	virtual bool is_js_foot(){ return false; }
	virtual bool is_menu(){ return true; }
	virtual bool is_left(){ return false; }
	virtual bool is_right(){ return false; }
	virtual bool is_top(){ return false; }
	virtual bool is_bottom(){ return false; }
	virtual bool is_content(){ return false; }

	virtual cppcms::application& get(){
		return *this;
	}
	virtual void prepare(){
		content_.name = name();
	}
	virtual std::string skin(){
		return wiki_sign::shortname + "_view";
	}
	virtual std::string view(const std::string& s){
		return wiki_sign::shortname + "_" + s;
	}
	virtual const std::string& name(){
		return wiki_sign::name;
	}
	virtual const std::string& shortname(){
		return wiki_sign::shortname;
	}
	virtual const std::string& slug(){
		return wiki_sign::slug;
	}
	virtual const std::string& version(){
		return wiki_sign::version;
	}
	virtual tools::vec_str& map(){
		return map_;
	}
/*
	~url(){
		map_.clear();
//		for (std::vector<std::string>::iterator it(map_.begin()); it != map_.end(); ++it)
//			delete *it;
		}
*/

private:
	tools::vec_str map_;
	content::wiki content_;

	std::string page_;
	bool new_page;
	wiki_impl impl;
};

class wiki_rpc : public PluginRpc
{
public:
	wiki_rpc(cppcms::service &srv)
	: PluginRpc::PluginRpc(srv), impl()
	{
		map_.push_back(std::string(wiki_sign::shortname+"_rpc"));
		map_.push_back(std::string("/")+wiki_sign::slug+"_rpc/{1}");
		map_.push_back(std::string("/")+wiki_sign::slug+"_rpc(/(.*))?");
		map_.push_back("2");

		bind("system.listMethods",cppcms::rpc::json_method(&wiki_rpc::methods,this),method_role);

		bind("load",cppcms::rpc::json_method(&wiki_rpc::load,this),method_role);
		bind("add",cppcms::rpc::json_method(&wiki_rpc::add,this),method_role);
		bind("get",cppcms::rpc::json_method(&wiki_rpc::get,this),method_role);
		bind("pos",cppcms::rpc::json_method(&wiki_rpc::pos,this),method_role);
		bind("color",cppcms::rpc::json_method(&wiki_rpc::color,this),method_role);
		bind("desc",cppcms::rpc::json_method(&wiki_rpc::desc,this),method_role);
		bind("del",cppcms::rpc::json_method(&wiki_rpc::del,this),method_role);

		methods_ = boost::assign::list_of ("system.listMethods") ("load") ("add") ("get") ("pos") ("color") ("desc") ("del");
		BOOST_ASSERT( methods_.size() == 8 );
		BOOST_ASSERT( methods_.back() == "del" );
		BOOST_ASSERT( methods_.front() == "system.listMethods" );
	}

	virtual tools::vec_str& map(){
		return map_;
	}

	virtual void methods(){
		return_result(methods_);
	}

	virtual void add(std::string id, int start, int len, std::string col)
	{
		cppcms::json::array item = items_.get<cppcms::json::array>(id, cppcms::json::array());

		if( item.empty() )
		{
			item.push_back(start);
			item.push_back(len);
			item.push_back(col);
		}
		else
		{
			item.at(0) = start;
			item.at(1) = len;
		}
		items_.set<cppcms::json::array>(id, item);

		return_result(cppcms::json::value("nil"));
	}

	virtual void load()
	{
		if(items_.is_null() || items_.is_undefined())
			return_result(cppcms::json::value("nil"));
		else
			return_result(items_);
	}

	virtual void get(std::string id)
	{
		if(items_.is_null() || items_.is_undefined() || items_.at(id).is_null() || items_.at(id).is_undefined() )
			return_result(cppcms::json::value("nil"));
		else
			return_result(items_.get<cppcms::json::array>(id, cppcms::json::array()));
	}

	virtual void pos(std::string id, int start, int len)
	{
		cppcms::json::array a = items_.get<cppcms::json::array>(id, cppcms::json::array());

		if(!a.empty()) {
			a.at(0) = start;
			a.at(1) = len;
			items_.set<cppcms::json::array>(id, a);
		}
		return_result(cppcms::json::value("nil"));
	}

	virtual void color(std::string id, std::string color)
	{
		cppcms::json::array a = items_.get<cppcms::json::array>(id,cppcms::json::array());

		if(!a.empty())
		{
			a.at(2) = color;
			items_.set<cppcms::json::array>(id, a);
		}
		return_result(cppcms::json::value("nil"));
	}

	virtual void desc(std::string id, std::string desc)
	{
		cppcms::json::array a = items_.get<cppcms::json::array>(id,cppcms::json::array());
		if(a.size() > 3)
		{
			a.pop_back();
			a.push_back(desc);
		}
		else
			a.push_back(desc);

		items_.at(id).null();
		items_.set<cppcms::json::array>(id, a);
		return_result(cppcms::json::value("nil"));
	}

	virtual void del(std::string id)
	{
		cppcms::json::value tmp;
		cppcms::json::array a;

		for(cppcms::json::object::const_iterator p=items_.object().begin(); p!=items_.object().end(); p++)
		{
			std::string pid = p->first;

			if(pid != id)
			{
				a = items_.get<cppcms::json::array>(pid,cppcms::json::array());
				tmp.set<cppcms::json::array>(pid, a);
			}
		}
		items_.null();
		items_.swap(tmp);
		return_result(cppcms::json::value("nil"));
	}

private:
	wiki_impl impl;
	tools::vec_str map_;
	cppcms::json::array methods_;
	cppcms::json::value items_;
};

extern "C" void plugin()
{
	cppcms::service &s = ioc::get<View>().service();
	if(ioc::get<Plug>().get(wiki_sign::shortname) == NULL && ioc::get<Plug>().get_rpc(wiki_sign::shortname) == NULL)
	{
		BOOSTER_LOG(debug, __FUNCTION__) << "Try to add the plugin " << wiki_sign::name;
		ioc::get<Plug>().add(wiki_sign::shortname,new wiki(s),new wiki_rpc(s),wiki_sign::api_version);
		BOOSTER_LOG(debug, __FUNCTION__) << "Add plugin's menu items";
		ioc::get<View>().link_add(wiki_sign::name,std::string("/")+wiki_sign::slug);
		ioc::get<View>().menu_add("sidebar",wiki_sign::name);
	}
}
