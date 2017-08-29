all: target/resources/libhashtable.so test_hashtable

test:
	./test_hashtable

CFLAGS=-I/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers -I/opt/java/include -I/opt/java/include/linux

src/c/javahashtable.h: src/java/magic_powder/Hashtable.java
	javah -o $@ -cp src/java magic_powder.Hashtable

src/c/hashtable.o: src/c/hashtable.h
src/c/javahashtable.o: src/c/javahashtable.h src/c/hashtable.h

.c.o:
	gcc $(CFLAGS) -c -Wall -Werror -fpic -o $@ $<

target/resources/libhashtable.so: src/c/hashtable.o src/c/javahashtable.o
	gcc -shared -o $@ $+

src/c/test_hashtable.o: src/c/hashtable.h

test_hashtable: src/c/test_hashtable.o target/resources/libhashtable.so
	gcc -o $@ $+ -Ltarget/resources -lhashtable

clean:
	rm -f target/resources/libhashtable.so src/c/hashtable.o src/c/javahashtable.o src/c/javahashtable.h test_hashtable src/c/test_hashtable.o
