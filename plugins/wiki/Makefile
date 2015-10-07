CC = g++
CFLAGS += -Wall -g -I../../ -I../../contrib/crypt_blowfish-1.2 -I../../contrib/cppformat -I../../contrib/mongo-cxx-driver/build/install/include
BASEVIEW = ../../opncms/base.tmpl

all: libwiki_view.so wiki.so

wiki.so: wiki.o
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -Wl,-soname,wiki.so \
        -o wiki.so wiki.o -lcppcms -lbooster

wiki.o: wiki.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -fPIC -c wiki.cpp

libwiki_view.so: wiki_view.o
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -Wl,-soname,libwiki_view.so \
        -o libwiki_view.so wiki_view.o -lcppcms -lbooster

wiki_view.o: wiki_view.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -fPIC -c wiki_view.cpp

wiki_view.cpp: wiki.tmpl
	cppcms_tmpl_cc -s wiki_view $(BASEVIEW) wiki.tmpl -o wiki_view.cpp

clean:
	rm -f wiki.o wiki_view.cpp wiki_view.o libwiki_view.so wiki.so

.PHONY: all clean

