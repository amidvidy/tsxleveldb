#include <atomic>
#include <map>
#include <chrono>
#include <string>
#include <mutex>

class DB {
public:
  virtual std::string get(std::string key) = 0;
  virtual void put(std::string key, std::string value) = 0;
  virtual std::size_t size() = 0;
/*
TODO: figure out lock_ref as well as how to make an enum of lock types
protected:
  virtual lock_ref lock(std::string key) = 0;
  virtual lock_ref lockr(std::string low, std::string high) = 0;
  */
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

class FineGrainedDB : public DB {
public:
  FineGrainedDB() = default;
  std::string get(std::string key) override;
  void put(std::string key, std::string value) override;
/*
private:
  void xlock(std::string key);
  void slock(std::string key);
  void unlock(std::string key);
*/
};

class LockElidedDB : public DB {
};

class HardwareXactDB : public DB {
};

