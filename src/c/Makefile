all: libhashtable.so test_hashtable

test:
	./test_hashtable

CFLAGS=-I$(JAVA_HOME)/include -I$(JAVA_HOME)/include/darwin

javahashtable.h: ../java/magic_powder/Hashtable.java
	javah -o $@ -cp ../java magic_powder.Hashtable

hashtable.o: hashtable.h
javahashtable.o: javahashtable.h hashtable.h

.c.o:
	gcc $(CFLAGS) -c -Wall -Werror -fpic $<

libhashtable.so: hashtable.o javahashtable.o
	gcc -shared -o $@ $+

test_hashtable.o: hashtable.h

test_hashtable: test_hashtable.o libhashtable.so
	gcc -o $@ $+ -L. -lhashtable

clean:
	rm -f libhashtable.so hashtable.o javahashtable.o javahashtable.h test_hashtable test_hashtable.o
