#include <emmintrin.h>

#include "concurrent_counter.hpp"
#include "transactional.hpp"

void IncorrectConcurrentCounter::increment(std::size_t index) {
  int current = storage[index];
  storage[index] = current + 1;
}

int IncorrectConcurrentCounter::get(std::size_t index) {
  return storage[index];
}

ConcurrentCounter::ConcurrentCounter(std::size_t size) {
  // parens ensure array elements are initialized to 0
  sz = size;
  storage = new int[size]();
}

ConcurrentCounter::~ConcurrentCounter() {
  delete [] storage;
}

std::size_t ConcurrentCounter::size() {
  return sz;
}

int ConcurrentCounter::total() {
  int total = 0;
  for (int i = 0; i < sz; ++i) {
    total += storage[i];
  }
  return total;
}

void CoarseConcurrentCounter::increment(std::size_t index) {
  std::lock_guard<std::mutex> lock(mutex);
  int current = storage[index];
  storage[index] = current + 1;
}

int CoarseConcurrentCounter::get(std::size_t index) {
  std::lock_guard<std::mutex> lock(mutex);
  return storage[index];
}

void FineConcurrentCounter::increment(std::size_t index) {
  std::lock_guard<std::mutex> lock(mutices[index]);
  int current = storage[index];
  storage[index] = current + 1;
}

int FineConcurrentCounter::get(std::size_t index) {
  std::lock_guard<std::mutex> lock(mutices[index]);
  return storage[index];
}

void RTMCoarseConcurrentCounter::increment(std::size_t index) {
  TransactionalScope xact(&mutex);
  int current = storage[index];
  storage[index] = current + 1;
}

int RTMCoarseConcurrentCounter::get(std::size_t index) {
  TransactionalScope xact(&mutex);
  return storage[index];
}

void RTMFineConcurrentCounter::increment(std::size_t index) {
  TransactionalScope xact(&mutices[index]);
  int current = storage[index];
  storage[index] = current + 1;
}

int RTMFineConcurrentCounter::get(std::size_t index) {
  TransactionalScope xact(&mutices[index]);
  return storage[index];
}

void RTMConcurrentCounter::increment(std::size_t index) {
  while (_xbegin() != -1) ;
  int current = storage[index];
  storage[index] = current + 1;
  _xend();
}

int RTMConcurrentCounter::get(std::size_t index) {
  int ret = -1;
  while (_xbegin() != -1) ;
  ret = storage[index];
  _xend();
  return ret;
}


