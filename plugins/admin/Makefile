CC = g++
CFLAGS += -Wall -g -I../../ -I../../contrib/crypt_blowfish-1.2/ -I../../contrib/cppformat -I../../contrib/mongo-cxx-driver/build/install/include -L../../
BASEVIEW = ../../opncms/base.tmpl

all: libadmin_view.so admin.so

admin.so: admin.o
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -Wl,-soname,admin.so \
        -o admin.so admin.o -lcppcms -lbooster

admin.o: admin.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -fPIC -c admin.cpp -lbcrypt

libadmin_view.so: admin_view.o
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -Wl,-soname,libadmin_view.so \
        -o libadmin_view.so admin_view.o -lcppcms -lbooster

admin_view.o: admin_view.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -fPIC -c admin_view.cpp -lbcrypt

admin_view.cpp: admin.tmpl $(BASEVIEW)
	cppcms_tmpl_cc -s admin_view $(BASEVIEW) admin.tmpl -o admin_view.cpp

clean:
	rm admin.o admin_view.cpp admin_view.o libadmin_view.so admin.so

.PHONY: all clean

