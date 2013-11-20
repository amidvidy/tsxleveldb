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

Mutex::Mutex() { PthreadCall("init mutex", pthread_mutex_init(&mu_, NULL)); }

Mutex::~Mutex() { PthreadCall("destroy mutex", pthread_mutex_destroy(&mu_)); }

void Mutex::Lock() {
    //PthreadCall("lock", pthread_mutex_lock(&mu_)); 
    
    // Attempt 1
    // xact_ = new TransactionalScope(&mu_, true);

    // Attempt 2
    /*do {
        _xbegin();
    } while (!_xtest());*/
    
    // Attempt 3
    unsigned int xact_status;
    xact_status = _xbegin();
    if (xact_status == _XBEGIN_STARTED) {
        if (pthread_mutex_trylock(&mu_)) {
	    //pthread_mutex_unlock(&mu_);
	    return;
        } else {
	    _xabort(0xFF);
	}
    } else {
	pthread_mutex_lock(&mu_);
    }
}

void Mutex::Unlock() {
    //PthreadCall("unlock", pthread_mutex_unlock(&mu_));
    
    // Attempt 1
    // delete xact_; 

    // Attempt 2
    //_xend();

    // Attempt 3
    if (_xtest()) {
    	_xend();
	pthread_mutex_unlock(&mu_);
    } else {
	pthread_mutex_unlock(&mu_);
    }
}

CondVar::CondVar(Mutex* mu)
    : mu_(mu) {
    PthreadCall("init cv", pthread_cond_init(&cv_, NULL));
}

CondVar::~CondVar() { PthreadCall("destroy cv", pthread_cond_destroy(&cv_)); }

void CondVar::Wait() {
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

}  // namespace port
}  // namespace leveldb
