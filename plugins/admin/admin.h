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

namespace content {

struct admin : public base {
	std::string name;
};

struct admin_config : public base {
	std::string name;
	std::map<std::string, std::string> config;
};

struct admin_pages : public base {
	std::string name;
	std::vector<std::string> pages;
};


}

#endif
