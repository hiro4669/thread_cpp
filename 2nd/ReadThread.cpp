#include <stdio.h>
#include <unistd.h>
#include "rw.hpp"

ReadThread::ReadThread() {
}

ReadThread::ReadThread(RWLock *lock) {
    this->id = tcount;
    this->lock = lock;
}
ReadThread::~ReadThread() {
}

void* ReadThread::run(void* arg) {
    printf("run Thread %d\n", this->id);
	lock->invoke(this->id);
	//lock->invoke_without_lock(this->id);
    return NULL;
}
