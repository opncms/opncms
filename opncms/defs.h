#pragma once
///
/// \def OPNCMS_CONF
/// Define name of root key in application config
///
#ifndef OPNCMS_CONF
#define OPNCMS_CONF "opncms."
#endif

///
/// \def OPNCMS_CONF_STORE
/// Define name of key for store in application config
///
#ifndef OPNCMS_CONF_STORE
#define OPNCMS_CONF_STORE "opncms.store"
#endif

///
/// \def OPNCMS_DATA_TYPE
/// Define default type for store
///
#ifndef OPNCMS_DATA_TYPE
#define OPNCMS_DATA_TYPE "sql"
#endif

///
/// \def OPNCMS_DATA_DRIVER
/// Define default driver for store
///
#ifndef OPNCMS_DATA_DRIVER
#define OPNCMS_DATA_DRIVER "sqlite3"
#endif

///
/// \def OPNCMS_DATA_SQL_PARAM
/// Define default connection string for SQL store
///
#ifndef OPNCMS_DATA_SQL_PARAM
#define OPNCMS_DATA_SQL_PARAM "sqlite3:db=./db/site.db;@pool_size=16"
#endif

///
/// \def OPNCMS_DATA_SQL_PARAM_JSON
/// Define default connection string for SQL store
///
#ifndef OPNCMS_DATA_SQL_PARAM_JSON
#define OPNCMS_DATA_SQL_PARAM_JSON "{\"db\": \"./db/site.db\"; \"@pool_size\" : 16}"
#endif

///
/// \def OPNCMS_DATA_FILE_PATH
/// Define default root path for NoSQL store
///
#ifndef OPNCMS_DATA_FILE_PATH
#define OPNCMS_DATA_FILE_PATH "/tmp"
#endif

///
/// \def OPNCMS_DATA_CACHE_TIMEOUT
/// Define default timeout for data caching - 30 min
///
#ifndef OPNCMS_DATA_CACHE_TIMEOUT
#define OPNCMS_DATA_CACHE_TIMEOUT 1800
#endif