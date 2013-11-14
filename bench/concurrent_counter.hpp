#include <array>
#include <mutex>
#include "tbb/spin_rw_mutex.h"

namespace counter {

typedef tbb::spin_rw_mutex_v3 spinlock_t;

class ConcurrentCounter {
public:
  ConcurrentCounter(std::size_t size);
  ~ConcurrentCounter();
  virtual void increment(std::size_t index) = 0;
  virtual int get(std::size_t index) = 0;
  size_t size();
  int total(); // default implementation not threadsafe
protected:
  int *storage;
  std::size_t sz;
};

class IncorrectConcurrentCounter : public ConcurrentCounter {
public:
  IncorrectConcurrentCounter(std::size_t size) : ConcurrentCounter(size) {};
  void increment(std::size_t index) override;
  int get(std::size_t index) override;
};

class CoarseConcurrentCounter : public ConcurrentCounter {
public:
  CoarseConcurrentCounter(std::size_t size) : ConcurrentCounter(size) {}
  void increment(std::size_t index) override;
  int get(std::size_t index) override;
protected:
  spinlock_t spinlock;
};

class FineConcurrentCounter : public ConcurrentCounter {
public:
  FineConcurrentCounter(std::size_t size) : ConcurrentCounter(size) {
    spinlocks = new spinlock_t[size];
  }
  ~FineConcurrentCounter() {
    delete spinlocks;
  }
  void increment(std::size_t index) override;
  int get(std::size_t index) override;
protected:
  //std::mutex *mutices;
  spinlock_t *spinlocks;
};

class RTMCoarseConcurrentCounter : public CoarseConcurrentCounter {
public:
  RTMCoarseConcurrentCounter(std::size_t size) : CoarseConcurrentCounter(size) {}
  void increment(std::size_t index) override;
  int get(std::size_t index) override;
};

class RTMFineConcurrentCounter : public FineConcurrentCounter {
public:
  RTMFineConcurrentCounter(std::size_t size) : FineConcurrentCounter(size) {}
  void increment(std::size_t index) override;
  int get(std::size_t index) override;
};

class RTMConcurrentCounter : public ConcurrentCounter {
public:
  RTMConcurrentCounter(std::size_t size) : ConcurrentCounter(size) {}
  void increment(std::size_t index) override;
  int get(std::size_t index) override;
};

} // namespace counter
