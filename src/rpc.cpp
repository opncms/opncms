////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2013-2016 Vladimir Yakunin (kpeo) <opncms@gmail.com>
//
//  The redistribution terms are provided in the COPYRIGHT.txt file
//  that must be distributed with this source code.
//
////////////////////////////////////////////////////////////////////////////

#include <opncms/site.h>
#include <opncms/rpc.h>

///
/// \cond internal
///

namespace apps {

rpc::rpc(cppcms::service& srv, cppcms::application& app)
:cppcms::rpc::json_rpc_server(srv),
app_(app)
{
	bind("system.listMethods",cppcms::rpc::json_method(&rpc::methods,this),method_role);
	bind("upload",cppcms::rpc::json_method(&rpc::upload,this),method_role);
	bind("rss",cppcms::rpc::json_method(&rpc::rss,this),method_role);
	bind("set_locale",cppcms::rpc::json_method(&rpc::set_locale,this),method_role);

	methods_ = boost::assign::list_of ("system.listMethods")("upload")("rss")("set_locale");
	BOOST_ASSERT( methods_.size() == 4 );
}

void rpc::methods()
{
	BOOSTER_LOG(debug, __FUNCTION__);
	return_result(methods_);
}

void rpc::upload(std::string file)
{
	BOOSTER_LOG(debug,__FUNCTION__) << "file(" << file << ")";
	std::vector<booster::shared_ptr<cppcms::http::file> > files = request().files();
	booster::shared_ptr<cppcms::http::file> file_;
	bool set = false;
	
	for(unsigned i=0;i<files.size();i++) {
		BOOSTER_LOG(debug,__FUNCTION__) << "Uploading iteration: " << files[i]->name();
		if (files[i]->name()=="file_upload") {
			file_=files[i];
			set = true;
			break;
		}
	}
	if (set) {
		std::string file_name = file_->filename();
		BOOSTER_LOG(debug,__FUNCTION__) << "filename(" << file_name << ")";
	}
	return_result("ok");
}

void rpc::rss()
{
	BOOSTER_LOG(debug,__FUNCTION__);
	return_result("ok");
}

void rpc::set_locale(std::string lang)
{
	BOOSTER_LOG(debug,__FUNCTION__) << "language(" << lang << ")";
	ioc::get<View>().locale(lang);
	//app_.context().locale(lang);
	return_result("ok");
}

} //namespace

///
/// \endcond
///
