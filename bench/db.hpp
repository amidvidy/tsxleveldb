#include <map>
#include <string>
#include <mutex>

class DB {
public:
  virtual std::string get(std::string key) = 0;
  virtual void put(std::string key, std::string value) = 0;
};

class CoarseGrainedDB : DB {
public:
  CoarseGrainedDB() = default;
  //  ~CoarseGrainedDB();
  std::string get(std::string key);
  void put(std::string key, std::string value);
private:
  std::mutex mutex;
  std::map<std::string, std::string> storage;
};


