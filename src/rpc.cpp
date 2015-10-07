////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2013-2015 Vladimir Yakunin (kpeo) <opncms@gmail.com>
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
	bind("system.listMethods",cppcms::rpc::json_method(&rpc::list,this),method_role);
	bind("upload",cppcms::rpc::json_method(&rpc::upload,this),method_role);
}

void rpc::list() {
	if( !methods_.size() )
	{
		methods_.push_back("upload");
	}
	return_result(methods_);
}

void rpc::upload(std::string file)
{
	BOOSTER_LOG(debug,__FUNCTION__) << "File=" << file;
	std::vector<booster::shared_ptr<cppcms::http::file> > files = request().files();
	booster::shared_ptr<cppcms::http::file> file_;
	bool set = false;
	
	for(unsigned i=0;i<files.size();i++) {
		BOOSTER_LOG(debug,__FUNCTION__) << "Uploading iteration: " << files[i]->name() << std::endl;
		if (files[i]->name()=="file_upload") {
			file_=files[i];
			set = true;
			break;
		}
	}
	if (set) {
		std::string file_name = file_->filename();
		BOOSTER_LOG(debug,__FUNCTION__) << "Filename=" << file_name << std::endl;
	}
	return_result("Successful!");
}

void rpc::rss()
{
	BOOSTER_LOG(debug,__FUNCTION__);
	return_result("Successful!");
}

} //namespace

///
/// \endcond
///
