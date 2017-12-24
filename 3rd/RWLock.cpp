#include "rw.hpp"
#include <unistd.h>

RWLock::RWLock() {
    this->lock = true;
    pthread_mutex_init(&this->mutex, NULL);
    pthread_cond_init(&this->mcond, NULL);
}

RWLock::~RWLock() {
    pthread_mutex_destroy(&this->mutex);
    pthread_cond_destroy(&this->mcond);
}

void RWLock::readLock(int id) {

    pthread_mutex_lock(&this->mutex); // lock

    printf("read lock by %d\n", id);
    while (!this->lock) {
        printf("lock failed wait by %d\n", id);
        pthread_cond_wait(&this->mcond, &this->mutex);
        printf("finish wait by %d\n", id);
    }
    this->lock = false;
    printf("get lock by %d\n", id);

    pthread_mutex_unlock(&this->mutex); // unlock
}

void RWLock::readUnlock(int id) {
    pthread_mutex_lock(&this->mutex);

    printf("read unlock by %d\n", id);
    this->lock = true;
    pthread_cond_broadcast(&this->mcond);

    pthread_mutex_unlock(&this->mutex);
}
