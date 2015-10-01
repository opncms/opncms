# opnCMS

opnCMS is an simple environment based on [CppCMS web-framework](http://cppcms.com) for building high-performance modern web applications.

##Features
* Configurable plugin system supported both synchronous (GET/POST) and asyncronous (JSON-RPC) connections to core app.
* Flexible template support for plugins with push and pull styles.
* Independant modules to handle view, data, media, authentication tasks with set of extendable drivers.
* Ready-made plugin's code generator like in modern web-frameworks
* Using CppDB for SQL databases, MongoDB, own NoSQL-driver
* Ability of changing type of storage on the fly (to use it simultaneously)
* Universal interface for all type of storages (with capability of using direct DB drivers)
*  Basic user plugin to handle Bcrypt authorization (including registration, reset/change password features) with email notifications (supports TLS and authentication on SMTP server)
* Basic administration plugin for configuration and pages
