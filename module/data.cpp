////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2013-2015 Vladimir Yakunin (kpeo) <opncms@gmail.com>
//
//  The redistribution terms are provided in the COPYRIGHT.txt file
//  that must be distributed with this source code.
//
////////////////////////////////////////////////////////////////////////////

#include <opncms/module/data.h>

// ----------------------------------------- Data -----------------------------------------
//
// default:
//	connection_string = sqlite3:db=./db/site.db;@pool_size=16
//	file.params.path = "./store"
//	store = "sql"
//	sql.driver = "sqlite3"

Data::Data(cppcms::application& app)
:app_(app), dsql_(), dfile_(), dmongodb_()
{
	BOOSTER_LOG(debug,__FUNCTION__);
//	Data::driver("sql", dsql_);
	Data::driver("sqlite3", dsql_);
	Data::driver("postgresql", dsql_);
	Data::driver("odbc", dsql_);
	Data::driver("mysql", dsql_);
	Data::driver("file", dfile_);
	Data::driver("mongodb", dmongodb_);
}

Data::~Data()
{
	BOOSTER_LOG(debug,__FUNCTION__);
	/*
	ioc::del<IData>("Data:mongodb");
	ioc::del<IData>("Data:file");
	ioc::del<IData>("Data:mysql");
	ioc::del<IData>("Data:odbc");
	ioc::del<IData>("Data:postgresql");
	ioc::del<IData>("Data:sqlite3");
	*/
}

IData& Data::driver(const std::string& driver, IData& driverclass)
{
	BOOSTER_LOG(debug,__FUNCTION__) << "Register new driver: " << driver;
	ioc::add<IData>(driverclass, std::string("Data:")+driver);
	return driverclass;
}

IData& Data::driver(const std::string& driver, IData& driverclass, tools::map_str& params)
{
	if (params.size()) {
		for(tools::map_str::const_iterator p=params.begin(); p!=params.end(); ++p) {
			if (!p->second.empty())
				Data::params_[p->first] = p->second;
		}
	}
	if(!driverclass.init(driver, Data::params_))
	{
		BOOSTER_LOG(error,__FUNCTION__) << "Can't init driver";
		throw std::bad_alloc();
	}
	Data::driver_ = driver;
	return Data::driver(driver, driverclass);
}

IData& Data::driver(const std::string& driver, tools::map_str& params)
{
	if (params.size()) {
		for(tools::map_str::const_iterator p=params.begin(); p!=params.end(); ++p) {
			if (!p->second.empty())
				Data::params_[p->first] = p->second;
		}
	}
	Data::driver_ = driver;
	if(!Data::driver().init(driver, params_))
	{
		BOOSTER_LOG(error,__FUNCTION__) << "Can't init driver";
		throw std::bad_alloc();
	}		
	return Data::driver();
}

//return reference to driver instance
IData& Data::driver(const std::string& d)
{
	return ioc::get<IData>(std::string("Data:")+d);
}

//return reference to current driver
IData& Data::driver()
{
	return ioc::get<IData>(std::string("Data:")+Data::driver_);
}

//return the name of current driver
const std::string& Data::driver_name()
{
	return driver_;
}

const std::string& Data::path()
{
	return params_["path"];
}

const cppcms::json::value& Data::settings()
{
	return Data::app_.settings();//return settings_;
}

cppcms::cache_interface& Data::cache()
{
	return Data::app_.cache();
}

cppcms::session_interface& Data::session()
{
	return Data::app_.session();
}

cppcms::http::request& Data::request()
{
	return Data::app_.request();
}
