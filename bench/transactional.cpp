#include "transactional.hpp"
#include <iostream>
#include <emmintrin.h>
#include <immintrin.h>
#include "tbb/spin_rw_mutex.h"


// todo refactor into types header
typedef tbb::spin_rw_mutex_v3 spinlock_t;

TransactionalScope::TransactionalScope(spinlock_t *fallback_mutex) {
  unsigned int xact_status;
  
  spinlock = fallback_mutex; 

  xact_status = _xbegin();
  if (xact_status == _XBEGIN_STARTED) {
    if (spinlock->try_lock()) {
      // terrible, terrible hack
      spinlock->unlock();
      return;
    } else {
      _xabort(0xFF);
    }
  } else { 
    spinlock->lock();
  }
}

TransactionalScope::~TransactionalScope() {
  if (_xtest()) {
    _xend();
  } else {
    spinlock->unlock();
  }
}
