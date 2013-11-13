#include <atomic>
#include <map>
#include <chrono>
#include <string>
#include <mutex>
#include <tbb/tbb.h>
#include <tbb/concurrent_hash_map.h>

class DB {
public:
  virtual std::string get(std::string key) = 0;
  virtual void put(std::string key, std::string value) = 0;
  virtual std::size_t size() = 0;
};

class CoarseGrainedDB : public DB {
public:
  CoarseGrainedDB() = default;
   ~CoarseGrainedDB();
  std::string get(std::string key) override;
  void put(std::string key, std::string value) override;
  std::size_t size() override;
  long waitTime() { return _waitTime.count(); };
  long getLockCount() { return _nextTxId - 1L; } 
private:
  void lock();
  void unlock();
  std::atomic<long> _nextTxId = 0;
  std::mutex mutex;
  std::chrono::microseconds _waitTime = 
    std::chrono::microseconds(0);
  std::map<std::string, std::string> storage;
};

//
// From: http://software.intel.com/sites/products/documentation/doclib/tbb_sa/help/tbb_userguide/concurrent_hash_map.htm
class StringHash {
  static std::size_t hash(const std::string& x) {
    std::size_t h = 0;
    for (const char* s = x.c_str(); *s; ++s) {
      h = (h * 17) ^ * s;
    }
    return h;
  }
  //! True if strings are equal
  static bool equal(const std::string& x, const std::string& y) {
    return x == y;
  }
};

typedef tbb::concurrent_hash_map<std::string, std::string, StringHash> tbbmap;

class FineGrainedDB : public DB {
public:
  FineGrainedDB() = default;
  std::string get(std::string key) override;
  void put(std::string key, std::string value) override;
  std::size_t size() override;
private:
  tbbmap storage;
};

class LockElidedDB : public DB {
};

class HardwareXactDB : public DB {
};

