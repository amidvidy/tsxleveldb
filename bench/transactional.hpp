#include "tbb/spin_rw_mutex.h"
#include "lock.hpp"


struct threadstate_t {
  /* How many operations have been executed in the current transaction. */
  int curTxLen = 0;

  /* The maximum amount of operations to coalesce into a single transaction. */
  int maxTxLen = 1;

  /* The total number of transactional aborts that have occurred during execution for this thread. */
  int totalAborts = 0;
  
  /* The number of successive aborts that have occurred. */
  int successiveAborts = 0;
};

class TransactionalScope {
public:
  TransactionalScope(spinlock_t *fallback);
  ~TransactionalScope();
private:
  static __thread threadstate_t ts;
  spinlock_t *spinlock;
  int lockstate;
};
