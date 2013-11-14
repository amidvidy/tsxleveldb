CC=icc
CFLAGS=-Wall -std=c++11 -g -O2

all: bench

concurrent_counter.o: concurrent_counter.cpp
	$(CC) $(CFLAGS) -c concurrent_counter.cpp

transactional.o: transactional.cpp
	$(CC) $(CFLAGS) -c transactional.cpp

bench.o: bench.cpp 
	$(CC) $(CFLAGS) -c bench.cpp

db.o: db.cpp db.hpp
	$(CC) $(CFLAGS) -c db.cpp 

bench: bench.o db.o transactional.o concurrent_counter.o
	$(CC) bench.o db.o transactional.o concurrent_counter.o -o bench -lpthread -ltbb

run: bench
	./bench

clean:
	rm -rf *o bench



