run: test_bcrypt
	LD_LIBRARY_PATH=. ./test_bcrypt
all: test_bcrypt
test_bcrypt: test_bcrypt.cpp bcrypt.h
	g++ test_bcrypt.cpp -L. -lbcrypt -o test_bcrypt
