#include "transactional.hpp"
#include <iostream>
#include <emmintrin.h>
#include <immintrin.h>
#include "lock.hpp"

__thread threadstate_t TransactionalScope::ts = threadstate_t();

TransactionalScope::TransactionalScope(spinlock_t *fallback_mutex) {
  unsigned int xact_status;
  
  spinlock = fallback_mutex; 

  // we are already executing transactionally.
  if (_xtest()) return;

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
    if (ts.curTxLen < ts.maxTxLen) {
      ++ts.curTxLen;
      return;
    } else {
      _xend();
      //std::cout << ts.curTxLen << std::endl;
      ts.maxTxLen++; 
      ts.curTxLen = 0;
    }

  } else {
    spinlock->unlock();
  }
}
