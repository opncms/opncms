////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2013-2015 Vladimir Yakunin (kpeo) <opncms@gmail.com>
//
//  The redistribution terms are provided in the COPYRIGHT.txt file
//  that must be distributed with this source code.
//
////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef OPNCMS_M_CONT_H
#define OPNCMS_M_CONT_H

#include <opncms/base.h>
#include <cppcms/localization.h>
#include <cppcms/xss.h>

#include <cppcms/http_file.h> //for ava upload
#include <booster/log.h> //for current log

#include <opncms/module/data.h>
#include <opncms/module/auth.h>
#include <opncms/module/media.h>
#include <cppcms/service.h>

#define _(X) ::cppcms::locale::translate(X)
#define N_(S,P,N)  ::cppcms::locale::translate(S,P,N)

/*
struct PageAlert
{
	bool enabled;
	bool dismiss;
	std::string type;
	std::string text;
};
*/
///
/// \brief View Manager - performs various tasks for preparing and rendering data of base template
/// \class View Manager - performs various tasks for preparing and rendering data of base template
///
class View : public cppcms::base_content
{

public:
	///
	/// \brief Constructor
	///
	View(cppcms::application& /*app*/, cppcms::service& /*srv*/);
	///
	/// \brief Destructor
	///
	~View();
	///
	/// \brief Performs data initialiation for base view
	/// \param c content for base view
	///
	void init(content::base& /*c*/);
	///
	/// \brief Check post data for media or login and call it's handlers
	/// \param c content for base view
	///
	void post(content::base& /*c*/);
	///
	/// \brief Returns locale for current user
	///
	const std::string& locale_name();
	///
	/// \brief Returns path to media
	///
	const std::string& media();
	///
	/// \brief Set Alert struct
	///
	void alert(std::string const& /*text*/, std::string const& /*type*/, bool /*enabled*/, bool /*dismiss*/);
	///
	/// \brief Performs initialization of language 
	///
	void init_lang();
	void load_form(content::base& /*c*/);
	bool load(content::base &/*c*/);
	void fill_menu(const std::string& menu, tools::vec_map& dst_menu);

	void link_add(std::string name,std::string url);
	void menu_add(const std::string& menu, std::string name, size_t pos);
	void menu_add(const std::string& menu, std::string name);

	std::string brand();
	std::string url(const std::string& /*s*/);
	cppcms::service& service();
	cppcms::http::context& context();
	cppcms::http::request& request();
	cppcms::http::response& response();
	cppcms::json::value const& settings();

private:
	cppcms::application& app_;
	cppcms::service& service_;
	cppcms::json::value *settings_;

	std::string media_;
	std::string alert_text;
	std::string alert_type;
	bool alert_enabled;
	bool alert_dismiss;

	std::string cookie_prefix_;
	std::string locale_name_;
	std::string file_;
	tools::map_str languages;

	//static & default menu items
	cppcms::json::value menu_;
	tools::map_str links_;
};

#endif
