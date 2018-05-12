# opnCMS

[![Join the chat at https://gitter.im/opncms/opncms](https://badges.gitter.im/opncms/opncms.svg)](https://gitter.im/opncms/opncms?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=B3U4YZF4QXV6G&lc=EN&item_name=opnCMS%20donation&item_number=opncms&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donate_SM%2egif%3aNonHosted)
[![Donate with Bitcoin](https://en.cryptobadges.io/badge/micro/1KJbM7yxHFQCGpNzHraDTdstqSfEtW885L)](https://en.cryptobadges.io/donate/1KJbM7yxHFQCGpNzHraDTdstqSfEtW885L)
[![Donate with Ethereum](https://en.cryptobadges.io/badge/micro/0xec44da132d844ab028fbfdbb3323df2b1ad3c8e7)](https://en.cryptobadges.io/donate/0xec44da132d844ab028fbfdbb3323df2b1ad3c8e7)

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
