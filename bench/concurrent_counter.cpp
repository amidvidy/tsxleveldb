#include "concurrent_counter.hpp"

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
