#include <iostream>
#include "db.hpp"

std::string CoarseGrainedDB::get(std::string key) {
  mutex.lock();
  std::string value = storage[key];
  mutex.unlock();
  return value;
}

void CoarseGrainedDB::put(std::string key, std::string value) {
  mutex.lock();
  storage[key] = value;
  mutex.unlock();
}

std::size_t CoarseGrainedDB::size() {
  size_t size;
  mutex.lock();
  size = storage.size();
  mutex.unlock();
  return size;
}
