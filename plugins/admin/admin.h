////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2013-2015 Vladimir Yakunin (kpeo) <opncms@gmail.com>
//
//  The redistribution terms are provided in the COPYRIGHT.txt file
//  that must be distributed with this source code.
//
////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef DATA_admin_H
#define DATA_admin_H

#include <cppcms/view.h>
#include <cppcms/form.h>
#include <booster/function.h>
#include <opncms/base.h>

struct plugin
{
	bool enabled;
	std::string name;
	std::vector<std::string> url;
	std::vector<std::string> rpc;
	std::string skin;
};

struct plugins_config
{
	std::string root;
	std::vector<plugin> plugins;
};

namespace content {

struct admin : public base {
	std::string name;
};

struct admin_config : public base {
	std::string name;
//	std::map<std::string, std::string> config;
	plugins_config plugins;
};

struct admin_pages : public base {
	std::string name;
	std::vector<std::string> pages;
};

}

#endif
