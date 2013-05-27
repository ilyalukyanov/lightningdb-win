CC = gcc
W = -W -Wall -Wno-unused-parameter -Wbad-function-cast
OPT = -O2 -g
CFLAGS = -pthread $(OPT) $(W) $(XCFLAGS)
LDLIBS =
SOLIBS =
prefix= /usr/local

IHDRS = ./liblmdb/lmdb.h
ILIBS = liblmdb.a liblmdb.so
IPROGS = mdb_stat mdb_copy
IDOCS = ./Unix/mdb_stat.1 ./Unix/mdb_copy.1
PROGS = $(IPROGS) my-test mtest mtest2 mtest3 mtest4 mtest5 mtest6
all:	$(ILIBS) $(PROGS)

install: $(ILIBS) $(IPROGS) $(IHDRS)
	cp $(IPROGS) $(DESTDIR)$(prefix)/bin
	cp $(ILIBS) $(DESTDIR)$(prefix)/lib
	cp $(IHDRS) $(DESTDIR)$(prefix)/include
	cp $(IDOCS) $(DESTDIR)$(prefix)/man/man1

clean:
	rm -rf $(PROGS) *.[ao] *.so *~ testdb my-testdb

test: all
	mkdir testdb
	./mtest && ./mdb_stat testdb
	./mtest2 && ./mdb_stat testdb
	./mtest3 && ./mdb_stat testdb
	./mtest4 && ./mdb_stat testdb
	./mtest5 && ./mdb_stat testdb
	./mtest6 && ./mdb_stat testdb

test-my: all
	mkdir my-testdb
	./my-test && ./mdb_stat testdb

test-all: test test-my

liblmdb.a: mdb.o midl.o
	ar rs $@ mdb.o midl.o

liblmdb-debug.a: mdb-debug.o midl.o
	ar rs $@ mdb-debug.o midl.o

liblmdb.so: mdb.o midl.o
	$(CC) $(LDFLAGS) -pthread -shared -o $@ mdb.o midl.o $(SOLIBS)

mdb_stat: mdb_stat.o liblmdb.a
mdb_copy: mdb_copy.o liblmdb.a
my-test: my-test.o liblmdb.a
mtest: mtest.o liblmdb.a
mtest2:	mtest2.o liblmdb.a
mtest3:	mtest3.o liblmdb.a
mtest4:	mtest4.o liblmdb.a
mtest5:	mtest5.o liblmdb.a
mtest6:	mtest6.o liblmdb-debug.a

mdb_copy.o: ./Unix/mdb_copy.c 
	$(CC) -c ./Unix/mdb_copy.c -o mdb_copy.o -I./liblmdb

mdb_stat.o: ./Unix/mdb_stat.c ./liblmdb/lmdb.h
	$(CC) -c ./Unix/mdb_stat.c -o mdb_stat.o -I./liblmdb

my-test.o: ./liblmdb.my-test/mtest.c
	$(CC) -c ./liblmdb.my-test/mtest.c -o my-test.o -I./liblmdb

mtest.o: ./liblmdb.test/mtest.c
	$(CC) -c ./liblmdb.test/mtest.c -o mtest.o -I./liblmdb

mtest2.o: ./liblmdb.test2/mtest2.c
	$(CC) -c ./liblmdb.test2/mtest2.c -o mtest2.o -I./liblmdb

mtest3.o: ./liblmdb.test3/mtest3.c
	$(CC) -c ./liblmdb.test3/mtest3.c -o mtest3.o -I./liblmdb

mtest4.o: ./liblmdb.test4/mtest4.c
	$(CC) -c ./liblmdb.test4/mtest4.c -o mtest4.o -I./liblmdb

mtest5.o: ./liblmdb.test5/mtest5.c
	$(CC) -c ./liblmdb.test5/mtest5.c -o mtest5.o -I./liblmdb

mtest6.o: ./liblmdb.test6/mtest6.c
	$(CC) -c ./liblmdb.test6/mtest6.c -o mtest6.o -I./liblmdb

mdb.o: ./liblmdb/mdb.c ./liblmdb/lmdb.h ./liblmdb/midl.h
	$(CC) $(CFLAGS) -fPIC $(CPPFLAGS) -c ./liblmdb/mdb.c

mdb-debug.o: ./liblmdb/mdb.c ./liblmdb/lmdb.h ./liblmdb/midl.h
	$(CC) $(CFLAGS) -fPIC $(CPPFLAGS) -c ./liblmdb/mdb.c -o mdb-debug.o -D MDB_DEBUG

midl.o: ./liblmdb/midl.c ./liblmdb/midl.h
	$(CC) $(CFLAGS) -fPIC $(CPPFLAGS) -c ./liblmdb/midl.c

%: %.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c lmdb.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

