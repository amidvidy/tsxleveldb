#include <map>
#include <string>
#include <mutex>

class DB {
public:
  virtual std::string get(std::string key) = 0;
  virtual void put(std::string key, std::string value) = 0;
  virtual std::size_t size() = 0;
};

class CoarseGrainedDB : public DB {
public:
  CoarseGrainedDB() = default;
  //  ~CoarseGrainedDB();
  std::string get(std::string key) override;
  void put(std::string key, std::string value) override;
  std::size_t size() override;
private:
  std::mutex mutex;
  std::map<std::string, std::string> storage;
};

class FineGrainedDB : public DB {
public:
  FineGrainedDB() = default;
  std::string get(std::string key) override;
  void put(std::string key, std::string value) override;
  
};

class LockElidedDB : public DB {
};

class HardwareXactDB : public DB {
};

