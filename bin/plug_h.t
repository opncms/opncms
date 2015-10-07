#ifndef DATA_${plug}_H
#define DATA_${plug}_H

#include <cppcms/view.h>
#include <cppcms/form.h>
#include <booster/function.h>
#include <opncms/${plug_base_h}>

namespace content {

struct ${plug}_edit_form : public cppcms::form {
        ${plug}_edit_form();
        virtual bool validate();
};

struct ${plug} : public base {
        ${plug}_edit_form ${plug}_form;
        std::string name;
};

}

#endif
