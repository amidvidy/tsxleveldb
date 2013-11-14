#include "tbb/spin_rw_mutex.h"

typedef tbb::spin_rw_mutex_v3 spinlock_t;

class TransactionalScope {
public:
  TransactionalScope(spinlock_t *fallback);
  ~TransactionalScope();
private:
  spinlock_t *spinlock;
  int lockstate;
};
