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

void ReadThread::lock_unlock_test() {
    lock->readLock(this->id);
    for (int i = 0; i < 5; ++i) {
        printf("heavy job by %d(%d)...\n", this->id, i);
        usleep(500000);
    }
    lock->readUnlock(this->id);

}

void* ReadThread::run(void* arg) {
    printf("run Thread %d\n", this->id);
    this->lock_unlock_test();
    return NULL;
}
