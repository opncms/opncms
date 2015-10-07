/////////////////////////////////////////////////////////////////////////////
//                                                                             
//  Copyright (C) 2010-2013 Vladimir V Yakunin (kpeo) <kpeo.y3k@gmail.com>
//                                                                             
//  The redistribution terms are provided in the COPYRIGHT.txt file
//  that must be distributed with this source code.
//
////////////////////////////////////////////////////////////////////////////

#ifndef OPNCMS_BCRYPT_H
#define OPNCMS_BCRYPT_H

#define __SKIP_GNU	// against the conflict with unistd.h crypt declaration
//#define _OW_SOURCE

//#define _XOPEN_SOURCE
//#include "unistd.h"
#include "ow-crypt.h"
#include <cstdlib>
#include <fstream>

#define RANDOM_DEVICE			"/dev/urandom"
#define CRYPT_PREFIX			"$2a$"
#define CRYPT_ROUNDS			12

std::string bcrypt_salt(int count);
std::string bcrypt_salt();
std::string bcrypt_hash(const std::string& key,const std::string& salt);

#endif
