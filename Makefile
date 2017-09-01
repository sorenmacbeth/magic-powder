OS = $(shell uname -s)

ifdef MAGIC_POWDER_BUILD_FAT_UBERJAR
all: fat
else
all: slim
endif

slim: target/resources/libhashtable.so test_hashtable.$(OS)

fat: target/resources/libhashtable.Darwin.so target/resources/libhashtable.Linux.so test_hashtable.$(OS)

test:
	./test_hashtable.$(OS)

target/resources/libhashtable.so: src/c/libhashtable.$(OS).so
	cp $< $@

target/resources/libhashtable.%.so: src/c/libhashtable.%.so
	cp $< $@

CFLAGS=-I/System/Library/Frameworks/JavaVM.framework/Versions/Current/Headers -I/opt/java/include -I/opt/java/include/linux -I/usr/lib/jvm/java/include -I/usr/lib/jvm/java/include/linux

src/c/javahashtable.h: src/java/magic_powder/Hashtable.java
	javah -o $@ -cp src/java magic_powder.Hashtable

src/c/hashtable.$(OS).o: src/c/hashtable.h
src/c/javahashtable.$(OS).o: src/c/javahashtable.h src/c/hashtable.h

%.$(OS).o: %.c
	gcc $(CFLAGS) -c -Wall -Werror -fpic -o $@ $<

src/c/libhashtable.$(OS).so: src/c/hashtable.$(OS).o src/c/javahashtable.$(OS).o
	gcc -shared -o $@ $+

ifeq ($(OS), Darwin)
src/c/libhashtable.Linux.so:
	docker run -i --rm=true -v `pwd`:/src -w /src centos:7 bash -c "yum install -y make gcc java-devel && make src/c/libhashtable.Linux.so"
endif

src/c/test_hashtable.$(OS).o: src/c/hashtable.h

test_hashtable.$(OS): src/c/test_hashtable.$(OS).o src/c/libhashtable.$(OS).so
	gcc -o $@ $+ -Lsrc/c -lhashtable.$(OS)

clean:
	rm -f target/resources/libhashtable.so target/resources/libhashtable.*.so src/c/libhashtable.*.so src/c/*.o src/c/javahashtable.h test_hashtable.*
