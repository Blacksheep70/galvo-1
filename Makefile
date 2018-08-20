# gcc galvo.o -shared -Wl,--verbose=99,-Bstatic -lbcm2835 -lm -Wl,-Bdynamic,-soname=libgalvo.so -o libgalvo.so

all: clean shared test main
.PHONY: all

clean:
	rm -rf *.so
	rm -rf *.o
	rm -rf bin/test
	rm -rf lib/*.so
	rm -rf main

main:
	# not really static, still depends on lib/libgalvo.so, path might be hard-coded at runtime
	go build main.go

shared:
	rm -rf libgalvo.so
	gcc -c -fPIC -std=gnu99 galvo.c
	gcc galvo.o -shared -Wl,-Bstatic -lbcm2835 -lm -Wl,-Bdynamic,-soname=libgalvo.so -o lib/libgalvo.so
	rm -rf galvo.o

test:
	rm -rf bin/test
	gcc -c test.c -std=gnu99
	LIBRARY_PATH=./lib gcc test.o -Wl,-rpath,./lib,-rpath,../lib,-lgalvo -o bin/test
	rm -rf test.o
