#include "tbb/spin_rw_mutex.h"
#include "lock.hpp"

namespace bench {

// thread-local state
struct threadstate_t {
  /* How many times this thread has entered transactional scope. */
  int txCount = 0;

  /* How many operations have been executed in the current transaction. */
  int curTxLen = 0;

  /* The maximum amount of operations to coalesce into a single transaction. */
  int maxTxLen = 1;

  /* the total number of transactional commits that have occurred during execution for this thread. */
  int totalCommits = 0;

  /* The total number of transactional aborts that have occurred during execution for this thread. */
  int totalAborts = 0;
  
  /* The number of successive aborts that have occurred. */
  int successiveAborts = 0;

  /* The maximum number of aborts that can occur before the lock is acquired. */
  int maxAborts = 1;

  /* How many times the fallback was taken. */
  int fallbackTaken = 0;
} __attribute__((aligned(64)));

class TransactionalScope {
public:
  TransactionalScope(spin_rw_mutex &fallback, bool writeAccess = false);
  ~TransactionalScope();
  static void printStats();
private:
  static __thread threadstate_t ts;
  spinlock_t &spinlock;
  int lockstate;
};

} // namespace bench
