#include "transactional.hpp"
#include <thread>
#include <iostream>
#include <emmintrin.h>
#include <immintrin.h>
#include <tbb/spin_rw_mutex.h>

namespace bench {

__thread threadstate_t TransactionalScope::ts = threadstate_t();

  TransactionalScope::TransactionalScope(spin_rw_mutex &fallback_mutex, bool writeAccess) : 
    // initializaer list
    spinlock(fallback_mutex) 
  {
  unsigned int xact_status;

  ts.txCount++;

  // we are already executing transactionally.
  if (_xtest()) return;
  xact_status = _xbegin();
  if (xact_status == _XBEGIN_STARTED) {
    if ( !ts.isLocked ) {
      // terrible, terrible hack
      return;
    } else {
      _xabort(0xFF);
    }
  } else { 
    /** We have aborted. */
    ts.totalAborts++;

    if (writeAccess) { spinlock.lock(); } 
    else { spinlock.lock_read(); }
    
    ts.isLocked = true;
  }
}

TransactionalScope::~TransactionalScope() {
  if (_xtest()) {
    if (ts.curTxLen < ts.maxTxLen) {
      ++ts.curTxLen;
      return;
    } else {
      _xend();
      ts.maxTxLen++; 
      ts.curTxLen = 0;
    }

  } else {
    spinlock.unlock();
    ts.isLocked = false;
  }
}

void TransactionalScope::printStats() {
  std::cout << "Thread Id: " << std::this_thread::get_id() << std::endl;
  std::cout << "txCount: " << std::this_thread::get_id() << std::endl;
  std::cout << "totalAborts: " << std::this_thread::get_id() << std::endl;
}

}  // namespace bench {

