#include <emmintrin.h>
#include "concurrent_counter.hpp"
#include "transactional.hpp"

namespace counter {

void IncorrectConcurrentCounter::increment(std::size_t index) {
  int current = storage[index];
  storage[index] = current + 1;
}

int IncorrectConcurrentCounter::get(std::size_t index) {
  return storage[index];
}

ConcurrentCounter::ConcurrentCounter(std::size_t size) {
  sz = size;
  // parens ensure array elements are initialized to 0
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
  std::lock_guard<spinlock_t> lock(spinlock);
  int current = storage[index];
  storage[index] = current + 1;
}

int CoarseConcurrentCounter::get(std::size_t index) {
  std::lock_guard<spinlock_t> lock(spinlock);
  return storage[index];
}

void FineConcurrentCounter::increment(std::size_t index) {
  std::lock_guard<spinlock_t> lock(spinlocks[index]);
  int current = storage[index];
  storage[index] = current + 1;
}

int FineConcurrentCounter::get(std::size_t index) {
  std::lock_guard<spinlock_t> lock(spinlocks[index]);
  return storage[index];
}

void RTMCoarseConcurrentCounter::increment(std::size_t index) {
  TransactionalScope xact(&spinlock);
  int current = storage[index];
  storage[index] = current + 1;
}

int RTMCoarseConcurrentCounter::get(std::size_t index) {
  TransactionalScope xact(&spinlock);
  return storage[index];
}

void RTMFineConcurrentCounter::increment(std::size_t index) {
  TransactionalScope xact(&spinlocks[index]);
  int current = storage[index];
  storage[index] = current + 1;
}

int RTMFineConcurrentCounter::get(std::size_t index) {
  TransactionalScope xact(&spinlocks[index]);
  return storage[index];
}

void RTMConcurrentCounter::increment(std::size_t index) {
  do { 
    _xbegin();
  } while (!_xtest());
  int current = storage[index];
  storage[index] = current + 1;
  _xend();
}

int RTMConcurrentCounter::get(std::size_t index) {
  int ret = -1;
  do {
    _xbegin();
  } while (!_xtest());
  ret = storage[index];
  _xend();
  return ret;
}

} // namespace counter {
