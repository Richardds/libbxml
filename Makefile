#
# BinaryXML library
# 
# Richard Boldiš <richard.boldis@protonmail.com>
# https://github.com/Richardds
# 
#

CC = gcc
AR = ar
RM = rm
MKDIR = mkdir
CFLAGS = -Wall -Wextra -pedantic -std=c11 -O2

all: compile-static-lib compile-dynamic-lib compile-cli

compile-cli: bin/bxml bin/libbxml.a

compile-static-lib: bin/libbxml.so

compile-dynamic-lib: bin/libbxml.so

run:
	./bin/bxml example.bxml > example.xml

clean:
	$(RM) -rf bin/
	$(RM) -rf obj/
	$(RM) -f example.xml

bin/bxml: bxml-static

bxml-static: obj/cli.o bin/libbxml.a
	$(MKDIR) -p bin
	$(CC) -Lbin $(CFLAGS) -static obj/cli.o -o bin/bxml -lbxml

bxml-dynamic: obj/cli.o bin/libbxml.so
	$(MKDIR) -p bin
	$(CC) -Lbin $(CFLAGS) obj/cli.o -o bin/bxml -lbxml

bin/libbxml.a: obj/bxml.o
	$(MKDIR) -p bin
	$(AR) rcs $@ $<

bin/libbxml.so: obj/bxml.o
	$(MKDIR) -p bin
	$(CC) $(CFLAGS) -shared -o $@ $<

obj/cli.o: cli.c
	$(MKDIR) -p obj
	$(CC) $(CFLAGS) -c -o $@ $<

obj/bxml.o: src/bxml.c
	$(MKDIR) -p obj
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<
