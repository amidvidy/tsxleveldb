CC=icc
CFLAGS=-Wall -std=c++11 -g -O2

all: bench

bench.o: bench.cpp 
	$(CC) $(CFLAGS) -c bench.cpp

db.o: db.cpp db.hpp
	$(CC) $(CFLAGS) -c db.cpp 

bench: bench.o db.o
	$(CC) bench.o db.o -o bench -lpthread

run: bench
	./bench

clean:
	rm -rf *o bench



