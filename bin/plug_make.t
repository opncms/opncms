CC = g++
CFLAGS += -Wall -g -I../../
BASEVIEW = ${plug_base_tmpl}

all: lib${plug}_view.so ${plug}.so

${plug}.so: ${plug}.o
	$$(CC) $$(CFLAGS) $$(LDFLAGS) -shared -Wl,-soname,${plug}.so \
        -o ${plug}.so ${plug}.o -lcppcms -lbooster

${plug}.o: ${plug}.cpp
	$$(CC) $$(CFLAGS) $$(LDFLAGS) -fPIC -c ${plug}.cpp

lib${plug}_view.so: ${plug}_view.o
	$$(CC) $$(CFLAGS) $$(LDFLAGS) -shared -Wl,-soname,lib${plug}_view.so \
        -o lib${plug}_view.so ${plug}_view.o -lcppcms -lbooster

${plug}_view.o: ${plug}_view.cpp
	$$(CC) $$(CFLAGS) $$(LDFLAGS) -fPIC -c ${plug}_view.cpp

${plug}_view.cpp: ${plug}.tmpl
	cppcms_tmpl_cc -s ${plug}_view $$(BASEVIEW) ${plug}.tmpl -o ${plug}_view.cpp

clean:
	rm ${plug}.o ${plug}_view.cpp ${plug}_view.o lib${plug}_view.so ${plug}.so

.PHONY: all clean

