///////////////////////////////////////////////////////////////////////////////
//                                                                             
//  Copyright (C) 2010-2013 Vladimir V Yakunin (kpeo) <kpeo.y3k@gmail.com>
//                                                                             
//  The redistribution terms are provided in the COPYRIGHT.txt file
//  that must be distributed with this source code.
//
////////////////////////////////////////////////////////////////////////////

#include "bcrypt.h"
#include <iostream>

std::string bcrypt_salt(int count)
{
	char entropy[16];
	std::string ret;

	if (count<4 || count>31) {
		std::cout << __FUNCTION__ << "Invalid count of rounds";
	}
	
	std::ifstream f(RANDOM_DEVICE);
	f.read(entropy, sizeof(entropy));
	if (f.gcount() != sizeof(entropy)) {
		std::cout << __FUNCTION__ << "Unable to get an entropy";
		f.close();
		return "";
	}
	f.close();
	
	char* retval = crypt_gensalt(CRYPT_PREFIX, count, entropy, sizeof(entropy));
	ret = std::string(retval);
	//memset(entropy, 0, sizeof(entropy));

	if (ret.empty()) {
		std::cout << __FUNCTION__ << "Unable to generate a salt";
		return "";
	}
	return ret;
}

std::string bcrypt_salt()
{
	return bcrypt_salt(CRYPT_ROUNDS);
}

std::string bcrypt_hash(const std::string& key,const std::string& salt)
{
	void* data=NULL;
	int size;
	std::string h=crypt_ra(key.c_str(),salt.c_str(),&data,&size);
	free(data);
	return h;
}

int main()
{
	std::string h = "123456789abcdef";
	std::string s = "$2a$12$123456789abcdefghijklm";

	std::cout << "hash=" << h << ", salt=" << bcrypt_salt() << std::endl; 
	std::cout << bcrypt_hash(h,s) << std::endl;
	return 0;
}
