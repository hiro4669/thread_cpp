#include "rw.hpp"

ReadWriteLock::ReadWriteLock() {
    this->readingReaders = 0;
    this->waitingWriters = 0;
    this->writingWriters = 0;
    this->preferWriter = true;

    pthread_mutex_init(&this->mutex, NULL);
    pthread_cond_init(&this->mcond, NULL);
}

ReadWriteLock::~ReadWriteLock() {
    pthread_mutex_destroy(&this->mutex);
    pthread_cond_destroy(&this->mcond);
}

void ReadWriteLock::readLock() {
    pthread_mutex_lock(&this->mutex);
    while (writingWriters > 0 || (preferWriter && waitingWriters > 0)) {
        pthread_cond_wait(&this->mcond, &this->mutex);
    }
    this->readingReaders++;
    pthread_mutex_unlock(&this->mutex);
}

void ReadWriteLock::readUnlock() {
    pthread_mutex_lock(&this->mutex);
    this->readingReaders--;
    this->preferWriter = true;
    pthread_cond_broadcast(&this->mcond);
    pthread_mutex_unlock(&this->mutex);
}

void ReadWriteLock::writeLock() {
    pthread_mutex_lock(&this->mutex);
    this->waitingWriters++;
    while (readingReaders > 0 || writingWriters > 0) {
        pthread_cond_wait(&this->mcond, &this->mutex);
    }
    this->waitingWriters--;
    this->writingWriters++;
    pthread_mutex_unlock(&this->mutex);
}

void ReadWriteLock::writeUnlock() {
    pthread_mutex_lock(&this->mutex);
    this->writingWriters--;
    this->preferWriter = false;
    pthread_cond_broadcast(&this->mcond);
    pthread_mutex_unlock(&this->mutex);
}
