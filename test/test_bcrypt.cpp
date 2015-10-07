// This code rewriten from project BCrypt.Net by Vladimir V Yakunin (kpeo) 
// <opncms@gmail.com>
// 
// Copyright (c) 2006 Damien Miller <djm@mindrot.org>
// Copyright (c) 2010 Ryan D. Emerle
// 
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#include <iostream>
#include <algorithm>
#include <opncms/bcrypt.h>

const std::string test_data[20][3] = {
{ "",                                   "$2a$06$DCq7YPn5Rq63x1Lad4cll.", "$2a$06$DCq7YPn5Rq63x1Lad4cll.TV4S6ytwfsfvkgY8jIucDrjc8deX1s." },
{ "",                                   "$2a$08$HqWuK6/Ng6sg9gQzbLrgb.", "$2a$08$HqWuK6/Ng6sg9gQzbLrgb.Tl.ZHfXLhvt/SgVyWhQqgqcZ7ZuUtye" },
{ "",                                   "$2a$10$k1wbIrmNyFAPwPVPSVa/ze", "$2a$10$k1wbIrmNyFAPwPVPSVa/zecw2BCEnBwVS2GbrmgzxFUOqW9dk4TCW" },
{ "",                                   "$2a$12$k42ZFHFWqBp3vWli.nIn8u", "$2a$12$k42ZFHFWqBp3vWli.nIn8uYyIkbvYRvodzbfbK18SSsY.CsIQPlxO" },
{ "a",                                  "$2a$06$m0CrhHm10qJ3lXRY.5zDGO", "$2a$06$m0CrhHm10qJ3lXRY.5zDGO3rS2KdeeWLuGmsfGlMfOxih58VYVfxe" },
{ "a",                                  "$2a$08$cfcvVd2aQ8CMvoMpP2EBfe", "$2a$08$cfcvVd2aQ8CMvoMpP2EBfeodLEkkFJ9umNEfPD18.hUF62qqlC/V." },
{ "a",                                  "$2a$10$k87L/MF28Q673VKh8/cPi.", "$2a$10$k87L/MF28Q673VKh8/cPi.SUl7MU/rWuSiIDDFayrKk/1tBsSQu4u" },
{ "a",                                  "$2a$12$8NJH3LsPrANStV6XtBakCe", "$2a$12$8NJH3LsPrANStV6XtBakCez0cKHXVxmvxIlcz785vxAIZrihHZpeS" },
{ "abc",                                "$2a$06$If6bvum7DFjUnE9p2uDeDu", "$2a$06$If6bvum7DFjUnE9p2uDeDu0YHzrHM6tf.iqN8.yx.jNN1ILEf7h0i" },
{ "abc",                                "$2a$08$Ro0CUfOqk6cXEKf3dyaM7O", "$2a$08$Ro0CUfOqk6cXEKf3dyaM7OhSCvnwM9s4wIX9JeLapehKK5YdLxKcm" },
{ "abc",                                "$2a$10$WvvTPHKwdBJ3uk0Z37EMR.", "$2a$10$WvvTPHKwdBJ3uk0Z37EMR.hLA2W6N9AEBhEgrAOljy2Ae5MtaSIUi" },
{ "abc",                                "$2a$12$EXRkfkdmXn2gzds2SSitu.", "$2a$12$EXRkfkdmXn2gzds2SSitu.MW9.gAVqa9eLS1//RYtYCmB1eLHg.9q" },
{ "abcdefghijklmnopqrstuvwxyz",         "$2a$06$.rCVZVOThsIa97pEDOxvGu", "$2a$06$.rCVZVOThsIa97pEDOxvGuRRgzG64bvtJ0938xuqzv18d3ZpQhstC" },
{ "abcdefghijklmnopqrstuvwxyz",         "$2a$08$aTsUwsyowQuzRrDqFflhge", "$2a$08$aTsUwsyowQuzRrDqFflhgekJ8d9/7Z3GV3UcgvzQW3J5zMyrTvlz." },
{ "abcdefghijklmnopqrstuvwxyz",         "$2a$10$fVH8e28OQRj9tqiDXs1e1u", "$2a$10$fVH8e28OQRj9tqiDXs1e1uxpsjN0c7II7YPKXua2NAKYvM6iQk7dq" },
{ "abcdefghijklmnopqrstuvwxyz",         "$2a$12$D4G5f18o7aMMfwasBL7Gpu", "$2a$12$D4G5f18o7aMMfwasBL7GpuQWuP3pkrZrOAnqP.bmezbMng.QwJ/pG" },
{ "~!@#$%^&*()      ~!@#$%^&*()PNBFRD", "$2a$06$fPIsBO8qRqkjj273rfaOI.", "$2a$06$fPIsBO8qRqkjj273rfaOI.HtSV9jLDpTbZn782DC6/t7qT67P6FfO" },
{ "~!@#$%^&*()      ~!@#$%^&*()PNBFRD", "$2a$08$Eq2r4G/76Wv39MzSX262hu", "$2a$08$Eq2r4G/76Wv39MzSX262huzPz612MZiYHVUJe/OcOql2jo4.9UxTW" },
{ "~!@#$%^&*()      ~!@#$%^&*()PNBFRD", "$2a$10$LgfYWkbzEvQ4JakH7rOvHe", "$2a$10$LgfYWkbzEvQ4JakH7rOvHe0y8pHKF9OaFgwUZ2q7W2FFZmZzJYlfS" },
{ "~!@#$%^&*()      ~!@#$%^&*()PNBFRD", "$2a$12$WApznUOJfkEGSmYRfnkrPO", "$2a$12$WApznUOJfkEGSmYRfnkrPOr466oFDCaj4b6HY3EXGvfxm43seyhgC" }
};

//Test method for 'bcrypt_hash(string, string)'
int test_hash()
{
	std::cout << "bcrypt_hash ";
	for (int i = 0; i < 20; i++) {
		std::cerr << ".";
		std::string hashed = bcrypt_hash( test_data[i][0], test_data[i][1] );

		if (hashed != test_data[i][2]) {
			std::cerr << std::endl << "hash[" << hashed << "] != expected[" << test_data[i][2] << "]" << std::endl;
			return 1;
		}
	}
	std::cerr << "Ok" << std::endl;
	return 0;
}

//Test method for 'bcrypt_salt()'
int test_salt(int rounds)
{
	std::cout << "bcrypt_salt ";
	for (int i = 0; i < 20; i++) {
		std::cerr << ".";
		std::string plain = test_data[i][0];
		std::string salt = bcrypt_salt(rounds);
		std::string hashed1 = bcrypt_hash(plain, salt);
		std::string hashed2 = bcrypt_hash(plain, hashed1);

		if (hashed1 != hashed2) {
			std::cerr << std::endl << "hash[" << hashed1 << "] != expected[" << hashed2 << "]" << std::endl;
			return 1;
		}
	}
	std::cerr << "Ok" << std::endl;
	return 0;
}

//Test method for 'bcrypt_salt(int)' (work factor)
int test_salt_int(int rounds)
{
	std::cout << "bcrypt_salt(" << rounds << ") " << std::endl;
	for (int j = 4; j <= rounds; j++) {
		std::cerr << " " << j << " ";
		for (int i = 0; i < 20; i++) {
			std::cerr << ".";
			std::string plain = test_data[i][0];
			std::string salt = bcrypt_salt(j);
			std::string hashed1 = bcrypt_hash(plain, salt);
			std::string hashed2 = bcrypt_hash(plain, hashed1);

			if (hashed1 != hashed2) {
				std::cerr<<"hash[" << hashed1 << "] != expected[" << hashed2 << "]" << std::endl;
				return 1;
			}
		}
		std::cerr << "Ok" << std::endl;
	}
	return 0;
}

//Test method for 'bcrypt_salt(max)' - 
int test_salt_max(int rounds)
{
	std::cerr << "bcrypt_salt max(" << rounds << ") ";
	
	std::string plain = test_data[14][0];
	std::string salt = bcrypt_salt(rounds);

	if (salt.empty()) {
		std::cerr<<"Can`t generate salt" << std::endl;
		return 1;
	}
	std::cerr << "Ok" << std::endl;
	return 0;
}

//Test for correct hashing of non-US-ASCII passwords
int test_international_chars(int rounds)
{
	std::cout << "bcrypt_hash w/ international chars ";

	std::string pw1 = "ππππππππ";
	std::string pw2 = "????????";

	std::string h1 = bcrypt_hash(pw1, bcrypt_salt(rounds));
	if (pw2 == h1) {
		std::cerr<<"hash[" << pw2 << "] == expected[" << h1 << "]" << std::endl;
		return 1;                   
	}

	std::string h2 = bcrypt_hash(pw2, bcrypt_salt(rounds));
	if (pw1 == h2) {
		std::cerr<<"hash[" << pw1 << "] == expected[" << h2 << "]" << std::endl;
		return 1;
	}
	std::cerr << "Ok" << std::endl;
	return 0;
}

int main()
{
	if (test_salt_int(6)==0 && test_salt_max(31)==0 && test_salt(12)==0 && test_hash()==0 && test_international_chars(12)==0) {
		std::cout << "Success" << std::endl;
		return EXIT_SUCCESS;
	}
	else {
		std::cerr << "Fail " << std::endl;
		return EXIT_FAILURE;
	}
}
