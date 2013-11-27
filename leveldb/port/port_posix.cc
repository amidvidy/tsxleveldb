// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "port/port_posix.h"

#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include "util/logging.h"
#include <immintrin.h>

namespace leveldb {
namespace port {

static void PthreadCall(const char* label, int result) {
  if (result != 0) {
    fprintf(stderr, "pthread %s: %s\n", label, strerror(result));
    abort();
  }
}

__thread threadstate_t Mutex::ts_ = {0, 0, 1, 0, 0, 0, 1, 0};

Mutex::Mutex() { PthreadCall("init mutex", pthread_mutex_init(&mu_, NULL)); }

Mutex::~Mutex() { PthreadCall("destroy mutex", pthread_mutex_destroy(&mu_)); }

/* Abort amounts, depending on contention. If this was less hacky, it would be an enum. */
#define HIGH 1
#define MEDIUM 2
#define LOW 4
#define NONE 8

void Mutex::Lock() {
  //PthreadCall("lock", pthread_mutex_lock(&mu_)); 
  unsigned int xact_status;
  ts_.successiveAborts = 0;
  //s we are already executing transactionally, continue.
  if (_xtest()) return;

  do {
    xact_status = _xbegin();

    if (xact_status == _XBEGIN_STARTED) {
      
      if ( mu_.__data.__lock == 0 ) { 
	return;
      } else { 
	_xabort(0xFF); 
      }
    
    } else { 
      /** We have aborted. */
      ++ts_.totalAborts;
      ++ts_.successiveAborts;

      // if we xaborted because the lock was held, acquire the lock
      if ((xact_status & _XABORT_EXPLICIT) && _XABORT_CODE(xact_status) == 0xFF) {
	ts_.maxAborts = HIGH;
	//ts_.maxTxLen = 1;
	break;
      }

      //if xabort:retry or xabort:conflict is set retry
      if (xact_status & (_XABORT_RETRY | _XABORT_CONFLICT)) {
	ts_.maxAborts = MEDIUM;
	//ts_.maxTxLen = 1; // be more conservative with coalescing
      }

      // // if we used too much buffer space inside the transaction half the max transaction length
      if ((xact_status & _XABORT_CAPACITY)) {
	//ts_.maxTxLen = 1;
      }
      _mm_pause();
    }
  } while (ts_.successiveAborts < ts_.maxAborts);

  // Fallback to lock
  pthread_mutex_lock(&mu_);
}

void Mutex::Unlock() {
   if (_xtest()) {
     
     if (ts_.curTxLen < ts_.maxTxLen) {
       ++ts_.curTxLen;
       return;
     }

     _xend();

     int sa = ts_.successiveAborts;
     
     if (sa == 0) {
       ts_.maxTxLen++;
       ts_.maxAborts = NONE;
     } else if (sa <= 2) { 
       // leave maxTxlen steady
       ts_.maxAborts = LOW;
     } else if (sa <= 4) {
       ts_.maxTxLen--;
       ts_.maxAborts = MEDIUM;
     } else if (sa <= 8) {
       ts_.maxTxLen = 1;
       ts_.maxAborts = HIGH;
     }

     ++ts_.totalCommits;

   } else {
     pthread_mutex_unlock(&mu_);
     ts_.maxTxLen = 1;
     ts_.maxAborts = MEDIUM;
   }

}

CondVar::CondVar(Mutex* mu)
    : mu_(mu) {
    PthreadCall("init cv", pthread_cond_init(&cv_, NULL));
}

CondVar::~CondVar() { PthreadCall("destroy cv", pthread_cond_destroy(&cv_)); }

void CondVar::Wait() {
  if (_xtest()) return;
  PthreadCall("wait", pthread_cond_wait(&cv_, &mu_->mu_));
}

void CondVar::Signal() {
  PthreadCall("signal", pthread_cond_signal(&cv_));
}

void CondVar::SignalAll() {
  PthreadCall("broadcast", pthread_cond_broadcast(&cv_));
}

void InitOnce(OnceType* once, void (*initializer)()) {
  PthreadCall("once", pthread_once(once, initializer));
}

__thread long TSXCondVar::tcount_ = 0L;

void TSXCondVar::PrepareWait() {
  tcount_ = counter_.load(std::memory_order_relaxed);
}

void TSXCondVar::CompleteWait() {
  int result = 0;
  
  
}



}  // namespace port
}  // namespace leveldb
