#include <iostream>
#include <chrono>
#include "db.hpp"

std::string CoarseGrainedDB::get(std::string key) {
  this->lock();
  std::string value = storage[key];
  this->unlock();
  return value;
}

void CoarseGrainedDB::put(std::string key, std::string value) {
  this->lock();
  storage[key] = value;
  this->unlock();
}

std::size_t CoarseGrainedDB::size() {
  size_t size;
  this->lock();
  size = storage.size();
  this->unlock();
  return size;
}

void CoarseGrainedDB::lock() {
  auto start = std::chrono::high_resolution_clock::now();
  ++_nextTxId;
  mutex.lock();
  _waitTime += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start);
}

void CoarseGrainedDB::unlock() {
  mutex.unlock();
}

std::string FineGrainedDB::get(std::string key) {
  tbbmap::const_accessor a;
  bool success = storage.find(a, key);
  if (storage.find(a, key)) {
    return a->second;
  } else {
    return std::string("");
  }
}

void FineGrainedDB::put(std::string key, std::string value) {
  tbbmap::accessor a;
  
  storage.insert(a, key);
  a->second = value;
}

size_t FineGrainedDB::size() {
  return storage.size();
}


