#include "transactional.hpp"
#include <iostream>
#include <emmintrin.h>

TransactionalScope::TransactionalScope(std::mutex *fallback) {
  mutex = fallback;
  unsigned int xact_status;
  
  if (xact_status = _xbegin()) {
    if (mutex->try_lock()) {
      // terrible, terrible hack
      mutex->unlock();
      return;
    } else {
      _xabort(1);
    }
  } else { 
    mutex->lock();
  }
}

TransactionalScope::~TransactionalScope() {
  if (_xtest()) {
    _xend();
  } else {
    mutex->unlock();
  }
}
