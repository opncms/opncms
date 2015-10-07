////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2013-2015 Vladimir Yakunin (kpeo) <opncms@gmail.com>
//
//  The redistribution terms are provided in the COPYRIGHT.txt file
//  that must be distributed with this source code.
//
////////////////////////////////////////////////////////////////////////////

#pragma once
#ifndef DATA_WIKI_H
#define DATA_WIKI_H

#include <string>
#include <booster/function.h>
#include <booster/regex.h>

#include <cppcms/view.h>
#include <cppcms/form.h>
#include <cppcms/xss.h>

#include <opncms/tools.h>
#include <opncms/base.h>
#include <opncms/module/data.h>
#include <opncms/module/view.h>
#include "markdown.h"

//TODO: use private if possible:
/*
struct A {
        int a;
};

struct B : private A {
        using A::a;
}
*/

typedef std::vector< std::pair<std::string, std::string> > menu_t;

namespace content {

struct wiki_edit_form : public cppcms::form {
	cppcms::widgets::text title;
	cppcms::widgets::textarea content;
	cppcms::widgets::textarea sidebar;
	cppcms::widgets::text tags;

	cppcms::widgets::submit save;
	cppcms::widgets::submit preview;
	cppcms::widgets::submit post;

	cppcms::form fields;
	cppcms::form buttons;

	wiki_edit_form();
	virtual bool validate();
};

struct wiki : public base {
        wiki_edit_form wiki_form;
        bool wiki_edit;
        bool wiki_new;
        bool wiki_preview;
        std::string wiki_page;
        std::string wiki_title;
        std::string wiki_content;
        std::string wiki_sidebar;
        std::string wiki_tags;
	std::string wiki_link;
	std::map <std::string, std::string> wiki_info;
	std::string syntax_highlighter;
	booster::function<std::string(std::string const &)> markdown;
};

/*
struct wiki_edit : public wiki {
        edit_form form;
};
*/
}

#endif
